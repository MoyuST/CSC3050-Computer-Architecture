#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include "MIPSassembler.h"

using namespace std;

//name of the register
const char* Reg[32] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0",   "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0",   "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8",   "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

// different types of mips code
// xxx rd, rs, rt (notice: the opcode for mul is 0x1c)
const map<string, int> type1 ={ //(name, funct)
    make_pair("add", 0x20), make_pair("addu", 0x21), make_pair("and", 0x24),
    make_pair("mul", 2),    make_pair("nor", 0x27),  make_pair("or", 0x25),
    make_pair("sub", 0x22), make_pair("subu", 0x23), make_pair("xor", 0x26),
    make_pair("slt", 0x2a), make_pair("sltu", 0x2b)
};

//xxx rt, rs, imm
const map<string, int> type2 ={ //(name, opcode)
    make_pair("addi", 8),   make_pair("addiu", 9),   make_pair("andi", 0xc),
    make_pair("ori", 0xd),  make_pair("xori", 0xe),  make_pair("slti", 0xa),
    make_pair("sltiu", 0xb)
};

//xxx rd, rs
const map<string, int> type3 ={ //(name, funct)
    make_pair("clo", 0x21),   make_pair("clz", 0x20)
};

//xxx rs, rt and opcode == 0
const map<string, int> type4 ={ //(name, funct)
    make_pair("div", 0x1a),  make_pair("divu", 0x1b), make_pair("mult", 0x18),
    make_pair("multu", 0x19), make_pair("teq", 0x34),  make_pair("tne", 0x36),
    make_pair("tge", 0x30),  make_pair("tgeu", 0x31), make_pair("tlt", 0x32),
    make_pair("tltu", 0x33)
};

//xxx rs, rt and opcode == 0x1c
const map<string, int> type5 ={ //(name, funct)
    make_pair("madd", 0),  make_pair("maddu", 1), make_pair("msub", 4),
    make_pair("msubu", 5)
};

//xxx rd, rt shamt
const map<string, int> type6 ={ //(name, funct)
    make_pair("sll", 0),  make_pair("sra", 3), make_pair("srl", 2)
};

//xxx rd, rt, rs
const map<string, int> type7 ={ //(name, funct)
    make_pair("sllv", 4),  make_pair("srav", 7), make_pair("srlv", 6)
};

//xxx rs, rt, label
const map<string, int> type8 ={ //(name, op)
    make_pair("beq", 4),  make_pair("bne", 5)
};

//xxx rs, label
//since this type of operations have different opcode and rt code
//so use two maps to store them
const map<string, int> type9 ={ //(name, op)
    make_pair("bgez", 1),  make_pair("bgezal", 1), make_pair("bgtz", 7),
    make_pair("blez", 6),  make_pair("bltzal", 1), make_pair("bltz", 1)
};

const map<string, int> type9_2 ={ //(name, rt)
    make_pair("bgez", 1),  make_pair("bgezal", 0x11), make_pair("bgtz", 0),
    make_pair("blez",0),  make_pair("bltzal", 0x10), make_pair("bltz", 0)
};

//xxx target
const map<string, int> type10 ={ //(name, op)
    make_pair("j", 2),  make_pair("jal", 3)
};

//xxx rs
const map<string, int> type11 ={ //(name, funct)
    make_pair("jr", 8),  make_pair("mthi", 0x11), make_pair("mtlo", 0x13)
};

//xxx rd
const map<string, int> type12 ={ //(name, funct)
    make_pair("mfhi", 0x10),  make_pair("mflo", 0x12)
};

//xxx rs, imm
const map<string, int> type13 ={ //(name, rt)
    make_pair("teqi", 0xc), make_pair("tnei", 0xe), make_pair("tgei", 8),
    make_pair("tgeiu", 9),  make_pair("tlti", 0xa),   make_pair("tltiu", 0xb)
};

//xxx rt, address
const map<string, int> type14 ={ //(name, op)
    make_pair("lb", 0x20), make_pair("lbu", 0x24), make_pair("lh", 0x21),
    make_pair("lhu", 0x25),  make_pair("lw", 0x23),   make_pair("lwcl", 0x31),
    make_pair("lwl", 0x22),  make_pair("lwr", 0x26), make_pair("ll", 0x30),
    make_pair("sb", 0x28),  make_pair("sh", 0x29), make_pair("sw", 0x2b),
    make_pair("swcl", 0x31),  make_pair("swl", 0x2a), make_pair("swr", 0x2e),
    make_pair("sc", 0x38)
};

//xxx rt, imm
//there is only one of this type, only need to store the opcode
const int type15 = 0xf;

//xxx rs, rd
//there is only one of this type, only need to store the funct code
const int type16 = 9;



int main(int argc, char *argv[]){
    // check the format of input

    if (argc != 3){
        cout << "please enter the names of the input and output files" << endl;
        return 0;
    }

    //create a map to store the addresses for the labels
    map<string, int> labels;

    //create a map to store the line number of the counter
    map<int, int> numOfCounter;

    string out_path; //the address for the output file
    out_path = newaddress(argv[1], argv[2]); //create the address for outfile

    ifstream infile(argv[1]);

    // try to open the file
    if(infile.fail()){
        cout << "failed to open the file" << endl;
        infile.close();
        return 0;
    }

    ofstream outfile(out_path);

    //store the labels and lines of the instruction
    firstscan(argv[1], labels, numOfCounter);

    //transfer the MIPS code
    mipsToMachinecode(infile, outfile, labels, numOfCounter);

    cout << "Output in " << out_path << endl;
    infile.close();
    outfile.close();
    return 0;
}

//create the address for the outputfile
string newaddress(string input, string output){
    int endposi = -1;
    for(int i = input.length()-1; i>=0; i--){
        if(input[i]=='/'){
            endposi=i;
            break;
        }
    }
    //if there is no path in the input
    if(endposi==-1) return output;
    //if there is path in the output
    return input.substr(0,endposi+1)+output;
}

//input the number to the os stream in binary form
void convernum(int num, int size, ostream & os){
    int flag=0;
    if(num<0){
        flag=1;
        num *= -1; //change num to the absolute value
    }

    if(flag){ // conver the negative num to 2's complete form
        int mask=1;
        mask <<= (size);
        mask -= 1;
        num = num^mask;
        num += 1;
    }

    int a = 1;
    a = (1<<(size-1));

    for(int i = 0; i < size ; i++){
        if (num&a) os << 1;
        else os << 0;

        num <<= 1;
    }
}

//transfer the register to number
int converReg(char* code, bool end){
    char codeholder[20] = "";
    char *p = code;

    while(*p==' '||*p=='\t') p++; //skip all the space
    if(*p == '$') p++;
    else return -1; //no register found

    //extract the register out
    int i=0;
    while(*p!='\0' && (*p !=' '&& *p!='\t')){
        i++;
        p++;
    }
    strncpy(codeholder, p-i, i);

    int num = -1;
    //convert the name of register to number
    for(int j=0; j<32; j++){
        if(num!=-1) break;
        if(!strcmp(codeholder, Reg[j])) num = j;
    }

    //check whether there are invalid characters after the register
    while (*p == ' '|| *p=='\t') p++; //skip the space after the register

    //check whether there might be comments followed
    if(end){
        if(*p=='#') return num;
    }

    if (*p != '\0') return -1;
    else return num;

}

// convert the "imm(register)"
bool converArray(char* code, int & rs, int & imm){
    char codeholder[20]= "";
    char *p = code;
    imm = 0;
    int sign=1;

    while(*p==' '||*p=='\t') p++; //skip all the space
    if(*p=='-'){
        sign=-1;
        p++;
    }

    // invalid
    if(*p<'0'||*p>'9') return false;

    while(*p>='0' && *p<='9'){
        imm = 10*imm + (*p-'0');
        p++;
    }
    imm *= sign;

    //invalid input
    if(*p!='(') return false;
    p++;

    int i = 0;
    while(*p!='\0' && *p!=')'){
        i++;
        p++;
    }

    //no value in the bracket
    if(*p!=')') return false;

    strncpy(codeholder, p-i, i);
    rs = converReg(codeholder, true);
    if(rs==-1) return false;

    return true;
}

//transfer the char[] to num, if failed return false
bool charToNum(char* string, int & num, bool end){
    int p = 0;
    int sign = 1;
    num = 0;
    int length = int(strlen(string));

    while(p<length && (string[p]==' '||string[p]=='\t')) p++; //skip the space
    if(string[p] == '-'){
        sign = -1;
        p++;
    }

    while(string[p]<='9' && string[p]>='0'){
        num = 10 * num + (string[p] - '0');
        p++;
    }
    num *= sign;

    while(p<length && (string[p]==' '|| string[p]=='\t')) p++; //skip the space

    //check whether there are comment followed
    if(end){
        if(string[p]=='#') return true;
    }
    if(string[p]!='\0') return false;
    return true;
}

//extrace the label
bool labelextrace(char* label, char* & labelholder){
    char *p = label;

    //skip the space
    while(*p!='\0'&&(*p==' '||*p=='\t')) p++;

    //extrace the label
    int i=0;
    while(*p!='\0' && (*p !=' '&& *p!='\t')){
        i++;
        p++;
    }
    strncpy(labelholder, p-i, i);

    //skip the space behind the label
    while(*p!='\0'&&(*p==' '||*p=='\t')) p++;

    // invalid input
    if(*p!='\0'&&*p!='#') return false;

    return true;

}

// scan first to find the label
void firstscan(string filename, map<string, int> & labels, map<int, int> & numOfCounter){
    ifstream infile(filename);
    string line;
    int counter = -1;
    int linenum = 0;
    while(getline(infile, line)){
        counter++;
        linenum++;

        int ps=0; // start of the label
        int pe=0; // end of the label
        int length = line.length();

        // skip of the space
        while( ps<length && (line[ps]==' '||line[ps]=='\t')) ps++;

        //the line is empty
        if(ps==length){
            if(!numOfCounter.count(counter)) numOfCounter.insert(make_pair(counter, linenum+1));
            else numOfCounter.find(counter)->second += 1;
            counter--;
            continue;
        }

        pe = ps;
        while(pe <length && (line[pe]!=' '&& line[pe]!='\t') ) pe++;

        // skip the line of ".data" and ".text" and comments
        if (line[ps]=='.' || line[ps]=='#'){
            counter--;
            continue;
        }

        // skip the line of instruction
        if (line[pe-1]!=':') continue;


        string label = line.substr(ps, pe-ps-1);

        //check whether there are instruction after the label
        while(pe < length && (line[pe]==' '||line[pe]=='\t')) pe++;

        labels.insert(pair<string, int> (label, counter));

        //not followed by instruction
        if(line[pe]=='\0' || line[pe]=='#'){
            if(!numOfCounter.count(counter)) numOfCounter.insert(make_pair(counter, linenum+1));
            else numOfCounter.find(counter)->second += 1;
            counter--;
        }

    }
    infile.close();

}

// transfer the mips code to machine code
void mipsToMachinecode(istream & is, ostream & os, map<string, int> labels, map<int, int> numOfCounter){
    char ins[50], arg1[150], arg2[150], arg3[150]; //hold the instruction and arguements
    int linenum = 0; //count the line number
    int counter = -1;
    string cache;

    while (is >> ins){
        counter++;

        // skip the label if the label is not followed by instruction
        if(ins[strlen(ins)-1]==':'){
           counter--;
           continue;
        }

        linenum++;

        // skip the line of ".data" and ".text" and comments
        if (ins[0]=='.' || ins[0]=='#') {
            getline(is, cache);
            counter--;
            continue;
        }

        // update the line number for counter
        if(numOfCounter.count(counter)){
            linenum = numOfCounter.find(counter)->second;
        }

        //type 1 instruction
        // xxx rd, rs, rt (notice: the opcode for mul is 0x1c)
        if(type1.count(ins)==1){
            int op = 0;
            if(!strcmp(ins, "mul")) op=0x1c;

            // rd
            is.getline(arg1, 150, ',');
            int rd = converReg(arg1);
            if(rd==-1)
                cout << "error in line " << linenum << endl;

            // rs
            is.getline(arg2, 150, ',');
            int rs = converReg(arg2);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg3, 150);
            int rt = converReg(arg3, true);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type1.find(ins)->second;

            convernum(op, 6, os);    //opcode
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(rd, 5, os);    //rd
            convernum(0, 5, os);    //shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 2 instruction
        // xxx rt, rs, imm
        else if(type2.count(ins)==1){

            // rt
            is.getline(arg1, 150, ',');
            int rt = converReg(arg1);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            // rs
            is.getline(arg2, 150, ',');
            int rs = converReg(arg2);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            // imm
            is.getline(arg3, 150);
            int imm = 0;
            if(!charToNum(arg3, imm, true))
                cout << "error in line " << linenum << endl;

            //op
            int op = type2.find(ins)->second;

            convernum(op, 6, os);    //opcode
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(imm, 16, os); //imm

            os << endl;
        }

        //type 3 instruction
        // xxx rd, rs
        else if(type3.count(ins)==1){

            // rd
            is.getline(arg1, 150, ',');
            int rd = converReg(arg1);
            if(rd==-1)
                cout << "error in line " << linenum << endl;
            // rs
            is.getline(arg2, 150);
            int rs = converReg(arg2, true);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type3.find(ins)->second;

            convernum(0x1c, 6, os);  //opcode=0x1c
            convernum(rs, 5, os);     //rs
            convernum(0, 5, os);     //rt
            convernum(rd, 5, os);    //rd
            convernum(0, 5, os);     //shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 4 instruction
        // xxx rt, rs with opcode=0
        else if(type4.count(ins)==1){

            // rs

            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg2, 150);
            int rt = converReg(arg2, true);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type4.find(ins)->second;

            convernum(0, 6, os);     //opcode=0x1c
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(0, 10, os);    //rd+shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 5 instruction
        // xxx rt, rs with opcode=0x1c
        else if(type5.count(ins)==1){

            // rs
            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg2, 150);
            int rt = converReg(arg2, true);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type5.find(ins)->second;

            convernum(0x1c, 6, os);     //opcode=0x1c
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(0, 10, os);    //rd+shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 6 instruction
        // xxx rd, rt, shamt
        else if(type6.count(ins)==1){

            // rd
            is.getline(arg1, 150, ',');
            int rd = converReg(arg1);
            if(rd ==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg2, 150, ',');
            int rt = converReg(arg2);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            // shamt
            is.getline(arg3, 150);
            int shamt = 0;
            if(!charToNum(arg3, shamt, true))
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type6.find(ins)->second;

            convernum(0, 6, os);     //op
            convernum(0, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(rd, 5, os);    //rd
            convernum(shamt, 5, os); //shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 7 instruction
        // xxx rd, rt, rs
        else if(type7.count(ins)==1){

            // rd
            is.getline(arg1, 150, ',');
            int rd = converReg(arg1);
            if(rd ==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg2, 150, ',');
            int rt = converReg(arg2);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            // rs
            is.getline(arg3, 150);
            int rs = converReg(arg3, true);
            if(rs==-1)
                cout << "error in line " << linenum << endl;

            //funct
            int funct = type7.find(ins)->second;

            convernum(0, 6, os);     //op
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(rd, 5, os);    //rd
            convernum(0, 5, os);     //shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        //type 8 instruction
        // xxx rs, rt, label/offset
        else if(type8.count(ins)==1){

            // rs
            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs ==-1)
                cout << "error in line " << linenum << endl;

            // rt
            is.getline(arg2, 150, ',');
            int rt = converReg(arg2);
            if(rt==-1)
                cout << "error in line " << linenum << endl;

            // label/offset
            is.getline(arg3, 150);
            int offset;
            char labelholder[150] = "";
            char *p =labelholder;

            //it is a label
            if(labelextrace(arg3, p)&&labels.count(labelholder)){
                offset = labels.find(labelholder)->second - counter - 1;
            }
            // it is the offset
            else if(charToNum(arg3,offset,true));
            //invalid
            else cout << "error in line " << linenum << endl;

            //op
            int op = type8.find(ins)->second;

            convernum(op, 6, os);     //op
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(offset, 16, os); //offset
            os << endl;
        }

        //type 9 instruction
        // xxx rs, label
        else if(type9.count(ins)==1){

            // rs
            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs ==-1) cout << "error in line " << linenum << endl;

            // label
            is.getline(arg2, 150);
            int offset;
            char labelholder[150] = "";
            char *p =labelholder;

            //it is a label
            if(labelextrace(arg2, p)&&labels.count(labelholder)){
                offset = labels.find(labelholder)->second - counter - 1;
            }
            // it is the offset
            else if(charToNum(arg2,offset,true));
            //invalid
            else cout << "error in line " << linenum << endl;

            //op
            int op = type9.find(ins)->second;

            //rt
            int rt = type9_2.find(ins)->second;

            convernum(op, 6, os);     //opcode=0x1c
            convernum(rs, 5, os);    //rs
            convernum(rt, 5, os);    //rt
            convernum(offset, 16, os); //offset
            os << endl;
        }

        //type 10 instruction
        // xxx target
        else if(type10.count(ins)==1){

            // label
            is.getline(arg1, 150);
            int target;
            char labelholder[150] = "";
            char *p =labelholder;

            //it is a label
            if(labelextrace(arg1, p)&&labels.count(labelholder)){
                target = labels.find(labelholder)->second;
            }
            // it is the address
            else if(charToNum(arg1,target,true));
            //invalid
            else cout << "error in line " << linenum << endl;

            //remove the first 4 digits of the target and shift right 2 bits
            target *= 4;
            target = target & 0x0fff;
            target >>= 2;

            //op
            int op = type10.find(ins)->second;

            convernum(op, 6, os);     //opcode=0x1c
            convernum(target, 26, os); //target
            os << endl;
        }

        //type 11 instruction
        // xxx rs
        else if(type11.count(ins)==1){

            // rs
            is.getline(arg1, 150);
            int rs = converReg(arg1, true);
            if(rs ==-1) cout << "error in line " << linenum << endl;

            //funct
            int funct = type11.find(ins)->second;

            convernum(0, 6, os);  //op
            convernum(rs, 5, os); //rs
            convernum(0, 15, os); //rt+rd+shamt
            convernum(funct, 6, os);//funct
            os << endl;
        }

        //type 12 instruction
        // xxx rd
        else if(type12.count(ins)==1){

            // rd
            is.getline(arg1, 150);
            int rd = converReg(arg1, true);
            if(rd ==-1) cout << "error in line " << linenum << endl;

            //funct
            int funct = type12.find(ins)->second;

            convernum(0, 6, os); //op
            convernum(0, 10, os); //rs+rt
            convernum(rd, 5, os); //rd
            convernum(0, 5, os); //shamt
            convernum(funct, 6, os);//funct
            os << endl;
        }

        //type 13 instruction
        // xxx rs, imm
        else if(type13.count(ins)==1){

            // rs
            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs ==-1) cout << "error in line " << linenum << endl;
            // imm
            is.getline(arg2, 150);
            int imm = 0;
            if(!charToNum(arg2, imm, true))
                cout << "error in line " << linenum << endl;

            //rt
            int rt = type13.find(ins)->second;

            convernum(1, 6, os); //opcode = 1
            convernum(rs, 5, os); //rs
            convernum(rt, 5, os); //rt
            convernum(imm, 16, os); //imm
            os << endl;
        }

        //type 14 instruction
        // xxx rt, address
        else if(type14.count(ins)==1){

            // rt
            is.getline(arg1, 150, ',');
            int rt = converReg(arg1);
            if(rt ==-1)
                cout << "error in line " << linenum << endl;

            // rs, imm
            is.getline(arg2, 150);
            int rs, imm;
            if(!converArray(arg2,rs,imm))
                cout << "error in line " << linenum << endl;

            //op
            int op = type14.find(ins)->second;

            convernum(op, 6, os); //op
            convernum(rs, 5, os); //rs
            convernum(rt, 5, os); //rt
            convernum(imm, 16, os); //imm
            os << endl;
        }

        //type 15 instruction
        // lui rt, imm
        else if(!strcmp(ins, "lui")){

            // rt
            is.getline(arg1, 150, ',');
            int rt = converReg(arg1);
            if(rt ==-1)
                cout << "error in line " << linenum << endl;

            // imm
            is.getline(arg2, 150);
            int imm = 0;
            if(!charToNum(arg2, imm, true))
                cout << "error in line " << linenum << endl;

            //op
            int op = type15;

            convernum(op, 6, os); //op
            convernum(0, 5, os); //rs
            convernum(rt, 5, os); //rt
            convernum(imm, 16, os); //imm
            os << endl;
        }

        //type 16 instruction
        // jalr rs, rd
        else if(!strcmp(ins, "jalr")){

            // rs
            is.getline(arg1, 150, ',');
            int rs = converReg(arg1);
            if(rs ==-1)
                cout << "error in line " << linenum << endl;

            // rd
            is.getline(arg2, 150);
            int rd = converReg(arg2, true);
            if(rd==-1)
                cout << "error in line " << linenum << endl;


            //funct
            int funct = type16;

            convernum(0, 6, os); //op
            convernum(rs, 5, os); //rs
            convernum(0, 5, os); //rt
            convernum(rd, 5, os); //rd
            convernum(0, 5, os); //shamt
            convernum(funct, 6, os); //funct
            os << endl;
        }

        else{
            cout << "error in line " << linenum << endl;
            os << endl;
            getline(is, cache);
        }
    }
}
