#ifndef HELPIR
#define HELPIR
#include "semanticAnalyze.cpp"

//函数声明
void addAssign(Gnode AssignTree);  //赋值语句
void addState(Gnode StateTree);    //变量声明
int addArithmetic(Gnode AssignTree);  //算术运算语句



const int maxQuaternion = 1024;
int IRnum = 0;         //四元式的数量
int Ti = 0;
struct Quaternion
{
    int id;        //语句序号
    string op;    //操作
    int arg1Index;   //参数1 指向符号表的位置
    int arg2Index;   //参数2指向符号表的位置
    int resultIndex;   //结果 指向符号表的位置
}IRtable[maxQuaternion];

int findArgIndex(string arg)     //寻找参数在符号表中位置
{
    for(int i=0;i<notationNum;i++)
        if(notationTable[i].name == arg)
        {
        
            return i;
        }
    cout<<"参数不在符号表中！"<<endl;
    exit(0);
    
}

void addState(Gnode StateTree)    //变量声明
{
    string type = StateTree->type;
    for(int i=0;i<StateTree->child.size();i++)
    {
        int index = findArgIndex(StateTree->child[i]->name);
        IRtable[IRnum].id = IRnum;
        IRtable[IRnum].op = type;
        IRtable[IRnum].arg1Index = index;
    }
}

void addAssign(Gnode AssignTree)  //赋值语句
{

}
int addArithmetic(Gnode ArithmeticTree)  //算术运算语句 返回临时变量的地址
{
    //算数运算子树一定是二叉树
    Gnode left = ArithmeticTree->child[0];
    Gnode right = ArithmeticTree->child[1];
    int leftIndex,rightIndex;
    bool leftFuct = false,rightFuct = false;
    if(left->name == "+" || left->name == "-")
    {
        leftFuct = true;
        leftIndex = addArithmetic(left);
    }
    if(right->name == "+" || right->name == "-")
    {
        rightFuct = true;
        rightIndex = addArithmetic(right);
    }
    if(!leftFuct)
    {
        //to do
    }
        
    
}













#endif // 
