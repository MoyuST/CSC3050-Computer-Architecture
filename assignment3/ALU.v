module aluSeg(i_datain, gr1, gr2, zero_wire, negative_wire, overflow_wire, c, hi_wire, lo_wire);

//output result
output signed[31:0] c;

//flags
output zero_wire, negative_wire, overflow_wire;

//lo, hi
output [31:0] lo_wire, hi_wire;

input [31:0] i_datain, gr1, gr2;

// wire [31:0] imm_wire;

reg [5:0] opcode, funct;
reg [4:0] shamt;
reg [4:0] aluctr;
reg alusrc;
reg [31:0] reg_A, reg_B;
reg [31:0] imm;

//ALUcontrol
always @(i_datain, gr1, gr2) 
begin
    reg_A = gr1;
    reg_B = gr2;
    // reg_C = c;
    opcode = i_datain[31:26];
    funct = i_datain[5:0];
    shamt = i_datain[10:6];
    imm = {{16{i_datain[15]}} ,i_datain[15:0]};

    if (opcode==6'b00_0000) 
    begin
    
        alusrc = 1'b0;

        //add
        if(funct==6'h20)
        begin
            aluctr = 5'b0_0000;
        end   

        //addu
        else if(funct==6'h21)
        begin
            aluctr = 5'b0_0001;
        end

        //sub
        else if(funct==6'h22)
        begin
            aluctr = 5'b0_0010;
        end

        //subu
        else if(funct==6'h23)
        begin
            aluctr = 5'b0_0011;
        end

        //mult
        else if(funct==6'h18)
        begin
            aluctr = 5'b0_0100;
        end

        //multu
        else if(funct==6'h19)
        begin
            aluctr = 5'b0_0101;
        end

        //div
        else if(funct==6'h1a)
        begin
            aluctr = 5'b0_0110;
        end

        //divu
        else if(funct==6'h1b)
        begin
            aluctr = 5'b0_0111;
        end

        //and
        else if(funct==6'h24)
        begin
            aluctr = 5'b0_1000;
        end

        //nor
        else if(funct==6'h27)
        begin
            aluctr = 5'b0_1001;
        end

        //or
        else if(funct==6'h25)
        begin
            aluctr = 5'b0_1010;
        end

        //xor
        else if(funct==6'h26)
        begin
            aluctr = 5'b0_1011;
        end

        //slt
        else if(funct==6'h2a)
        begin
            aluctr = 5'b0_1100;
        end

        //sltu
        else if(funct==6'h2b)
        begin
            aluctr = 5'b0_1101;
        end

        //sll
        else if(funct==6'h0)
        begin
            aluctr = 5'b1_0000;
        end

        //sllv
        else if(funct==6'h4)
        begin
            aluctr = 5'b1_0001;
        end

        //srl
        else if(funct==6'h2)
        begin
            aluctr = 5'b1_0010;
        end

        //srlv
        else if(funct==6'h6)
        begin
            aluctr = 5'b1_0011;
        end

        //sra
        else if(funct==6'h3)
        begin
            aluctr = 5'b1_0100;
        end

        //srav
        else if(funct==6'h7)
        begin
            aluctr = 5'b1_0101;
        end

        else
        begin
            aluctr = 5'b1_1111;
        end
    end

    //handle the zero extend instructions
    //xori andi ori
    else if(opcode==6'he||opcode==6'hc||opcode==6'hd)
    begin

        imm = {{16{1'b0}}, i_datain[15:0]};
        
        //xori
        if(opcode==6'he)
        begin
            aluctr = 5'b0_1011;
            alusrc = 1'b1;
        end
        
        //andi
        else if(opcode==6'hc)
        begin
            aluctr = 5'b0_1000;
            alusrc = 1'b1;
        end

        //ori
        else
        begin
            aluctr = 5'b0_1010;
            alusrc = 1'b1;
        end

    end

    //addiu
    else if(opcode==6'h9)
    begin
        aluctr = 5'b0_0001;
        alusrc = 1'b1;
    end

    //addi
    else if(opcode==6'h8)
    begin
        aluctr = 5'b0_0000;
        alusrc = 1'b1;
    end

    //beq bne(same as the sub)
    else if(opcode==6'h4||opcode==6'h5)
    begin
        aluctr = 5'b0_0010;
        alusrc = 1'b0;
    end

    //slti
    else if(opcode==6'ha)
    begin
        aluctr = 5'b0_1100;
        alusrc = 1'b1;
    end

    //sltiu
    else if(opcode==6'hb)
    begin
        aluctr = 5'b0_1101;
        alusrc = 1'b1;
    end

    //lw
    else if(opcode==6'h23)
    begin
        aluctr = 5'b0_1110;
        alusrc = 1'b1;
    end

    //sw
    else if(opcode==6'h2b)
    begin
        aluctr = 5'b0_1111;
        alusrc = 1'b1;
    end

    //invalid input
    else
    begin
        aluctr = 5'b1_1111;
        alusrc = 1'b1;
    end

end

alu alu0(aluctr, alusrc, gr1, gr2, imm, shamt, zero_wire, negative_wire, overflow_wire, c, hi_wire, lo_wire);

endmodule


module alu(aluctr, alusrc, gr1, gr2, imm, shamt, zero, negative, overflow, c, hi_wire, lo_wire);

//output result
output signed[31:0] c;

//flags
output zero, negative, overflow;

//lo, hi
output signed[31:0]lo_wire, hi_wire;

input signed[31:0] gr1,gr2;
input [4:0] shamt;
input [31:0] imm;
input [4:0] aluctr;
input alusrc;


reg zero, negative, overflow;
reg signed[31:0] reg_C, reg_A, reg_B;
reg [31:0] unsigned_reg_B, unsigned_reg_A;
reg [31:0] lo, hi;

//used to dectect overflow
reg extra;


reg signed [31:0] lo_t, hi_t;
// reg signed[31:0] imm;
// reg[5:0] opcode, func;

parameter gr0 = 32'h0000_0000;
parameter Width = 32;
parameter MSB = Width - 1;

always @(aluctr,alusrc,imm,gr1,gr2)
begin
    if(alusrc==1'b0)
    begin  
        reg_B = gr2;
        unsigned_reg_B = gr2;
    end

    else
    begin
        reg_B = imm;
        unsigned_reg_B = imm;
    end



    //add
    if(aluctr==5'b0_0000)
    begin
        reg_A = gr1;
        {extra, reg_C} = {reg_A[MSB], reg_A} + {reg_B[MSB], reg_B};
        zero = reg_C ? 0 : 1;
        negative = reg_C[MSB];
        overflow = extra ^ reg_C[MSB];
    end

    //addu
    else if(aluctr==5'b0_0001)
    begin
        reg_A = gr1;
        unsigned_reg_A = gr1;
        reg_C = unsigned_reg_A + unsigned_reg_B;
        zero = reg_C ? 0 :1;
        negative = 1'b0;
        overflow = 1'b0;
    end

    //sub
    else if(aluctr==5'b0_0010)
    begin
        reg_A = gr1;
        {extra, reg_C} = {reg_A[MSB], reg_A} - {reg_B[MSB], reg_B};
        zero = reg_C ? 0 : 1;
        negative = reg_C[MSB];
        overflow = extra ^ reg_C[MSB];
    end

    //subu
    else if(aluctr==5'b0_0011)
    begin
        reg_A = gr1;
        unsigned_reg_A = gr1;
        reg_C =  unsigned_reg_A - unsigned_reg_B;
        zero = reg_C ? 0 :1;
        negative = 1'b0;
        overflow = 1'b0;
    end

    //mult
    else if(aluctr==5'b0_0100)
    begin
        reg_A = gr1;
        {hi, lo} = reg_A * reg_B;
        zero = {hi, lo} ? 0 :1;
        negative = hi[MSB];
        overflow = 1'b0;
    end

    //multu
    else if(aluctr==5'b0_0101)
    begin
        reg_A = gr1;
        unsigned_reg_A = gr1;
        {hi, lo} = unsigned_reg_A * unsigned_reg_B;
        zero = {hi, lo} ? 0: 1;
        negative = 1'b0;
        overflow = 1'b0;
    end

    //div
    else if(aluctr==5'b0_0110)
    begin
        reg_A = gr1;
        if(reg_A==32'h8000_0000 && reg_B==-1)
            overflow = 1;
        else
            overflow = 0;

        lo = reg_A / reg_B;
        hi = reg_A % reg_B;
        zero = reg_A ? 0 : 1;
        negative = lo[MSB];
    end

    //divu
    else if(aluctr==5'b0_0111)
    begin
        reg_A = gr1;
        unsigned_reg_A = gr1;
        lo = unsigned_reg_A / unsigned_reg_B;
        hi = unsigned_reg_A % unsigned_reg_B;
        zero = unsigned_reg_A ? 0 : 1;
        negative = 1'b0;
        overflow = 1'b0;
    end

    //and
    else if(aluctr==5'b0_1000)
    begin
        reg_A = gr1;
        reg_C = reg_A & reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //nor
    else if(aluctr==5'b0_1001)
    begin
        reg_A = gr1;
        reg_C = ~(reg_A | reg_B);
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //or
    else if(aluctr==5'b0_1010)
    begin
        reg_A = gr1;
        reg_C = reg_A | reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //xor
    else if(aluctr==5'b0_1011)
    begin
        reg_A = gr1;
        reg_C = reg_A ^ reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //slt
    else if(aluctr==5'b0_1100)
    begin
        reg_A = gr1;
        reg_C = reg_A < reg_B;
        overflow = 1'b0;
        negative = 1'b0;
        zero = reg_C ? 0 : 1;
    end

    //sltu
    else if(aluctr==5'b0_1101)
    begin
        reg_A = gr1;
        unsigned_reg_A = gr1;
        reg_C = unsigned_reg_A < unsigned_reg_B;
        overflow = 1'b0;
        negative = 1'b0;
        zero = reg_C ? 0 : 1;
    end

    //lw
    else if(aluctr==5'b0_1110)
    begin
        reg_A = gr1;
        reg_C = reg_A + reg_B;
        overflow = 1'b0;
        negative = 1'b0;
        zero = reg_C ? 0 : 1;
    end

    //sw
    else if(aluctr==5'b0_1111)
    begin
        reg_A = gr1;
        reg_C = reg_A + reg_B;
        overflow = 1'b0;
        negative = 1'b0;
        zero = reg_C ? 0 : 1;
    end
    
    //sll
    else if(aluctr==5'b1_0000)
    begin
        reg_A = gr1;
        reg_B = {{27{1'b0}}, shamt};
        reg_C = reg_A << reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //sllv
    else if(aluctr==5'b1_0001)
    begin
        reg_A = gr1;
        reg_C = reg_A << reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //srl
    else if(aluctr==5'b1_0010)
    begin
        reg_A = gr1;
        reg_B = {{27{1'b0}}, shamt};
        reg_C = reg_A >> reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //srlv
    else if(aluctr==5'b1_0011)
    begin
        reg_A = gr1;
        reg_C = reg_A >> reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //sra
    else if(aluctr==5'b1_0100)
    begin
        reg_A = gr1;
        reg_B = {{27{1'b0}}, shamt};
        reg_C = reg_A >>> reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //srav
    else if(aluctr==5'b1_0101)
    begin
        reg_A = gr1;
        reg_C = reg_A >>> reg_B;
        overflow = 1'b0;
        negative = reg_C[MSB];
        zero = reg_C ? 0 : 1;
    end

    //invalid
    else
    begin
        $display("          invalid input");
    end


end


assign c = reg_C[31:0];
assign lo_wire = lo;
assign hi_wire = hi;
endmodule