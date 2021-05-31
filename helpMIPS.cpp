#ifndef HELPMIPS
#define HELPMIPS

#include "helpIR.cpp"
const int maxValue = 1024;
const int maxReg = 18;   //mipsCPU支持t0-t10 和S0-s10共18个寄存器
int VariableNum = 0;
int regUseNum = 0;
struct Variable
{
    int index;   //变量在符号表中的位置
    string regName; //变量存放在哪个寄存器
    bool isValid;   //是否有效
}VariableToReg[maxValue];

struct reg
{
    string name;   //寄存器名字
    int count;     //寄存器计数
}regTable[maxReg];

void VariableInit(int index)  //声明变量
{
    VariableToReg[VariableNum].index = index;
    int temp;
    string regName = "";
    if(regUseNum<10)
    {
        temp = regUseNum;
        char id = temp + '0';
        regName = "$t" + id;
    }   
    else if(regUseNum >=10 && regUseNum <18)
    {
        temp = regUseNum - 10;
        char id = temp + '0';
        regName = "$s" + id;
    }
    else
    {
        cout<<"寄存器数量不足"<<endl;
        exit(0);
    }
    regUseNum++;
    VariableToReg[VariableNum].regName = regName;
    VariableToReg[VariableNum].isValid = true;
}

string getReg(int index)  //获取变量存在哪个寄存器中
{
    for(int i=0;i<VariableNum;i++)
        if(VariableToReg[i].isValid && VariableToReg[i].index == index)
            return VariableToReg[i].regName;
    cout<<"未找到寄存器！错误"<<endl;
    exit(0);
}

void generateR_Type(string op,string rs,string rt,string rd)  //生成R型指令
{
    if(op=="+")
        cout<<"add "<<rd<<","<<rs<<","<<rt<<endl;
    else if(op=="-")
        cout<<"sub "<<rd<<","<<rs<<","<<rt<<endl;
    else if(op=="||" || op=="|")
        cout<<"or "<<rd<<","<<rs<<","<<rt<<endl;
    else if(op=="&&" || op=="&")
        cout<<"and "<<rd<<","<<rs<<","<<rt<<endl;
    else if(op=="<")
        cout<<"slt "<<rd<<","<<rs<<","<<rt<<endl;    
    else if(op==">")
        cout<<"slt "<<rd<<","<<rt<<","<<rs<<endl;  //将小于的操作数交换位置
    else if(op=="<=")
    {
        cout<<"slt "<<"$v0"<<","<<rt<<","<<rs<<endl;   //大于取反就是小于等于
        cout<<"xori "<<"$v0"<<","<<"$v0"<<","<<"1"<<endl;
    }
    else if(op==">=")
    {
        cout<<"slt "<<"$v0"<<","<<rs<<","<<rt<<endl;   //小于取反就是大于等于
        cout<<"xori "<<"$v0"<<","<<"$v0"<<","<<"1"<<endl;
    }
    else if(op=="==" || op=="!=")
    {
        cout<<"slt "<<"$v0"<<","<<rs<<","<<rt<<endl;   
        cout<<"slt "<<"$v1"<<","<<rt<<","<<rs<<endl;   
        cout<<"nor "<<rd<<","<<"$v0"<<","<<"$v1"<<endl;   
    }
    else if(op=="*")
        cout<<"mul "<<rs<<","<<rt<<endl;
    else if(op=="/")  //特殊情况
    {
        cout<<"div "<<rs<<","<<rt<<endl;
        cout<<"mtlo "<<rd<<endl;
    }
}

void generateI_Type(string op,string rs,string rt,string immediate)   //生成I型指令
{
    if(op=="+")
        cout<<"addi "<<rt<<","<<rs<<","<<immediate<<endl;
    else if(op=="-")
        cout<<"subi "<<rt<<","<<rs<<","<<immediate<<endl;
    else if(op=="||" || op=="|")
        cout<<"ori "<<rt<<","<<rs<<","<<immediate<<endl;
    else if(op=="&&" || op=="&")
        cout<<"andi "<<rt<<","<<rs<<","<<immediate<<endl;
    else if(op=="<")
        cout<<"slti "<<rt<<","<<rs<<","<<immediate<<endl;
    else if(op==">")
        cout<<"slti "<<rt<<","<<immediate<<","<<rs<<endl;
    else if(op=="==" || op=="!=")
    {
        cout<<"slti "<<"$v0"<<","<<rs<<","<<immediate<<endl;   
        cout<<"slti "<<"$v1"<<","<<immediate<<","<<rs<<endl;   
        cout<<"nor "<<rt<<","<<"$v0"<<","<<"$v1"<<endl;   
    }
    else if(op=="if")
        cout<<"beq "<<rs<<","<<"$zero"<<","<<immediate<<endl;

}

void generateJ_Type(string op,string immediate)
{
    if(op=="goto")
        cout<<"j "<<immediate<<endl;

}

void Compile()    //生成MIPS汇编代码
{
    for(int i=0;i<IRnum;i++)
    {
        string op = IRtable[i].op;
        if(op == "int" || op == "bool")
            VariableInit(IRtable[i].arg1Index);
        else if(op == "+" || op == "-" || op == ">" || op =="<"|| op ==">="|| op =="<="|| op =="==")
        {
            string rd = getReg(IRtable[i].resultIndex);
            stringstream ss1;
            ss1<<IRtable[i].arg1Index;
            string immediate1 = ss1.str();
            stringstream ss2;
            ss2<<IRtable[i].arg2Index;
            string immediate2 = ss2.str();
            if(IRtable[i].isArg1Num && IRtable[i].isArg2Num)
            {
                generateI_Type("+","$zero","$v0",immediate1);
                generateI_Type(op,"$v0",rd,immediate2);
            }
            else if(IRtable[i].isArg1Num && !IRtable[i].isArg2Num)
            {
                string rs = getReg(IRtable[i].arg2Index);
                generateI_Type(op,rs,rd,immediate1);
            }
            else if(!IRtable[i].isArg1Num && IRtable[i].isArg2Num)
            {
                string rs = getReg(IRtable[i].arg1Index);
                generateI_Type(op,rs,rd,immediate2);
            }
            else
            {
                string rs = getReg(IRtable[i].arg1Index);
                string rt = getReg(IRtable[i].arg2Index);
                generateR_Type(op,rs,rt,rd);
            }
        }
        else if(op == "=") //赋值使用加法
        {
            string rd = getReg(IRtable[i].resultIndex);
            if(IRtable[i].isArg1Num)
            {
                stringstream ss1;
                ss1<<IRtable[i].arg1Index;
                string immediate1 = ss1.str();
                generateI_Type("+","$zero",rd,immediate1);
            }
            else
            {
                string rs = getReg(IRtable[i].arg1Index);
                generateR_Type(op,rs,"$zero",rd);
            }
        }
        else if(op == "if")
        {
            string position = "";
            stringstream ss3;
            ss3<<IRtable[i].resultIndex;
            position = ss3.str();
            position = "position" + position;
            string rs = getReg(IRtable[i].arg1Index);
            cout<<"beq "<<rs<<","<<"$zero"<<","<<position<<endl;
        }
        else if(op == "goto")
        {
            string position = "";
            stringstream ss3;
            ss3<<IRtable[i].resultIndex;
            position = ss3.str();
            position = "position" + position;
            generateJ_Type("goto", position);
        }

        
    }
}














#endif // !HELPMIPS
