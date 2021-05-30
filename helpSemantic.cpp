#ifndef HELPSEMANTIC
#define HELPSEMANTIC

#include "helpLex.cpp"
#include "helpPaser.cpp"
#include "grammerAnalyze.cpp"


const int notationTableSize = 1024;
int notationNum = 0;
bool isStruct = false;

struct variable      //符号表
{
    string type;
    string name;
    bool isStructParam;
}notationTable[notationTableSize];


struct grammerNode {
    string name;   //名字
    string type;   //类型
    int row;       //行数
    vector<grammerNode*> child; //子节点
};
typedef grammerNode* Gnode;    //语法树节点
struct grammerTree {
    Gnode root;
}AST;

Gnode nowFather;  //表示现在指向的语法树父节点
Gnode nowTemp;
string nowType;

void FxS(analyzeTree& AT);
vector<Gnode> FxE(treeNode nowNode);
Gnode FxF(treeNode nowNode);
Gnode FxV(treeNode nowNode);
Gnode FxN(treeNode nowNode,string I);
Gnode FxL(treeNode nowNode,string property);
Gnode FxY(treeNode nowNode);
Gnode FxX(treeNode nowNode,Gnode leftNode);
Gnode FxP(treeNode nowNode);
Gnode FxG(treeNode nowNode);
Gnode FxR(treeNode nowNode);
vector<Gnode> FxA(treeNode nowNode);
Gnode FxW(treeNode nowNode);
Gnode FxM(treeNode nowNode);
Gnode FxO(treeNode nowNode);
Gnode FxT(treeNode nowNode);

void init()
{
    AST.root = new grammerNode;
    AST.root->name = "stmts_list";  //语法树根节点
    AST.root->type = "root";
    nowFather = AST.root;
    notationTable[0].type = "bool";
    notationTable[1].type = "bool";

    notationTable[0].name = "false";
    notationTable[1].name = "true";
    notationNum = 2;

}

bool checkVariable(string type,string value)    //查找符号表
{
    int index = atoi(value.c_str());
    string property = symbolTable[index];
    bool flag = false;
    for(int i=0;i<notationNum;i++)
        if(notationTable[i].name == property)
            flag = true;
    if(!flag)    //如果没有直接添加
    {
        if(isStruct)
            notationTable[notationNum].isStructParam = 1;
        else
            notationTable[notationNum].isStructParam = 0;
        notationTable[notationNum].type = type;
        notationTable[notationNum].name = property;
        notationNum++;
    }
    return flag;
}

string findType(string property,int row)
{
    bool flag = false;
    string type = "";
    for(int i=0;i<notationNum;i++)
    {
        if(notationTable[i].name == property)
        {
            type = notationTable[i].type;
            flag = true;
            break;
        }
    }
    if(!flag)
    {
        cout<<"变量未声明就使用！"<<" 行数为："<<row<<endl;
        exit(0);
    }
    return type;
}


//定义通用语法制导翻译的函数
void FxS(analyzeTree& AT)  //翻译S
{
    treeNode nowNode = AT.root;
    vector<Gnode> equalSetence;
    equalSetence = FxE(nowNode->child[0]);
    AST.root->child = equalSetence;
}

vector<Gnode> FxE(treeNode nowNode)  //翻译E
{
    int index = nowNode->productionIndex;
    
    switch (index)
    {
        case 1:       //E->FE  两者同级
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;

            nowRoot = FxF(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 2: //E->PE
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;

            nowRoot = FxP(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 3: //E->GE
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;

            nowRoot = FxG(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 4: // E->R E 
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;
            nowRoot = FxR(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 5: //E-> W E
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;
            nowRoot = FxW(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 6:        //E->VN;E
        {
            int childIndex = nowNode->child[1]->productionIndex;
            if(childIndex == 37)
            {
                vector<Gnode> equalSetence;
                Gnode nowRoot = new grammerNode;
                nowRoot = FxV(nowNode->child[0]);
                if(nowRoot->type != "int" && nowRoot->type !="bool")
                    equalSetence.push_back(nowRoot);
                string I = nowRoot->name;
                nowRoot = FxN(nowNode->child[1],I);
                if(nowRoot->name != "#")
                    equalSetence.push_back(nowRoot);

                vector<Gnode> temp = FxE(nowNode->child[3]);
                for(int i = 0; i < temp.size();i++)
                    equalSetence.push_back(temp[i]);
                return equalSetence;
            }
            else if(childIndex == 38)
            {
                vector<Gnode> equalSetence;
                Gnode nowRoot = new grammerNode;
                nowRoot->name = "functUse";
                nowRoot->type = "keyWords";

                Gnode functName = FxV(nowNode->child[0]);
                nowRoot->child.push_back(functName);

                Gnode functParam = new grammerNode;
                functParam -> name = "functParam";
                nowRoot->type = "keyWords";
                nowTemp = functParam;
                string I = "错误";
                Gnode waste = FxN(nowNode->child[1],I);

                equalSetence.push_back(nowRoot);
                vector<Gnode> temp = FxE(nowNode->child[3]);
                for(int i = 0; i < temp.size();i++)
                    equalSetence.push_back(temp[i]);
                return equalSetence;
            }
            
        }
        case 45: //E->T E
        {
            vector<Gnode> equalSetence;
            Gnode nowRoot = new grammerNode;

            nowRoot = FxT(nowNode->child[0]);
            equalSetence.push_back(nowRoot);
            vector<Gnode> temp;
            temp = FxE(nowNode->child[1]);
            for(int i=0;i<temp.size();i++)
                equalSetence.push_back(temp[i]);
            return equalSetence;
        }
        case 7:   //E->#
        {
            vector<Gnode> equalSetence;
            return equalSetence;
        }
        default: break;
    }


}

Gnode FxF(treeNode nowNode)    //翻译非终结符F
{
    int index = nowNode->productionIndex;
    switch (index)
    {
        case 35:    //F->int id N; 变量申明语句 或函数声明
        {
            //首先判断这是函数声明语句还是变量声明语句
            int childIndex = nowNode->child[2]->productionIndex;
            if(childIndex == 38) //函数声明
            {
                //处理int
                Gnode nowRoot = new grammerNode; 
                nowRoot->name = "functdefine";
                nowRoot->type ="keyWord";
                Gnode nodeFirst = new grammerNode; 
                nodeFirst->name = "int";
                nodeFirst->type = "functFirst";
                nowRoot->child.push_back(nodeFirst);
                //处理id
                string value = nowNode->child[1]->value;//得到id属性
                int row = nowNode->child[1]->row;
                bool exist = checkVariable("functName",value);
                // if(exist && !isStruct)
                if(exist )
                {
                    cout<<"变量或函数重复申明!"<<"  行数为："<<row<<endl;
                    exit(0);
                }
                Gnode nodeSecond = new grammerNode; 
                nodeSecond->type = "functSecond";
                int index1 = atoi(value.c_str());
                string property = symbolTable[index1];
                nodeSecond->name =  property;
                nowRoot->child.push_back(nodeSecond);    //挂载函数名节点
                //处理N
                Gnode nodeThird = new grammerNode;
                nodeThird->name = "functParameter";
                nodeThird->type = "functThird";
                nowTemp = nodeThird;
                nowNode = nowNode->child[2];
                Gnode waste = FxN(nowNode,property);

                nowRoot->child.push_back(nowTemp);
                return nowRoot;
            }
            // if(childIndex == 37)  //变量声明
            else
            {
                //处理int
                Gnode nowRoot = new grammerNode; 
                nowRoot->name = "int";
                nowRoot->type ="keyWord";
                nowTemp = nowRoot;
                //处理id
                string value = nowNode->child[1]->value;//得到id属性
                int row = nowNode->child[1]->row;
                bool exist = checkVariable("int",value);
                if(exist)
                {
                    cout<<"变量或函数重复申明!"<<"  行数为："<<row<<endl;
                    exit(0);
                }
                Gnode nodeTemp1 = new grammerNode; 
                nodeTemp1->type = "int";
                int index1 = atoi(value.c_str());
                string property = symbolTable[index1];
                nodeTemp1->name =  property;
                nowTemp->child.push_back(nodeTemp1);    //挂载节点
                //处理L
                nowNode = nowNode->child[2];
                Gnode waste = FxN(nowNode,property);
                return nowTemp;
            }
            
            

        }
        case 36: //F->bool id L
        {
            //处理bool
            Gnode nowRoot = new grammerNode; 
            nowRoot->name = "bool";
            nowRoot->type ="keyWord";
            nowTemp = nowRoot;
            //处理id
            string value = nowNode->child[1]->value;//得到id属性
            int row = nowNode->child[1]->row;
            bool exist = checkVariable("bool",value);
            if(exist&& !isStruct)
            {
                cout<<"变量重复申明!"<< "行数为："<<row<<endl;
                exit(0);
            }
            Gnode nodeTemp1 = new grammerNode; 
            nodeTemp1->type = "bool";
            int index1 = atoi(value.c_str());
            string property = symbolTable[index1];
            nodeTemp1->name =  property;
            nowTemp->child.push_back(nodeTemp1);    //挂载节点
            //处理L
            nowNode = nowNode->child[2];
            Gnode waste = FxL(nowNode,property);
            return nowTemp;
        }
    default:
        break;
    }
}

Gnode FxL(treeNode nowNode,string I)
{
    int index = nowNode->productionIndex;
    switch(index)
    {
        case 8:  //L->, id L   还是变量声明
        {
            //处理id
            string idType = nowTemp->name;
            if(idType == "functParameter")
                idType = "int";
            string value = nowNode->child[1]->value;
            int row = nowNode->child[1]->row;
            bool exist = checkVariable(idType,value);

            if(exist && nowTemp->name != "get"&& !isStruct)
            {
                cout<<"变量重复定义"<<" 行数为："<<row<<endl;
                exit(0);
            }
            Gnode nodeTemp1 = new grammerNode; 
            nodeTemp1->type = idType;
            int index1 = atoi(value.c_str());
            string property = symbolTable[index1];
            nodeTemp1->name =  property;
            nowTemp->child.push_back(nodeTemp1);    //挂载节点
            nowNode = nowNode->child[2];
            Gnode waste = FxL(nowNode,property);
            return waste;
        }
        case 9:  //L->= Y
        {
            //处理等号
            Gnode nowRoot = new grammerNode;
            nowRoot->name = "=";
            nowRoot->type = "assign";
            int row = nowNode->child[0]->row;
            //将前面传进来的id挂载到左子节点
            string type = findType(I,row);
            Gnode nodeTemp1 = new grammerNode; 
            nodeTemp1->type = type;
            nodeTemp1->name = I;
            nowRoot->child.push_back(nodeTemp1);
            nowType = type;
            //寻找右子树
            nowNode = nowNode->child[1];
            Gnode subTree = FxY(nowNode);
            //做赋值类型检查
            // if(type == "int" && (subTree->type != "arithmetic" && subTree->type != "int"))
            // {
            //     cout<<"整型赋值错误"<<" 行数为："<<row<<endl;
            //     exit(0);
            // }
            // if(type == "bool" && (subTree->type != "logic" && subTree->type != "bool"))
            // {
            //     cout<<"布尔型赋值错误"<<" 行数为："<<row<<endl;
            //     exit(0);
            // }
            nowRoot->child.push_back(subTree);
            return nowRoot;
        }
        case 10: //L->#
        {
            Gnode nowRoot = new grammerNode;
            nowRoot->name = "#";
            return nowRoot;
        }
        default:break;
    }
}

Gnode FxV(treeNode nowNode)
{
    int index = nowNode->productionIndex;
    switch(index)
    {
        case 12:  //V->id
        {
            string value = nowNode->child[0]->value;
            int row = nowNode->child[0]->row;
            int index1 = atoi(value.c_str());
            string property = symbolTable[index1];
            string type = findType(property,row);
            Gnode nodeTemp1 = new grammerNode;
            nodeTemp1->type = type;
            nodeTemp1->name = property;
            return nodeTemp1;

        }
        case 13: //V->num        现在只有int
        {
            string value = nowNode->child[0]->value;
            Gnode nodeTemp1 = new grammerNode;
            nodeTemp1->type = "int";
            nodeTemp1->name = value;
            return nodeTemp1;
        }
        case 14: //V -> (Y)
        {
            return FxY(nowNode->child[1]);
        }
        case 15: //V->!V
        {
            Gnode nowRoot = new grammerNode;
            nowRoot->type = "logic";
            nowRoot->name = "!";
            Gnode nodeTemp1 = FxV(nowNode->child[1]);
            nowRoot->child.push_back(nodeTemp1);
            return nowRoot;
        }
        default:break;
    }
}

Gnode FxN(treeNode nowNode,string I)
{
    int index = nowNode->productionIndex;
    switch (index)
    {
        case 37:  //N->L   变量声明语句
        {
            nowNode = nowNode->child[0];
            Gnode nowRoot = FxL(nowNode,I);
            return nowRoot;
        }
        case 38: //N->(M)  函数声明或调用语句
        {
            Gnode nowRoot = FxM(nowNode->child[1]);
            return nowRoot;
        }
            
    
        default:break;
    }
}

Gnode FxM(treeNode nowNode)
{
    int index = nowNode->productionIndex;
    switch(index)
    {
        case 39:  //M->id L  函数调用
        {
            //处理id
            Gnode nodeTemp = new grammerNode;
            string value = nowNode->child[0]->value;
            int row = nowNode->child[0]->row;
            int index = atoi(value.c_str());
            string property = symbolTable[index];
            bool exist = checkVariable("int",property);
            if(!exist)
            {
                cout<<"变量未声明就使用,行数为："<<row<<endl;
                exit(0);
            }
            nodeTemp->name = property;
            nodeTemp->type = "int";
            nowTemp->child.push_back(nodeTemp);
            Gnode waste = FxL(nowNode->child[1],"错误");
            return nodeTemp;
        }
        case 40: //M->#
        {
            Gnode nodeTemp = new grammerNode;
            nodeTemp->name = "#";
            return nodeTemp;
        }
        case 41: //M->int id O   函数声明语句 
        {
            Gnode nodeTemp = new grammerNode;
            string value = nowNode->child[1]->value;
            int index = atoi(value.c_str());
            string property = symbolTable[index];
            nodeTemp ->name =  property;
            nodeTemp->type = "int";
            nowTemp->child.push_back(nodeTemp);
            Gnode waste = FxO(nowNode->child[2]);
            return waste;
        }
    }
}

Gnode FxO(treeNode nowNode)
{
    int index = nowNode->productionIndex;
    switch(index)
    {
        case 42: //O->, int id O
        {
            Gnode nodeTemp = new grammerNode;
            string value = nowNode->child[2]->value;
            int index = atoi(value.c_str());
            string property = symbolTable[index];
            nodeTemp ->name =  property;
            nodeTemp->type = "int";
            nowTemp->child.push_back(nodeTemp);
            Gnode waste = FxO(nowNode->child[3]);
            return waste;
        }
        case 43: //O->#
        {
            Gnode nodeTemp = new grammerNode;
            return nodeTemp;
        }
    }
}


Gnode FxY(treeNode nowNode)
{
    //Y只能产生VX
    Gnode nowRoot = new grammerNode;
    
    Gnode childV = FxV(nowNode->child[0]);
    
    nowRoot = FxX(nowNode->child[1],childV); //可能返回空串
    if(nowRoot->name == "#")
        return childV;
    else
        return nowRoot;



}

Gnode FxX(treeNode nowNode,Gnode leftNode)
{
    int index = nowNode->productionIndex;
    string type = "";
    string name = "";
    switch(index)
    {
        case 16:  //x->#
        {
            Gnode tempNode = new grammerNode;
            tempNode->name = "#";
            return tempNode;
        }
        case 17:  //X->+VX
        {
            type = "arithmetic";
            name = "+";
            break;
        }
        case 18:  //X->-VX
        {
            type = "arithmetic";
            name = "-";
            break;
        }
        case 19:  //X->&VX
        {
            type = "index";
            name = "&";
            break;
        }
        case 20:  //X->|VX
        {
            type = "index";
            name = "|";
            break;
        }
        case 21:  //X->&&VX
        {
            type = "logic";
            name = "&&";
            break;
        }
        case 22:  //X->||VX
        {
            type = "logic";
            name = "||";
            break;
        }
        case 23:  //X->!=VX
        {
            type = "logic";
            name = "!=";
            break;
        }
        case 24:  //X->==VX
        {
            type = "logic";
            name = "==";
            break;
        }
        case 25:  //X->>VX
        {
            type = "logic";
            name = ">";
            break;
        }
        case 26:  //X->>=VX
        {
            type = "logic";
            name = ">=";
            break;
        }
        case 27:  //X-><VX
        {
            type = "logic";
            name = "<";
            break;
        }
        case 28:  //X->+VX
        {
            type = "logic";
            name = "<=";
            break;
        }
        default:break;
        
    }
    //处理最开始的符号
    Gnode nowRoot = new grammerNode;
    nowRoot->name = name;
    nowRoot->type = type;
    nowRoot->child.push_back(leftNode);

    //处理V
    Gnode childV = FxV(nowNode->child[1]);

    //处理X
    Gnode nowTemp = FxX(nowNode->child[2],childV);

    //挂载节点
    if(nowTemp->name != "#")
        nowRoot->child.push_back(nowTemp);
    else
        nowRoot->child.push_back(childV);
    
    //检查类型  只有int才能参与算术运算
    // if(nowRoot->type == "arithmetic" && (childV->type != "int" || leftNode->type != "int"))
    // {
    //     cout<<"只有整数才能参与算数运算"<<endl;
    //     exit(0);
    // }
    // if(nowRoot->type != "logic" && childV->type == "bool" )
    // {
    //     cout<<"布尔型只能参与逻辑运算"<<endl;
    //     exit(0);
    // }
     // if(nowRoot->type == "logic" && (childV->type != "bool" || leftNode->type != "bool"))
    // {
    //     cout<<"布尔型只能参与逻辑运算"<<endl;
    //     exit(0);
    // }
    
    return nowRoot;

}

Gnode FxP(treeNode nowNode)
{
    //只能是   P->put ( V ) ; 
    Gnode nowRoot = new grammerNode;
    nowRoot->name = "put";
    nowRoot->type = "keyWord";
    Gnode nodeTemp = FxV(nowNode->child[2]);
    nowRoot->child.push_back(nodeTemp);
    return nowRoot;
}

Gnode FxG(treeNode nowNode)
{
    //只能是   G->get ( id L ) ; 
    Gnode nowRoot = new grammerNode;
    nowRoot->name = "get";
    nowRoot->type = "keyWord";
    nowTemp = nowRoot;

    string value = nowNode->child[2]->value;
    int index = atoi(value.c_str());
    int row = nowNode->child[2]->row;
    string property = symbolTable[index];
    string type = findType(property,row);

    Gnode childNode = new grammerNode;
    childNode->name = property;
    childNode->type = type;
    nowTemp->child.push_back(childNode);
    string I = "错误";
    Gnode waste = FxL(nowNode->child[3],I);    //是且只能是L->,id L


    return nowTemp;
}

Gnode FxR(treeNode nowNode)
{
    //只能是 R->if ( Y ) { E } ; A
    Gnode nowRoot = new grammerNode;
    nowRoot->name = "if-then-else";
    nowRoot->type = "keyWord";
    //处理（Y）
    Gnode ifStart = new grammerNode;
    ifStart->name = "ifCondition";
    ifStart->type = "keywords";
    Gnode ifCondition = FxY(nowNode->child[2]);
    if(ifCondition->type != "logic")
    {
        cout<<"if的判断条件必须是逻辑运算!  "<<"出错行数： "<<endl;
        exit(0);
    }
    ifStart->child.push_back(ifCondition);
    nowRoot->child.push_back(ifStart);

    //处理E
    Gnode ifSecond = new grammerNode;
    ifSecond->name = "ifThen";
    ifSecond->type = "keyWords";
    vector<Gnode> ifThen = FxE(nowNode->child[5]);
    ifSecond->child = ifThen;
    nowRoot->child.push_back(ifSecond);

    //处理else
    Gnode ifThird = new grammerNode;
    ifThird->name = "ifelse";
    ifThird->type = "keyWords";
    vector<Gnode> ifElse = FxA(nowNode->child[8]);
    ifThird->child = ifElse;
    if(ifElse.size() != 0)
        nowRoot->child.push_back(ifThird);
        
    return nowRoot;


}

vector<Gnode> FxA(treeNode nowNode)
{
    int index = nowNode->productionIndex;
    vector<Gnode> equalSetence;
    switch(index)
    {
        case 30: //A -> else { E }
        {
            equalSetence = FxE(nowNode->child[2]);
            break;
        }
        case 31: //A->#
        {
            break;
        }
    }
    return equalSetence;
}


Gnode FxW(treeNode nowNode)
{
    //只能是 W->while ( Y ) { E } ;
    Gnode nowRoot = new grammerNode;
    nowRoot->name = "while";
    nowRoot->type = "keyWord";
    //处理（Y）
    Gnode whileStart = new grammerNode;
    whileStart->name = "whileCondition";
    whileStart->type = "keywords";
    Gnode whileCondition = FxY(nowNode->child[2]);
    if(whileCondition->type != "logic")
    {
        cout<<"while的判断条件必须是逻辑运算!  "<<"出错行数： "<<endl;
        exit(0);
    }
    whileStart->child.push_back(whileCondition);
    nowRoot->child.push_back(whileStart);

    //处理E
    Gnode whileSecond = new grammerNode;
    whileSecond->name = "whileThen";
    whileSecond->type = "keyWords";
    vector<Gnode> whileThen = FxE(nowNode->child[5]);
    whileSecond->child = whileThen;
    nowRoot->child.push_back(whileSecond);


        
    return nowRoot;
}

Gnode FxT(treeNode nowNode)
{
    //T->struct id { E }; 
    Gnode nowRoot = new grammerNode;
    nowRoot->name = "struct";
    nowRoot->type = "keyWords";
    Gnode nodeName = new grammerNode;
    string value = nowNode->child[1]->value;
    int row = nowNode->child[1]->row;
    int index = atoi(value.c_str());
    string property = symbolTable[index];
    nodeName->type = "structName";
    bool exist = checkVariable("structName",value);
    if(exist)
    {
        cout<<"结构体名重复定义,行数为："<<row<<endl;
        exit(0);
    }
    nodeName->name = property;
    nowRoot->child.push_back(nodeName);
    isStruct = true;
    vector<Gnode> variableState = FxE(nowNode->child[3]);
    for(int i=0;i<variableState.size();i++)
        nowRoot->child.push_back(variableState[i]);
    isStruct = false;
    return nowRoot;
}



void printAST(Gnode root,int level)
{
    int size = root->child.size();
    for(int i=0;i<level;i++)
        cout<<"--";
    cout<<root->name<<endl;
    int nowLevel = level + 1;
    for(int i=0;i<size;i++)
        printAST(root->child[i],nowLevel);
}



#endif // !HELPSEMANTIC