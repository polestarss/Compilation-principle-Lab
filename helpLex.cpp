#ifndef HLEPLEX_CPP
#define HLEPLEX_CPP

#include<bits/stdc++.h>
using namespace std;
#define buffer_size 1024
#define symbol_size 1024
#define terminal_size 1024
#define noTerminal_size 2048
const int production_size  = 1024;
// #define
string symbolTable[symbol_size];
map<string,int> wordNum;        //C语言一共有六类单词
map<string,int> :: iterator iter_si;  //生成一个map迭代器

//---------------------------------------词法分析变量声明
int symbolNum = 0;
int state = 0;                 //自动机状态
int row = 0;                   //行数
int charNum = 0;               //文件字符总数
char Buffer[buffer_size];     //缓冲区
char C;                       //每一步读入的字符
string token;                 //由一组字符生成的记号
string attribute;              //记号的属性
FILE *fpInput;                //读入待测文件
ofstream fPrint("lexResult.txt", ios::out);              //将结果输出至文件



string reserve[36] = {                                //保留字表
    "auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long","bool",
	"register", "return", "short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "unsigned", "void",
	  "while", "get", "put","false","true"         //补充get和put
};



// 词法分析部分

bool isDigit(char c)
{
    return (c >= '0' && c <= '9') ? 1 : 0;
}

bool isOct(char c)
{
    return (c >= '0' && c <= '7') ? 1 : 0;
}

bool isHex(char c)
{
    return (c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') ? 1 : 0;
}

bool isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0; 
}

bool isReserve(string str)
{
    for(int i=0;i<34;i++)
    {
        if(str == reserve[i])
            return 1;
    }
    return 0;
}

string cat(string token,char ch)
{
    string str = token + ch;
    return str;
}

char getChar(int& lexemebegin,int& forward,int len)    //获取当前字符
{
    char ch = Buffer[forward];
    forward++;
    return ch;
}

void outResultToFile(string token1,string attribute1)   //将结果输出到文件中
{
    fPrint<<"<"<< token1 <<"  ,  "<< attribute1 <<" > " << row<<endl;
}

void addressError(int& lexemebegin,int& forward,int len)  //处理词法分析中的错误
{
    do
    {
        C = getChar(lexemebegin,forward,len);
    }while(isLetter(C) || isDigit(C) || C == '.');
}


// 语法分析部分

//判断是否是非终结符
bool isNotTerminal(string s)
{
    if(s.length() != 1)
        return 0;
    else
    {
        char c = s[0];
        return c >= 'A' && c <= 'Z';

    }
}



#endif // !HLEPLEX_CPP