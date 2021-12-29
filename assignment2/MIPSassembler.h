#ifndef MIPSASSEMBLE_H
#define MIPSASSEMBLE_H

#include <cstring>
#include <map>
#include <iostream>

//assemble the instruction to machine code
int mispAssemble(int argc, char argv1[], char argv2[]);

//create the address for the outputfile
std::string newaddress(std::string input, std::string output);

//input the number to the os stream in binary form
void convernum(int num, int size, std::ostream & os);

//transfer the register to number
int converReg(char* code, bool end = false);

// onvert the "imm(register)" to binary form
int converArray(char* code);

//transfer the char[] to num
bool charToNum(char* string, int & num, bool end = false);

//extrace the label
bool labelextrace(char* label, char* & labelholder);

// scan first to find the label
void firstscan(std::string filename, std::map<std::string, int> & labels, std::map<int, int> & numOfEmpty);

// transfer the mips code to machine code
void mipsToMachinecode(std::istream & is, int * pint, std::map<std::string, int> labels, std::map<int, int> numOfEmpty);

// convert a mips file to machine code and store it in the memory
void mipsToMachinecode(std::istream & is, std::ostream & os, std::map<std::string, int> labels, std::map<int, int> numOfEmpty);

extern std::map<std::string, int>
type1, type2, type3, type3, type5, type6, type7, type8,
type9, type9_2, type10, type11, type12, type13, type14;


#endif // MIPSASSEMBLE_H
