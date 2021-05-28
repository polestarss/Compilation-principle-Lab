// #include "helpLex.cpp"
#include "helpPaser.cpp"
#include "LexAnalyze.cpp"
void grammerAnalyze()
{
    LexAnalyze();
    getProduction();
    getAllFirst();
    getAllFollow();
    getAnalyzeTable();
    vector<string> remainS;
    parser();
    printTree(AT);

}