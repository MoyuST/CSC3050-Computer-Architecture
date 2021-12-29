#ifndef HEADER_H
#define HEADER_H

#include <cstring>
#include <map>
#include <iostream>

void SYSCALL();
void ADD(int * rs, int * rt, int* rd, int shamt);
void ADDU(int * rs, int * rt, int* rd, int shamt);
void ADDI(int * rs, int * rt, int imm);
void ADDIU(int * rs, int * rt, int imm);
void AND(int * rs, int * rt, int* rd, int shamt);
void ANDI(int * rs, int * rt, int imm);
void DIV(int * rs, int * rt, int* rd, int shamt);
void DIVU(int * rs, int * rt, int* rd, int shamt);
void MULT(int * rs, int * rt, int* rd, int shamt);
void MULTU(int * rs, int * rt, int* rd, int shamt);
void NOR(int * rs, int * rt, int* rd, int shamt);
void OR(int * rs, int * rt, int* rd, int shamt);
void ORI(int * rs, int * rt, int imm);
void SLL(int * rs, int * rt, int* rd, int shamt);
void SLLV(int * rs, int * rt, int* rd, int shamt);
void SRA(int * rs, int * rt, int* rd, int shamt);
void SRAV(int * rs, int * rt, int* rd, int shamt);
void SRL(int * rs, int * rt, int* rd, int shamt);
void SRLV(int * rs, int * rt, int* rd, int shamt);
void SUB(int * rs, int * rt, int* rd, int shamt);
void SUBU(int * rs, int * rt, int* rd, int shamt);
void XOR(int * rs, int * rt, int* rd, int shamt);
void XORI(int * rs, int * rt, int imm);
void LUI(int * rs, int * rt, int imm);
void SLT(int * rs, int * rt, int* rd, int shamt);
void SLTU(int * rs, int * rt, int* rd, int shamt);
void SLTI(int * rs, int * rt, int imm);
void SLTIU(int * rs, int * rt, int imm);
void BEQ(int * rs, int * rt, int imm);
void BGEZ(int * rs, int * rt, int imm);
void BGEZAL(int * rs, int * rt, int imm);
void BGTZ(int * rs, int * rt, int imm);
void BLEZ(int * rs, int * rt, int imm);
void BLTZAL(int * rs, int * rt, int imm);
void BLTZ(int * rs, int * rt, int imm);
void BNE(int * rs, int * rt, int imm);
void J(int target);
void JAL(int target);
void JALR(int * rs, int * rt, int* rd, int shamt);
void JR(int * rs, int * rt, int* rd, int shamt);
void TEQ(int * rs, int * rt, int* rd, int shamt);
void TEQI(int * rs, int * rt, int imm);
void TNE(int * rs, int * rt, int* rd, int shamt);
void TNEI(int * rs, int * rt, int imm);
void TGE(int * rs, int * rt, int* rd, int shamt);
void TGEU(int * rs, int * rt, int* rd, int shamt);
void TGEI(int * rs, int * rt, int imm);
void TGEIU(int * rs, int * rt, int imm);
void TLT(int * rs, int * rt, int* rd, int shamt);
void TLTU(int * rs, int * rt, int* rd, int shamt);
void TLTI(int * rs, int * rt, int imm);
void TLTIU(int * rs, int * rt, int imm);
void LB(int * rs, int * rt, int imm);
void LBU(int * rs, int * rt, int imm);
void LH(int * rs, int * rt, int imm);
void LHU(int * rs, int * rt, int imm);
void LW(int * rs, int * rt, int imm);
void LWL(int * rs, int * rt, int imm);
void LWR(int * rs, int * rt, int imm);
void LL(int * rs, int * rt, int imm);
void SB(int * rs, int * rt, int imm);
void SH(int * rs, int * rt, int imm);
void SW(int * rs, int * rt, int imm);
void SWL(int * rs, int * rt, int imm);
void SWR(int * rs, int * rt, int imm);
void SC(int * rs, int * rt, int imm);
void MFHI(int * rs, int * rt, int* rd, int shamt);
void MFLO(int * rs, int * rt, int* rd, int shamt);
void MTHI(int * rs, int * rt, int* rd, int shamt);
void MTLO(int * rs, int * rt, int* rd, int shamt);
void SYSCALL();
bool overflow(int x, int y);

#endif // HEADER_H
