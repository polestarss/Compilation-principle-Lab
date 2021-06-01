#include "helpIR.cpp"

void generateIR()
{
    semainticAnalyze();
    addIR(AST.root->child);
    cout<<"生成的三地址表示的四元式如下"<<endl;
    printIR();
}