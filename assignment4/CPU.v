`timescale 1ns / 1ps
module CPU(clock, start, i_datain);
    input clock;
    input start;
    input [319:0] i_datain;

    //general registers
    reg [31:0]gr[31:0];

    //instruction memory
    reg [7:0] i_memory[1023:0];

    //data memory
    reg [7:0] d_memory[1023:0];

    //fetch
    reg [31:0] PC;
    reg [31:0] PCF;
    reg [31:0] PCPlus4F;
    reg [31:0] RDF;

    //decoding

    //instruction holder
    reg [31:0] InstrD;
    reg [31:0] PCPlus4D;

    //registers from control unit
    reg RegWriteD;
    reg MemtoRegD;
    reg MemWriteD;
    reg BranchD;
    reg [4:0] ALUControlD;
    reg ALuSrcD;
    reg RegDstD;
    reg ShamtD;
    reg JumpD;
    reg JalD;
    reg JrD;
    reg MemreadD;

    reg [25:0] TargetD;
    reg [31:0] ShamtImmD;
    reg [4:0] A1D;
    reg [4:0] A2D;
    reg [4:0] A3D;
    reg [31:0] WD3D;
    reg [31:0] RD1D;
    reg [31:0] RD2D;
    reg [31:0] SignExtend;
    reg [4:0] shamt;
    reg [4:0] rt;
    reg [4:0] rd;

    //execution
    reg RegWriteE;
    reg MemtoRegE;
    reg MemWriteE;
    reg BranchE;
    reg [4:0] ALUControlE;
    reg ALuSrcE;
    reg RegDstE;
    reg ShamtE;
    reg JalE;
    reg RsHazardE;
    reg RtHazardE;
    reg MemreadE;
    reg stall;

    reg [31:0] ShamtImmE;
    reg [4:0] RtE;
    reg [4:0] RdE;
    reg [4:0] WriteRegE;
    reg [31:0] SignImmE;
    reg [31:0] PCPlus4E;
    reg [31:0] SrcAE;
    reg [31:0] SrcBE;
    reg [31:0] WriteDataE;
    reg [31:0] ALUOutE;
    reg [31:0] PCBranchE;
    reg ZeroE;
    reg negativeE;
    reg overflowE;

    //memory
    reg RegWriteM;
    reg MemtoRegM;
    reg MemWriteM;
    reg BranchM;
    reg RsHazardM;
    reg RtHazardM;

    reg ZeroM;
    reg PCSrcM;
    reg [31:0] AM;
    reg [31:0] WDM;
    reg [31:0] ALUOutM;
    reg [4:0] WriteRegM;
    reg [31:0] PCBranchM;
    reg [31:0] RDM;

    //write
    reg RegWriteW;
    reg MemtoRegW;

    reg [31:0] ALUOutW;
    reg [31:0] ResultW;
    reg [4:0] WriteRegW;

always @(start)
    begin
        gr[0] = 32'h0000_0000;
        PC = 32'h0000_0000;
        PCSrcM = 1'b0;
        JumpD = 1'b0;
        PCPlus4F = 32'h0000_0000;
        RsHazardE = 1'b0;
        RtHazardE = 1'b0;
        RsHazardM = 1'b0;
        RtHazardM = 1'b0;
        stall = 1'b0;
        i_memory[0]=i_datain[319:312];
        i_memory[1]=i_datain[311:304];
        i_memory[2]=i_datain[303:296];
        i_memory[3]=i_datain[295:288];
        i_memory[4]=i_datain[287:280];
        i_memory[5]=i_datain[279:272];
        i_memory[6]=i_datain[271:264];
        i_memory[7]=i_datain[263:256];
        i_memory[8]=i_datain[255:248];
        i_memory[9]=i_datain[247:240];
        i_memory[10]=i_datain[239:232];
        i_memory[11]=i_datain[231:224];
        i_memory[12]=i_datain[223:216];
        i_memory[13]=i_datain[215:208];
        i_memory[14]=i_datain[207:200];
        i_memory[15]=i_datain[199:192];
        i_memory[16]=i_datain[191:184];
        i_memory[17]=i_datain[183:176];
        i_memory[18]=i_datain[175:168];
        i_memory[19]=i_datain[167:160];
        i_memory[20]=i_datain[159:152];
        i_memory[21]=i_datain[151:144];
        i_memory[22]=i_datain[143:136];
        i_memory[23]=i_datain[135:128];
        i_memory[24]=i_datain[127:120];
        i_memory[25]=i_datain[119:112];
        i_memory[26]=i_datain[111:104];
        i_memory[27]=i_datain[103:96];
        i_memory[28]=i_datain[95:88];
        i_memory[29]=i_datain[87:80];
        i_memory[30]=i_datain[79:72];
        i_memory[31]=i_datain[71:64];
        i_memory[32]=i_datain[63:56];
        i_memory[33]=i_datain[55:48];
        i_memory[34]=i_datain[47:40];
        i_memory[35]=i_datain[39:32];
        i_memory[36]=i_datain[31:24];
        i_memory[37]=i_datain[23:16];
        i_memory[38]=i_datain[15:8];
        i_memory[39]=i_datain[7:0];
    end

//the circuit of the PC
always @(PCPlus4F, PCBranchM, PCSrcM, JumpD, TargetD, JalD, JrD, RsHazardE, RsHazardM, A1D, A3D)
    begin
        if(JrD==1'b1)
        begin
            if(RsHazardE==1'b1)
            begin
                PC = ALUOutE;
            end

            else if(RsHazardM==1'b1)
            begin
                PC = MemtoRegM ? {d_memory[ALUOutM],d_memory[ALUOutM+1],d_memory[ALUOutM+2],d_memory[ALUOutM+3]} : ALUOutM;
            end

            else if(A1D==A3D)
            begin
                PC = WD3D;
            end

            else
            begin
                PC = gr[A1D];
            end
        end

        else if(JumpD==1'b1||JalD==1'b1)
        begin
            PC = TargetD;
        end

        else if(PCSrcM==1'b1)
        begin
            PC = PCBranchM;
        end

        else
        begin
            PC = PCPlus4F;
        end
    end

//within the fetch part
always @(posedge clock)
    begin
        if(stall==1'b0)
        begin
            PCF <= PC;
        end
    end

//the circuits of data fectch
always @(PCF)
    begin
        RDF = {i_memory[PCF], i_memory[PCF+1], i_memory[PCF+2], i_memory[PCF+3]};
        PCPlus4F = PCF + 4;
    end

//fetch/decode
always @(posedge clock)
    begin
        if(PCSrcM==1'b1)
        begin
            InstrD <= 32'h0000_0000;
            PCPlus4D <= PCPlus4F;
        end

        else if(stall==1'b0)
        begin
            if(JumpD==1'b1||JalD==1'b1||JrD==1'b1)
            begin
                InstrD <= 32'hx;
            end

            else
            begin
                InstrD <= RDF;
            end
            PCPlus4D <= PCPlus4F;
        end
    end

//the circuits of the data decoding
always @(InstrD, WriteRegW, ResultW)
    begin
        A1D = InstrD[25:21];
        A2D = InstrD[20:16];
        A3D = WriteRegW;
        WD3D = ResultW;
        rt = InstrD[20:16];
        rd = InstrD[15:11];
        SignExtend = {{16{InstrD[15]}}, InstrD[15:0]};
        ShamtImmD = {{16{1'b0}}, InstrD[10:6]};
        JumpD = 1'b0;
        JalD = 1'b0;
        JrD = 1'b0;
        MemreadD = 1'b0;
        TargetD = {PCPlus4D[31:28], InstrD[25:0], 2'b00};

        //control unit
        if(InstrD[31:26]==6'b00_0000)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b1;
            ALuSrcD = 1'b0;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            
            //add
            if(InstrD[5:0]==6'h20)
            begin
                ALUControlD = 5'b0_0000;
            end

            //addu
            if(InstrD[5:0]==6'h21)
            begin
                ALUControlD = 5'b0_1011;
            end

            //sub
            else if(InstrD[5:0]==6'h22||InstrD[5:0]==6'h23)
            begin
                ALUControlD = 5'b0_0001;
            end

            //subu
            else if(InstrD[5:0]==6'h23)
            begin
                ALUControlD = 5'b0_1100;
            end

            //and
            else if(InstrD[5:0]==6'h24)
            begin
                ALUControlD = 5'b0_0010;
            end

            //or
            else if(InstrD[5:0]==6'h25)
            begin
                ALUControlD = 5'b0_0011;
            end

            //nor
            else if(InstrD[5:0]==6'h27)
            begin
                ALUControlD = 5'b0_0100;
            end
            
            //xor
            else if(InstrD[5:0]==6'h27)
            begin
                ALUControlD = 5'b0_0101;
            end

            //sll
            else if(InstrD[5:0]==6'h0)
            begin
                ALUControlD = 5'b0_0110;
                ShamtD = 1'b1;    
            end

            //srl
            else if(InstrD[5:0]==6'h2)
            begin
                ALUControlD = 5'b0_0111;
                ShamtD = 1'b1;    
            end

            //sra
            else if(InstrD[5:0]==6'h3)
            begin
                ALUControlD = 5'b0_1000;
                ShamtD = 1'b1;
            end

            //sllv
            else if(InstrD[5:0]==6'h4)
            begin
                ALUControlD = 5'b0_0110;
            end

            //srlv
            else if(InstrD[5:0]==6'h6)
            begin
                ALUControlD = 5'b0_0111;
            end

            //srav
            else if(InstrD[5:0]==6'h7)
            begin
                ALUControlD = 5'b0_1000;
            end

            //slt
            else if(InstrD[5:0]==6'h2a)
            begin
                ALUControlD = 5'b0_1010;
            end

            //jr
            else if(InstrD[5:0]==6'h8)
            begin
                JrD = 1'b1;
                RegWriteD = 1'b0;
            end

        end

        //addi
        else if(InstrD[31:26]==6'h8||InstrD[31:26]==6'h9)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            ALUControlD = 5'b0_0000;
        end

        //addiu
        else if(InstrD[31:26]==6'h9)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            ALUControlD = 5'b0_1011;
        end

        //andi
        else if(InstrD[31:26]==6'hc)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            ALUControlD = 5'b1_0010;
        end

        //ori
        else if(InstrD[31:26]==6'hd)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            ALUControlD = 5'b1_0011;
        end

        //beq
        else if(InstrD[31:26]==6'h4)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b0;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b0;
            MemWriteD = 1'b0;
            BranchD = 1'b1;
            ALUControlD = 5'b0_0001;
        end

        //bne
        else if(InstrD[31:26]==6'h5)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b0;
            MemtoRegD = 1'b0;
            RegWriteD = 1'b0;
            MemWriteD = 1'b0;
            BranchD = 1'b1;
            ALUControlD = 5'b0_1001;
        end

        //lw
        else if(InstrD[31:26]==6'h23)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b1;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            ALUControlD = 5'b0_0000;
            MemreadD = 1'b1;
        end

        //sw
        else if(InstrD[31:26]==6'h2b)
        begin
            ShamtD = 1'b0;
            RegDstD = 1'b0;
            ALuSrcD = 1'b1;
            MemtoRegD = 1'b1;
            RegWriteD = 1'b0;
            MemWriteD = 1'b1;
            BranchD = 1'b0;
            ALUControlD = 5'b0_0000;
        end

        //j
        else if(InstrD[31:26]==6'h2)
        begin
            RegWriteD = 1'b0;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            JumpD = 1'b1;
        end

        //jal
        else if(InstrD[31:26]==6'h3)
        begin
            ALUControlD = 5'b0_0000;
            RegWriteD = 1'b1;
            MemWriteD = 1'b0;
            BranchD = 1'b0;
            JalD = 1'b1;
        end


    end

//registers
always @(negedge clock)
    begin
        if(RegWriteW)
        begin
            gr[A3D] <= WD3D;
        end 
    end

always @(posedge clock)
    begin
        if(RsHazardE==1'b1)
        begin
            RD1D <= ALUOutE;
        end

        else if(RsHazardM==1'b1)
        begin
            RD1D <= MemtoRegM ? {d_memory[ALUOutM],d_memory[ALUOutM+1],d_memory[ALUOutM+2],d_memory[ALUOutM+3]} : ALUOutM;
        end

        else
        begin
            RD1D <= gr[A1D];
        end

        if(RtHazardE==1'b1)
        begin
            RD2D <= ALUOutE;
        end

        else if(RtHazardM==1'b1)
        begin
            RD2D <= MemtoRegM ? {d_memory[ALUOutM],d_memory[ALUOutM+1],d_memory[ALUOutM+2],d_memory[ALUOutM+3]} : ALUOutM;
        end

        else
        begin
            RD2D <= gr[A2D];
        end
    end

//decode/execution
always @(posedge clock)
    begin
        if(stall==1'b1)
        begin
            RegWriteE <= 1'b0;
            MemtoRegE <= MemtoRegD;
            MemWriteE <= 1'b0;
            BranchE <= 1'b0;
            ALUControlE <= 5'b0;
            ALuSrcE <= ALuSrcD;
            RegDstE <= RegDstD;
            WriteDataE <= RD2D;
            RtE <= rt;
            RdE <= rd;
            SignImmE <= SignExtend;
            JalE <= 1'b0;
            PCPlus4E <= PCPlus4D;
            ShamtE <= ShamtD;
            ShamtImmE <= ShamtImmD;
            MemreadE <= MemreadD;
        end

        else if(PCSrcM==1'b1)
        begin
            RegWriteE <= 1'b0;
            MemtoRegE <= 1'b0;
            MemWriteE <= 1'b0;
            BranchE <= 1'b0;
            ALUControlE <= 5'b0;
            ALuSrcE <= 1'b0;
            RegDstE <= 1'b0;
            WriteDataE <= RD2D;
            RtE <= rt;
            RdE <= rd;
            SignImmE <= SignExtend;
            JalE <= 1'b0;
            PCPlus4E <= PCPlus4D;
            ShamtE <= ShamtD;
            ShamtImmE <= ShamtImmD;
            MemreadE <= 1'b0;
        end

        else
        begin     
            RegWriteE <= RegWriteD;
            MemtoRegE <= MemtoRegD;
            MemWriteE <= MemWriteD;
            BranchE <= BranchD;
            ALUControlE <= ALUControlD;
            ALuSrcE <= ALuSrcD;
            RegDstE <= RegDstD;
            WriteDataE <= RD2D;
            RtE <= rt;
            RdE <= rd;
            SignImmE <= SignExtend;
            JalE <= JalD;
            PCPlus4E <= PCPlus4D;
            ShamtE <= ShamtD;
            ShamtImmE <= ShamtImmD;
            MemreadE <= MemreadD;
        end
    end

//circuits of the execution
always @(MemWriteE, ALuSrcE, RegDstE, WriteDataE, RtE, RdE, SignImmE, PCPlus4E,
        ShamtE, ShamtImmE, RD1D, RD2D, JalE)
    begin

        //SrcAE
        if(JalE==1'b1)
        begin
            SrcAE = PCPlus4E;
        end

        else if(ShamtE==1'b1)
        begin
            SrcAE = ShamtImmE;    
        end

        else
        begin
            SrcAE = RD1D;
        end


        //SrcBE
        if(JalE==1'b1)
        begin
            SrcBE = 32'b0;
        end

        else if(ALuSrcE==1'b1)
        begin
            SrcBE = SignImmE;
        end

        else
        begin
            SrcBE = RD2D;
        end

        //WriteRegE
        if(JalE==1'b1)
        begin
            WriteRegE = 32'd31;
        end

        else if(RegDstE==1'b1)
        begin
            WriteRegE = RdE;
        end

        else
        begin
            WriteRegE = RtE;
        end

        PCBranchE = (SignImmE<<2) + PCPlus4E;

    end

//Alu circuits
//used to dectect overflowE
reg extra;

parameter gr0 = 32'h0000_0000;
parameter Width = 32;
parameter MSB = Width - 1;
reg signed [31:0] reg_A;
reg signed [31:0] reg_B;
reg [31:0] unsigned_reg_A;
reg [31:0] unsigned_reg_B;


always @(ALUControlE,SrcAE,SrcBE)
    begin
        reg_A = SrcAE;
        unsigned_reg_A = SrcAE;

        reg_B = SrcBE;
        unsigned_reg_B = SrcBE;


        //add/addi
        if(ALUControlE==5'b0_0000)
        begin
            {extra, ALUOutE} = {reg_A[MSB], reg_A} + {reg_B[MSB], reg_B};
            ZeroE = ALUOutE ? 0 : 1;
            negativeE = ALUOutE[MSB];
            overflowE = extra ^ ALUOutE[MSB];
        end

        //addu/addiu
        else if(ALUControlE==5'b0_1011)
        begin
            ALUOutE = unsigned_reg_A + unsigned_reg_B;
            ZeroE = ALUOutE ? 0 :1;
            negativeE = 1'b0;
            overflowE = 1'b0;
        end

        //sub
        else if(ALUControlE==5'b0_0001)
        begin
            {extra, ALUOutE} = {reg_A[MSB], reg_A} - {reg_B[MSB], reg_B};
            ZeroE = ALUOutE ? 0 : 1;
            negativeE = ALUOutE[MSB];
            overflowE = extra ^ ALUOutE[MSB];
        end

        //sub
        else if(ALUControlE==5'b0_1001)
        begin
            {extra, ALUOutE} = {reg_A[MSB], reg_A} - {reg_B[MSB], reg_B};
            ZeroE = ALUOutE ? 1 : 0;
            negativeE = ALUOutE[MSB];
            overflowE = extra ^ ALUOutE[MSB];
        end

        //subu
        else if(ALUControlE==5'b0_1100)
        begin
            ALUOutE =  unsigned_reg_A - unsigned_reg_B;
            ZeroE = ALUOutE ? 0 :1;
            negativeE = 1'b0;
            overflowE = 1'b0;
        end

        //and
        else if(ALUControlE==5'b0_0010)
        begin
            ALUOutE = reg_A & reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end
        
        //andi
        else if(ALUControlE==5'b1_0010)
        begin
            reg_A = {{16{1'b0}}, reg_A[15:0]};
            ALUOutE = reg_A & reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //nor
        else if(ALUControlE==5'b0_0100)
        begin
            ALUOutE = ~(reg_A | reg_B);
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //or
        else if(ALUControlE==5'b0_0011)
        begin
            ALUOutE = reg_A | reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //ori
        else if(ALUControlE==5'b1_0011)
        begin
            reg_A = {{16{1'b0}}, reg_A[15:0]};
            ALUOutE = reg_A | reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //xor
        else if(ALUControlE==5'b0_0101)
        begin
            ALUOutE = reg_A ^ reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //slt
        else if(ALUControlE==5'b0_1010)
        begin
            ALUOutE = reg_A < reg_B;
            overflowE = 1'b0;
            negativeE = ALUOutE ? 1 : 0;
            ZeroE = ALUOutE ? 0 : 1;
        end
        
        //sll/sllv
        else if(ALUControlE==5'b0_0110)
        begin
            ALUOutE = reg_B << reg_A;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //srl/srlv
        else if(ALUControlE==5'b0_0111)
        begin
            ALUOutE = reg_B >> reg_A;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end

        //sra/srav
        else if(ALUControlE==5'b0_1000)
        begin
            ALUOutE = reg_B >>> reg_A;
            overflowE = 1'b0;
            negativeE = ALUOutE[MSB];
            ZeroE = ALUOutE ? 0 : 1;
        end
        
    end

//hazrad for instructions closed together
always @(ALUControlE,SrcAE,SrcBE,MemreadE, PCSrcM, JrD)
    begin
        if(PCSrcM==1'b1)
        begin
            RsHazardE = 1'b0;
            RtHazardE = 1'b0;
            RsHazardM = 1'b0;
            RtHazardM = 1'b0;
            stall = 1'b0;
        end
            
        else
        begin
            //hazard for Rs
            if((RegWriteE==1'b1)&&(WriteRegE!=5'b0)&&(WriteRegE==InstrD[25:21]))
            begin
                RsHazardE = 1'b1;
            end
            else
            begin
                RsHazardE = 1'b0;
            end

            //hazard for Rt
            if((RegWriteE==1'b1)&&(WriteRegE!=5'b0)&&(WriteRegE==InstrD[20:16]))
            begin
                RtHazardE = 1'b1;
            end
            else
            begin
                RtHazardE = 1'b0;
            end

            if(MemreadE==1'b1&&((RtE==InstrD[25:21])||(RtE==InstrD[20:16])))
            begin
                stall = 1'b1;
            end

            else
            begin
                stall = 1'b0;
            end
        end
        
    end

//execution/memory
always @(posedge clock)
    begin
        if(PCSrcM==1'b1)
        begin
            RegWriteM <= 1'b0;
            MemtoRegM <= 1'b0;
            MemWriteM <= 1'b0;
            BranchM <= 1'b0;
            ZeroM <= ZeroE;
            ALUOutM <= ALUOutE;
            WriteRegM <= 5'b0;
            PCBranchM <= 1'b0;
        end

        else
        begin
            RegWriteM <= RegWriteE;
            MemtoRegM <= MemtoRegE;
            MemWriteM <= MemWriteE;
            BranchM <= BranchE;
            ZeroM <= ZeroE;
            ALUOutM <= ALUOutE;
            WriteRegM <= WriteRegE;
            PCBranchM <= PCBranchE;
        end
    end

//circuits of the memory
always @(MemWriteM, BranchM, ZeroM, ALUOutM,
        WriteDataE)
    begin
        PCSrcM = BranchM & ZeroM;
        AM = ALUOutM;
        WDM = WriteDataE;

        //hazard for Rs
        if((RegWriteM==1'b1)&&(WriteRegM!=5'b0)&&(WriteRegM==InstrD[25:21]))
        begin
            RsHazardM = 1'b1;
        end
        else
        begin
            RsHazardM = 1'b0;
        end

        //hazard for Rt
        if((RegWriteM==1'b1)&&(WriteRegM!=5'b0)&&(WriteRegM==InstrD[20:16]))
        begin
            RtHazardM = 1'b1;
        end
        else
        begin
            RtHazardM = 1'b0;
        end

    end

//data memory
always @(posedge clock)
    begin
        if(MemWriteM)
        begin
            {d_memory[ALUOutM],d_memory[ALUOutM+1],d_memory[ALUOutM+2],d_memory[ALUOutM+3]} <= WDM;
            RDM <= WDM;
        end

        else
        begin
            RDM <= {d_memory[ALUOutM],d_memory[ALUOutM+1],d_memory[ALUOutM+2],d_memory[ALUOutM+3]};
        end
    end

//memory/write
always @(posedge clock)
    begin
        RegWriteW <= RegWriteM;
        MemtoRegW <= MemtoRegM;
        ALUOutW <= ALUOutM;
        WriteRegW <= WriteRegM;
    end

//circuits of the write
always @(ALUOutW, RDM, MemtoRegW)
    begin
        ResultW = MemtoRegW ? RDM : ALUOutW;
    end

endmodule