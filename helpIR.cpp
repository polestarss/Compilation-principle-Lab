#ifndef HELPIR
#define HELPIR
#include "semanticAnalyze.cpp"

const int maxQuaternion = 1024;
int IRnum = 0;         //四元式的数量

struct Quaternion
{
    int id;        //语句序号
    string op;    //操作
    int arg1Index;   //参数1 指向符号表的位置
    int arg2Index;   //参数2指向符号表的位置
    int resultIndex;   //结果 指向符号表的位置
}IRtable[maxQuaternion];

int findArgIndex(int arg)
{
    
}














#endif // 
