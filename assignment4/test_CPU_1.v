`timescale 1ns/1ps

// general register
`define gr0  	5'b00000
`define gr1  	5'b00001
`define gr2  	5'b00010
`define gr3 	5'b00011
`define gr4  	5'b00100
`define gr5  	5'b00101
`define gr6  	5'b00110
`define gr7  	5'b00111

module CPU_test;

    //Inputs
    reg clock;
    reg start;
    reg [319:0] in_instruction;

    CPU a(.clock(clock),.start(start),.i_datain(in_instruction));

    initial begin
    clock=0;
    start=0;

in_instruction[319:288]={6'b001000, `gr0, `gr1, 16'd1};//addi
in_instruction[287:256]={6'b101011, `gr0, `gr0, 16'b0}; //sw
in_instruction[255:224]={6'b001000, `gr0, `gr3, 16'b10};//addi
in_instruction[223:192]={6'b100011, `gr0, `gr2, 16'b0}; //lw
in_instruction[191:160]={6'b0, `gr1, `gr0, `gr4, 5'b0, 6'b100010};//sub
in_instruction[159:128]={6'b100, `gr0, `gr0, 16'b1};//branch
in_instruction[127:96]={6'b0, `gr1, `gr1, `gr1, 5'b0, 6'b100000};//add
in_instruction[95:64]={6'b0, `gr1, `gr1, `gr1, 5'b100, 6'b000000};//sll

$display("   pc   :   gr1  :   gr2  :   gr3  :   gr4  :   $ra  : data memory[0:31] :instruction");
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $display("%h:%h:%h:%h:%h:%h:      %h     :%h",a.PC,a.gr[1],a.gr[2],a.gr[3],a.gr[4],a.gr[31],{a.d_memory[0],a.d_memory[1],a.d_memory[2],a.d_memory[3]},a.InstrD);
#period $finish;
end

parameter period=10;
always #5clock=~clock;

endmodule