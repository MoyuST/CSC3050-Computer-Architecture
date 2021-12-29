#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include "dataHandler.h"

using namespace std;

int * asciiType(string data, int* pdata, bool ztype);
int * wordType(string data, int * pdata);
int * halfwordType(string data, int * pdata);
int * floatType(string data, int * pdata);
int * byteType(string data, int * pdata);
int * doubleType(string data, int * pdata);

int* dataHandle(char* filename, int * pdata){
    ifstream infile(filename);

    // try to open the file
    if(infile.fail()){
        cout << "failed to open the file" << endl;
        infile.close();
        return pdata;
    }

    string data;
    string data2;
    string cache;

    //jump to the .data
    while(getline(infile, cache)){
        stringstream cac;
        string marker;
        cac << cache;
        cac >> marker;

        if(marker[0]=='.'){
            break;
        }
    }

    while(infile>>data){
        if(data[0]=='.'){
            break;
        }

        if(data[0]=='#'){
            getline(infile, cache);
            continue;
        }

        infile >> data;
        getline(infile, cache);
        stringstream dataflow;
        dataflow << cache;
        dataflow >> data2;

        if(data==".asciiz"||data==".ascii"){
            int start = 0;
            int end = 0;
            while(cache[start]!='\0'&&(cache[start]==' '||cache[start]=='\t')) start++;

            // invalid
            if(cache[start]!='"'||cache[start]=='\0'){
                cout << "error in .data" << endl;
                continue;
            }
            end = start+1;

            while(cache[end]!='\0'&&cache[end]!='"') end++;
            cache = cache.substr(start, end-start+1);

            if(data==".asciiz") pdata = asciiType(cache, pdata, true);
            else pdata = asciiType(cache, pdata, false);

        }
        else if(data==".word"){
            pdata = wordType(data2, pdata);
        }
        else if(data==".byte"){
            pdata = byteType(data2, pdata);
        }
        else if(data==".double"){
            pdata = doubleType(data2,pdata);
        }
        else if(data==".float"){
            pdata = floatType(data2, pdata);
        }
        else if(data==".half"){
            pdata = halfwordType(data2, pdata);
        }
        else continue;
    }
    infile.close();
    return pdata;
}

int * asciiType(string data, int* pdata, bool ztype){ // ztype for Asciiz
    string holder;
    int lenOfWord = 0;
    int leng = data.length();

    if(data[0]!='"'){
        cout << "error1 in the .data" << endl;
        return pdata;
    }

    for(int i=1; i < leng; i++){
        if(data[i]=='"') break;
        lenOfWord++;
    }

    holder = data.substr(1, lenOfWord);

    char * pchar = (char *) pdata;
    for(int i =0; i<lenOfWord; i++){
        *pchar = holder[i];
        pchar++;
    }

    if(ztype){
        *pchar = '\0';
        lenOfWord++;
    }

    int quotient = (lenOfWord)/4;
    int reminder = (lenOfWord)%4;
    if(reminder) quotient++;

    // data = ""
    if(quotient==0) quotient=1;
    return pdata+quotient;
}


int * wordType(string data, int * pdata){
    int holder = 0;
    int sign = 1;
    for(int i = 0; data[i]!='\0';i++){
        if(data[i]==','){
            holder *= sign;
            *pdata =holder;
            pdata++;
            holder = 0;
            sign = 1;
        }
        else{
            if(data[i]=='-') sign = -1;
            else holder = holder*10 + (data[i]-'0');
        }
    }

    //store the last number
    holder *= sign;
    *pdata = holder;
    pdata++;
    return pdata;
}

int * doubleType(string data, int * pdata){
    stringstream dataflow;
    double num;
    dataflow << data;
    dataflow >> num;
    double * pdouble = (double *)  pdata;
    *pdouble = num;
    return (pdata+2);
}

int * floatType(string data, int * pdata){
    stringstream dataflow;
    float num;
    dataflow << data;
    dataflow >> num;
    float * pdouble = (float *) pdata;
    *pdouble = num;
    return ++pdata;
}

int * byteType(string data, int * pdata){
    char charholder = ' ';
    int intholder = 0;
    int count = 0;
    char * pchar = (char*) (pdata);
    int flag = 0;

    for(int i = 0; data[i]!='\0';i++){
        if(data[i]==','){
            if(!flag) charholder=intholder;
            *pchar = charholder;
            pchar++;
            count++;
            intholder = 0;
            flag = 0;
        }
        else{
            if(data[i]=='\'') flag=1;
            else{
                if(flag) charholder=data[i];
                else intholder = intholder*10 + (data[i]-'0');
            }
        }
    }

    //store the last char
    if(!flag) charholder=intholder;
    *pchar = charholder;
    count++;

    int quotient = count/4;
    int reminder = count%4;
    if(reminder) quotient++;
    return pdata+quotient;
}

int * halfwordType(string data, int * pdata){
    char charholder = ' ';
    int intholder = 0;
    int count = 0;
    uint16_t * pchar = (uint16_t *) (pdata);
    int flag = 0;

    for(int i = 0; data[i]!='\0';i++){
        if(data[i]==','){
            if(!flag) charholder=intholder;
            *pchar = charholder;
            pchar++;
            count++;
            intholder = 0;
            flag = 0;
        }
        else{
            if(data[i]=='\'') flag=1;
            else{
                if(flag) charholder=data[i];
                else intholder = intholder*10 + (data[i]-'0');
            }
        }
    }

    //store the last char
    if(!flag) charholder=intholder;
    *pchar = charholder;
    count++;

    int quotient = count/2;
    int reminder = count%2;
    if(reminder) quotient++;
    return pdata+quotient;

}
