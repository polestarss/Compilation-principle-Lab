#ifndef SEMANTIC
#define SEMANTIC
#include "helpSemantic.cpp"
// #include "helpLex.cpp"
// #include "helpPaser.cpp"
// #include "grammerAnalyze.cpp"
int main()
{
    grammerAnalyze();
    init();
    FxS(AT);
    cout<<"===========AST如下：================="<<endl;
    printAST(AST.root,0);
}






#endif 