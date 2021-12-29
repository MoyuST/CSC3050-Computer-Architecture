#ifndef HEADER_H
#define HEADER_H

#include <cstring>
#include <map>
#include <iostream>

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
void mipsToMachinecode(std::istream & is, std::ostream & os, std::map<std::string, int> labels, std::map<int, int> numOfEmpty);


#endif // HEADER_H
