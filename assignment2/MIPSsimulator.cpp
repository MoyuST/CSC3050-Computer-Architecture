#include <cstdio>
//#include <malloc.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include "MIPSsimulator.h"
#include "MIPSassembler.h"
#include "dataHandler.h"

using namespace std;

unsigned int stringToCodes(string line);
void storeMachindeCode(string filename, int * pdata);
void excute(int & pc, unsigned codes);
void Rins(unsigned int codes);
void Iins(int mark, unsigned int codes, int type);
void Jins(int, unsigned int);
int * excute(int * pc);
bool overflow(int x, int y);
typedef void (*RIns) (int *, int *, int *, int);
typedef void (*IIns) (int *, int *, int);
typedef void (*JIns) (int);

static map<int, RIns> RFunctions={
    {0x20, ADD}, {0x21, ADDU}, {0x24, AND},   {0x1a, DIV},
    {0x1b, DIVU},{0x18, MULT}, {0x19, MULTU}, {0x27, NOR},
    {0x25, OR},  {0, SLL},     {4, SLLV},     {3, SRA},
    {7, SRAV},   {2, SRL},     {6, SRLV},     {0x22, SUB},
    {0x23, SUBU},{0x26, XOR},  {0x2a, SLT},   {0x2b, SLTU},
    {9, JALR},   {8, JR},      {0x34, TEQ},   {0x36, TNE},
    {0x30, TGE}, {0x31, TGEU}, {0x32, TLT},   {0x33, TLTU},
    {0x10, MFHI},{0x12, MFLO}, {0x11, MTHI},  {0x13, MTLO}
};

static map<int, IIns> IFunctions={
    {0xf, LUI}, {8, ADDI},  {9, ADDIU},{0xc, ANDI}, {0xd, ORI},
    {0x23, LW}, {0xe, XORI},{0xf, LUI},{0xa, SLTI},
    {0xb, SLTIU},{4, BEQ}, {7, BGTZ},  {6, BLEZ}, {5, BNE},
    {0x20, LB}, {0x24, LBU},{0x21, LH},{0x25, LHU},
    {0x23, LW}, {0x22, LWL},{0x26, LWR},{0x30, LL},
    {0x28, SB}, {0x29, SH}, {0x2b, SW}, {0x2a, SWL},
    {0x2e, SWR},{0x38, SC}
};

static map<int, IIns> IFUNCTIONS_2 = {
    {1, BGEZ}, {0x11, BGEZAL}, {0x10, BLTZAL}, {0, BLTZ},
    {0xc, TEQI},{0xe, TNEI}, {8, TGEI}, {9, TGEIU},
    {0xa, TLTI}, {0xb, TLTIU}
};

static map<int, JIns> JFunctions={
    {3, JAL}, {2, J}
};
static int * pc;
static int * textStart;
static int flag = 0;
static int * pReg;
static int * hi;
static int * lo;
static int llbit=0;
static int * dynamicdata;


int main(int argc, char *argv[]){

    unsigned int memorysize = 6 * 1024 * 1024;
    unsigned int registersize = 32 * 4;

    if (argc != 3){
        cout << "please enter the names of the input and output files" << endl;
        return -1;
    }

    if(mispAssemble(3, argv[1], argv[2])==-1){
        cout << "error in the instruction" << endl;
        return 0;
    }

    //allocate the memory
    char * p;
    p = (char*) malloc(memorysize);
    memset(p, 0, memorysize);

    //allocate the register
    pReg = (int *) malloc(registersize);
    memset(pReg, 0, registersize);
    //$sp
    pReg[29] = memorysize + 0x400000 - 1;
    //$fp
    pReg[30] = memorysize + 0x400000 - 1;
    //$gp
    pReg[28] = 1024*1024 + 0x400000;

    //allocate the hi and lo
    int * holder = (int *) malloc(4*2);
    hi = holder;
    lo = holder+1;

    // pointer for data segment
    int * pdataSeg = (int *) (p+1024*1024);

    //store the .data
    dynamicdata = dataHandle(argv[1], pdataSeg);

    string newaddr = newaddress(argv[1], argv[2]);
    storeMachindeCode(newaddr, (int *) p);

    pc = (int*) p;
    textStart = (int*) p;
    while((*pc)!=0){
        if(flag) break;
        pc = excute(pc);
    }
    cout << "end address: "<< hex << (pc-textStart)*4+0x400000-4 << endl;
}

unsigned int stringToCodes(string line){
    unsigned int holder = 0;
    unsigned int exp = 0;
    int leng = line.length();

    for(int i = leng-1; i>-1; i--){
        if(exp==0) exp=1;
        else exp *= 2;

        holder += (line[i]-'0')*exp;

    }
    return holder;
}

void storeMachindeCode(string filename, int * pdata){
    ifstream infile(filename.c_str());
    unsigned int * codes = (unsigned int *) pdata;
    string buffer;
    while(getline(infile, buffer)){
        *codes = stringToCodes(buffer);
        codes++;
    }
}

int * excute(int * PC){
    unsigned int codes = *PC;
    int  opcode;
    opcode = (0xfc000000)&codes;
    opcode >>= 26;
    opcode &= 0x0000003f;

    if(codes==0xc){
        SYSCALL();
        return ++pc;
    }

    if(opcode == 0){
        Rins(codes);
        return ++pc;
    }
    else if(opcode==2||opcode==3){
        Jins(opcode, codes);
        return ++pc;
    }
    else{
        if(opcode==1){
            int rt;
            rt = 0x001f0000&codes;
            rt >>= 16;
            Iins(rt, codes, 1);
        }
        else Iins(opcode, codes, 0);
        return ++pc;
    }
}

bool overflow(int x, int y){
    long long X = (long long) x;
    long long Y = (long long) y;
    long long sum = X + Y;
    if(sum<INT_MIN||sum>INT_MAX){
        return true;
    }
    return false;
}

void Rins(unsigned int codes){
    int Rs = (codes&0x3e00000);
    Rs >>= 21;
    int * rs = (pReg+Rs);

    int Rt = (codes&0x1f0000);
    Rt >>= 16;
    int * rt = (pReg+Rt);

    int Rd = (codes&0xf800);
    Rd >>= 11;
    int * rd = (pReg+Rd);

    unsigned int shamt = (codes&0x7c0);
    shamt >>= 6;
    int funct = (codes&0x3f);
    RFunctions[funct](rs, rt, rd, shamt);
}

void Iins(int mark, unsigned int codes, int type){
    int Rs = (codes&0x3e00000);
    Rs >>= 21;
    int * rs = (pReg+Rs);

    int Rt = (codes&0x1f0000);
    Rt >>= 16;
    int * rt = (pReg+Rt);

    int imm = (codes&0xffff);
    //transfer 2's complement
    if(imm&0x8000){
        imm ^= 0xffff;
        imm +=1;
        imm *= -1;
    }
    if(type==0) IFunctions[mark](rs, rt, imm);
    else IFUNCTIONS_2[mark](rs, rt, imm);
}

void Jins(int opcode, unsigned int codes){
    int target = (codes&0x3ffffff);
    target <<= 2;
    int curAddr = (pc-textStart) + 0x400000;
    curAddr &= 0xfc000000;
    target = curAddr|target;
    JFunctions[opcode](target);
}

//R funct = 0x20
void ADD(int * rs, int * rt, int* rd, int shamt){
    if(overflow(*rs, *rt)){
        cout << "overflow" << endl;
        flag = 1;
    }
    else{
        *rd = *rs + *rt;
    }
}
//R 0x21
void ADDU(int * rs, int * rt, int* rd, int shamt){
    *rd = *rs + *rt;
}

//I op=8
void ADDI(int * rs, int * rt, int imm){
    if(overflow(*rs, imm)){
        cout << "overflow" << endl;
        flag = 1;
    }
    else{
//        cout << "addi " << *rt << " " << *rs << " " << imm <<endl;
        *rt = *rs + imm;
    }
}

//I op=9
void ADDIU(int * rs, int * rt, int imm){
    *rt = *rs + imm;
}

//R 0x24
void AND(int * rs, int * rt, int* rd, int shamt){
    *rd = *rs & *rt;
}

//I 0xc
void ANDI(int * rs, int * rt, int imm){
    *rt = *rs & imm;
}

//R 0x1a
void DIV(int * rs, int * rt, int* rd, int shamt){
    if(*rs==INT_MIN && *rt==-1){
        cout << "overflow" << endl;
        flag = 1;
    }
    else{
        *lo = *rs / *rt;
        *hi = *rs % *rt;
    }
}

//R 0x1b
void DIVU(int * rs, int * rt, int* rd, int shamt){
    *lo = *rs / *rt;
    *hi = *rs % *rt;
}

// R 0x18
void MULT(int * rs, int * rt, int* rd, int shamt){
    long long RS = (long long ) *rs;
    long long RT = (long long ) *rt;
    long long result = RS * RT;
    *lo = (int) result;
    result >>= 32;
    *hi = (int) result;
}

//R 0x19
void MULTU(int * rs, int * rt, int* rd, int shamt){
    unsigned long long RS = (long long) *rs;
    unsigned long long RT = (long long) *rt;
    unsigned long long result = RS * RT;
    *lo = (int) result;
    result >>= 32;
    *hi = (int) result;
}

//R 0x27
void NOR(int * rs, int * rt, int* rd, int shamt){
    *rd = ~(*rs | *rt);
}

// R 0x25
void OR(int * rs, int * rt, int* rd, int shamt){
    *rd = *rs | *rt;
}

// I 0xd
void ORI(int * rs, int * rt, int imm){
//    cout << "ORI " << endl;
    *rt = *rs | imm;
}

// R 0
void SLL(int * rs, int * rt, int* rd, int shamt){
//    cout << "rt: " << hex << *rt << " rd: " << *rd << " shamt " << shamt << dec<< endl;
    *rd = (*rt)<<shamt;
//    cout << "before: " << hex << *rt << " after: " << *rd << " shamt " << shamt << dec<< endl;
}

//R 4
void SLLV(int * rs, int * rt, int* rd, int shamt){
    *rd = (*rt)<<(*rs);
}

//R 3
void SRA(int * rs, int * rt, int* rd, int shamt){
    *rd = (*rt) >> shamt;
}

//R 7
void SRAV(int * rs, int * rt, int* rd, int shamt){
    *rd = (*rt) >> *rs;
}

// R 2
void SRL(int * rs, int * rt, int* rd, int shamt){
    unsigned RT = (unsigned) *rt;
    *rd = (RT >> shamt);
}

//R 6
void SRLV(int * rs, int * rt, int* rd, int shamt){
    unsigned RT = (unsigned) *rt;
    *rd = (RT >> (*rs));
}

//R 0x22
void SUB(int * rs, int * rt, int* rd, int shamt){
    if(overflow(*rs, -(*rd))) flag = 1;
    else *rd = *rs - *rt;
}

//R 0x23
void SUBU(int * rs, int * rt, int* rd, int shamt){
    *rd = *rs - *rt;
}

//R 0x26
void XOR(int * rs, int * rt, int* rd, int shamt){
    *rd = *rs ^ *rt;
}

//I 0xe
void XORI(int * rs, int * rt, int imm){
    *rt = *rs ^ imm;
}

//I 0xf
void LUI(int * rs, int * rt, int imm){
    int lower = imm & 0xffff;
    lower <<= 16;
    *rt = lower;
}

//R 0x2a
void SLT(int * rs, int * rt, int* rd, int shamt){
    *rd = (*rs < *rt);
}

//R 0x2b
void SLTU(int * rs, int * rt, int* rd, int shamt){
    unsigned int RS = (unsigned int ) *rs;
    unsigned int RT = (unsigned int ) *rt;
    *rd = (RS < RT);
}

//I 0xa
void SLTI(int * rs, int * rt, int imm){
    *rt = (*rs < imm);
}

//I 0xb
void SLTIU(int * rs, int * rt, int imm){
    unsigned int RS = (unsigned int) *rs;
    unsigned int IMM = (unsigned int) imm;
    *rt = (RS < IMM);
}

//I 4
void BEQ(int * rs, int * rt, int imm){
    if(*rs==*rt) pc+= imm;
}

//?
//B 1 1
void BGEZ(int * rs, int * rt, int imm){
    if(*rs>=0) pc+= imm;
}

//? ?register
//B 1 0x11
void BGEZAL(int * rs, int * rt, int imm){
    if(*rs>=0){
        pc += imm;
        pReg[31] = (pc-textStart)*4+(0x00400000);
    }
}

//?
//B 7 0
void BGTZ(int * rs, int * rt, int imm){
    if(*rs>0) pc+= imm;
}

//?
//B 6
void BLEZ(int * rs, int * rt, int imm){
    if(*rs<=0) pc+= imm;
}

//? ?register
//B 1 0x10
void BLTZAL(int * rs, int * rt, int imm){
    if(*rs<0){
        pc += imm;
        pReg[31] = (pc-textStart)*4+(0x00400000);
    }
}

//? ?
//B 1 0
void BLTZ(int * rs, int * rt, int imm){
    if(*rs<0) pc += imm;
}

//I 5
void BNE(int * rs, int * rt, int imm){
    if(*rs != *rt) pc += imm;
}

//J 2
void J(int target){
//    cout << "J" << endl;
    target -= 0x400000;
    pc = textStart+(target/4)-1;
}

//J 3
void JAL(int target){
//    cout << "JAL" << endl;
    target -= 0x400000;
    pReg[31] = (pc-textStart)*4+0x400000;
    pc =  textStart+(target/4)-1;
}

//????default
//R 9
void JALR(int * rs, int * rt, int* rd, int shamt){
    *rd = (pc-textStart)*4+0x400000;
    pc =  textStart+((*rs)-0x400000)/4-1;
}

//R 8
void JR(int * rs, int * rt, int* rd, int shamt){
    pc = textStart+((*rs)-(0x400000))/4;
}

//R 0x34
void TEQ(int * rs, int * rt, int* rd, int shamt){
    if(*rs == *rt) flag = 1;
}

//I 1
//T 0xc
void TEQI(int * rs, int * rt, int imm){
    if(*rs == imm) flag=1;
}

//R 0x36
void TNE(int * rs, int * rt, int* rd, int shamt){
    if(*rs != *rt) flag = 1;
}

//I 1
//T 0xe
void TNEI(int * rs, int * rt, int imm){
    if(*rs != imm) flag = 1;
}

//R 0x30
void TGE(int * rs, int * rt, int* rd, int shamt){
    if(*rs >= *rt) flag = 1;
}

//R 0x31
void TGEU(int * rs, int * rt, int* rd, int shamt){
    unsigned int RS = (unsigned int) (*rs);
    unsigned int RT = (unsigned int) (*rt);
    if(RS >= RT) flag = 1;
}

//I 1
//T 8
void TGEI(int * rs, int * rt, int imm){
    if(*rs >= imm) flag = 1;
}

//I 1
//T 9
void TGEIU(int * rs, int * rt, int imm){
    unsigned int RS = (unsigned int) (*rs);
    unsigned int IMM = (unsigned int) imm;
    if(RS >= IMM) flag = 1;
}

//R 0x32
void TLT(int * rs, int * rt, int* rd, int shamt){
    if(*rs < *rt) flag = 1;
}

//R 0x33
void TLTU(int * rs, int * rt, int* rd, int shamt){
    unsigned int RS = (unsigned int) (*rs);
    unsigned int RT = (unsigned int) (*rt);
    if(RS < RT) flag = 1;
}

//I 1
//T a
void TLTI(int * rs, int * rt, int imm){
    if(*rs < imm) flag = 1;
}

//I 1
//T b
void TLTIU(int * rs, int * rt, int imm){
    unsigned int RS = (unsigned int) (*rs);
    unsigned int IMM = (unsigned int) (imm);
    if(RS < IMM) flag = 1;
}

//I 0x20
void LB(int * rs, int * rt, int imm){
    int8_t * BYTE;
    int8_t * addr = (int8_t *) ((*rs-0x400000)/4+textStart);
    BYTE = (int8_t *)(addr+imm);
    *rt = *BYTE;
}

//I 0x24
void LBU(int * rs, int * rt, int imm){
    uint8_t * BYTE;
    uint8_t * addr = (uint8_t *) ((*rs-0x400000)/4+textStart);
    BYTE = (uint8_t *) (addr+imm);
    *rt = *BYTE;
}

//I 0x21
void LH(int * rs, int * rt, int imm){
    int16_t * halfword;
    int8_t * addr = (int8_t *) ((*rs-0x400000)/4+textStart);
    halfword = (int16_t *) (addr+imm);
    *rt = *halfword;
}

//I 0x25
void LHU(int * rs, int * rt, int imm){
    uint16_t * halhword;
    uint8_t * addr = (uint8_t *) ((*rs-0x400000)/4+textStart);
    halhword = (uint16_t *) (addr+imm);
}

//I 0x23
void LW(int * rs, int * rt, int imm){
    int * NUM;
    NUM = ((*rs-0x400000)/4+textStart+imm);
    *rt = *NUM;
}

//I 0x22
void LWL(int * rs, int * rt, int imm){
    int addr = *rs + imm*4;
    int remian = addr % 4;
    int num = 4 - remian;
    uint8_t *textST = (uint8_t *) textStart;
    uint8_t * newaddr = (uint8_t *) (addr-0x400000+textST);
    uint8_t * RT = (uint8_t *) rt;
    for(int i =0; i < num; i++){
        *(RT+i) = * (newaddr+i);
    }
}

//I 0x26
void LWR(int * rs, int * rt, int imm){
    int addr = *rs + imm*4;
    int remain = (addr % 4) + 1;
    uint8_t * textST = (uint8_t *) textStart;
    uint8_t * newaddr = (uint8_t *) (addr-0x400000+textST);
    uint8_t * RT = (uint8_t *) rt + 3;
    for(int i =0; i < remain; i++){
        *(RT-i) = * (newaddr-i);
    }
}

//I 0x30
void LL(int * rs, int * rt, int imm){
    int * NUM;
    NUM = ((*rs-0x400000)/4+textStart+imm);
    *rt = *NUM;
    llbit = 1;
}

//I 0x28
//lower byte
void SB(int * rs, int * rt, int imm){
   uint8_t lowerbyte = uint8_t (*rt);
   int * addr;
   addr = (*rs-0x400000)/4+textStart+imm;
   *addr = lowerbyte;
}

//I 0x29
void SH(int * rs, int * rt, int imm){
    uint16_t lowerbyte = uint16_t (*rt);
    int * addr;
    addr = (*rs-0x400000)/4+textStart+imm;
    *addr = lowerbyte;
}

//I 0x2b
void SW(int * rs, int * rt, int imm){
    int * NUM;
    int off = (*rs-0x400000)/4+imm;
    NUM = (textStart+off);
    *NUM = *rt;
}

//I 0x2a
void SWL(int * rs, int * rt, int imm){
    int addr = *rs + imm*4;
    int remian = addr % 4;
    int num = 4 - remian;
    uint8_t *textST = (uint8_t *) textStart;
    uint8_t * newaddr = (uint8_t *) (addr-0x400000+textST);
    uint8_t * RT = (uint8_t *) rt;
    for(int i =0; i < num; i++){
        * (newaddr+i) = *(RT+i) ;
    }
}

//I 0x2e
void SWR(int * rs, int * rt, int imm){
    int addr = *rs + imm*4;
    int remain = (addr % 4) + 1;
    uint8_t * textST = (uint8_t *) textStart;
    uint8_t * newaddr = (uint8_t *) (addr-0x400000+textST);
    uint8_t * RT = (uint8_t *) rt + 3;
    for(int i =0; i < remain; i++){
        * (newaddr-i) = *(RT-i);
    }
}

//I 0x38
void SC(int * rs, int * rt, int imm){
    if(llbit==1){
        int * NUM;
        NUM = ((*rs-0x400000)/4+textStart+imm);
        *NUM = *rt;
        *rt = 1;
        llbit = 0;
    }
    else{
        *rt = 0;
        llbit = 1;
    }
}

//R 0x10
void MFHI(int * rs, int * rt, int* rd, int shamt){
    *rd = *hi;
}

//R 0x12
void MFLO(int * rs, int * rt, int* rd, int shamt){
    *rd = *lo;
}

//R 0x11
void MTHI(int * rs, int * rt, int* rd, int shamt){
    *hi = *rs;
}

//R 0x13
void MTLO(int * rs, int * rt, int* rd, int shamt){
    *lo = *rs;
}

void SYSCALL(){
    //create a map for file operation
    static map<int, fstream*> filemap;
    static map<string, int> filenamemap;
    static int filecount = 0;

    if(pReg[2]==1){
        int * NUM = pReg+4;
        cout << "syscall out: "<< *NUM <<endl;
    }
    else if(pReg[2]==4){
        char * textST = (char *) textStart;
        char * STR = (char * ) ((pReg[4]-0x400000)+textST);
        cout << "syscall out: "<<(STR) << endl;
    }
    else if(pReg[2]==5){
        int num;
        cout << "syscall: enter a num:";
        cin >> num;
        pReg[2] = num;
    }
    else if(pReg[2]==8){
        string holder;
        string STR;
        char * textST = (char *) textStart;
        cout << "syscall: enter a string: ";
        cin >> holder;
        STR = holder.substr(0, pReg[5]);
        char * pchar = (char *) ((pReg[4]-0x400000)+textST);
        for(int i=0; i<pReg[5]; i++){
            *pchar = STR[i];
            pchar++;
        }
        *pchar = '\0';
    }
    else if(pReg[2]==9){
        int addr = (dynamicdata-textStart)*4+0x400000;
        pReg[2] = addr;
        dynamicdata += pReg[4];
    }
    else if(pReg[2]==10){
        flag = 1;
    }
    else if(pReg[2]==11){
        char CHAR = (char) (pReg[4]);
        cout << "syscall out: " <<  CHAR << endl;
    }
    else if(pReg[2]==12){
        char holder;
        cout << "syscall: enter a char: ";
        cin >> holder;
        pReg[2] = holder;
    }
    else if(pReg[2]==13){
        string filename;
        fstream testfile;
        char * textST = (char *) textStart;
        char * addr = (char *) ((pReg[4]-0x400000)+textST);
        int filedes;
        filename = addr;

        if(filenamemap.count(filename)){
            filedes = filenamemap[filename];
        }
        else{
            filedes = filecount;
            filenamemap[filename] = filecount;
        }

        //read only
        if(pReg[5]==0) {
            filemap[filedes] = new fstream(filename.c_str(), ios::in);
        }
        //write only
        else if(pReg[5]==1){
            filemap[filedes] = new fstream(filename.c_str(), ios::out);
        }
        //write and read
        else {
            filemap[filedes] = new fstream(filename.c_str(), ios::out|ios::in);;
        }

        if((*filemap[filedes]).fail()){
            cout << "fail to open the file" << endl;
            pReg[4] = -1;
            return;
        }

        pReg[4] = filedes;
        filecount++;
    }
    else if(pReg[2]==14){
        string cache="";
        char holder;
        int count = 0;
        char * textST = (char *) textStart;
        char * pchar = (char *) ((pReg[5]-0x400000)+textST);

        if((*filemap[pReg[4]]).fail()){
            pReg[4] = 0;
            cout << "the file is failed to open, unable to read" << endl;
            return;
        }

        while(((*filemap[pReg[4]]).get(holder))&&(count<pReg[6])){
            *pchar = holder;
            pchar++;
            count++;
        }
        *pchar = '\0';

        pReg[4] = count;
    }
    else if(pReg[2]==15){
        char * textST = (char *) textStart;
        string cache;
        char * addr = (char *) ((pReg[5]-0x400000)+textST);
        cache = addr;
        cache = cache.substr(0, pReg[6]);

        if((*filemap[pReg[4]]).fail()){
            pReg[4] = 0;
            cout << "the file is failed to open, unable to write" << endl;
            return;
        }

        (*filemap[pReg[4]]) << cache;
        pReg[4] = pReg[6];
    }
    else if(pReg[2]==16){
        (*filemap[pReg[4]]).close();
        filemap.erase(pReg[4]);
    }
    else if(pReg[2]==17){
        cout << "syscall out: " << pReg[4] << endl;
        flag = 1;
    }
}
