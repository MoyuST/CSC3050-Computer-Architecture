#ifndef DATAHANDLER_H
#define DATAHANDLER_H
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

int * asciiType(std::string data, int* pdata, bool ztype);
int * wordType(std::string data, int * pdata);
int * halfwordType(std::string data, int * pdata);
int * floatType(std::string data, int * pdata);
int * byteType(std::string data, int * pdata);
int * doubleType(std::string data, int * pdata);
int * dataHandle(char* filename, int * pdata);

#endif // DATAHANDLER_H
