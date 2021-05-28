#ifndef HELP_PASER
#define HELP_PASER

#include"helpLex.cpp"

//-----------------------------------------------语法分析变量声明
int pdNum = 0;   //产生式数量
vector<string> VT;   //存储终结符
vector<string> VNT;  //存储非终结符
map<string,set<string> > first;    //first集
map<string,set<string> > follow;   //follow集
map<string,set<string> >:: iterator iter_ff; //生成一个map迭代器
set<string> ::iterator iter_set;   //生成一个set迭代器

map<string,map<string,int> > analyzeTable;   //得到的分析表


struct production{        //存储产生式
    char left;
    string right;
}pd[production_size];

struct analyzeNode   //分析树节点
{
    string name;  //名字
    int level;    //层数
    int productionIndex;   //使用的产生式编号
    string value;       //值
    int row;          //行数
    vector<analyzeNode*> child;  //孩子节点
};

typedef analyzeNode* treeNode;
struct analyzeTree   //构建分析树
{
    analyzeNode* root;
}AT;
vector<string> tokenString;
stack<treeNode> analyzeStack;  //分析栈
stack<string> remainString; //剩余符号串
stack<string> charIndex;//记录字符的位置
stack<string> propertyValue;  //记录token的属性



//获取产生式,并且获取终结符和非终结符集合
void getProduction()
{
    FILE *fpd;
    if((fpd = fopen("production.txt", "r")) == NULL)
    {
        cout<<"打开文法文件失败，语法分析终止"<<endl;
        return;
    }
    char buffer_pd[1024];
    while(fgets(buffer_pd, sizeof(buffer_pd), fpd))
    {
        int len = strlen(buffer_pd);
        pd[pdNum].left = buffer_pd[0];  //产生式左部
        //由于使用正规文法，故产生式左边一定是非终结符
        int vntSize = VNT.size();
        bool flag = 0;
        string start = "";
        start += buffer_pd[0];
        for(int i=0;i<vntSize;i++)     
        {
            if(VNT[i] == start)
            {
                flag = 1;
                break;
            }
        }
        if(!flag)
            VNT.push_back(start);

        string temp = "";
        int i = 3;
        do{
            temp += buffer_pd[i];
            i++;
        }while(i<len-1);
        
        pd[pdNum].right = temp;
        pdNum++;

        string s = "";
        for(int i=0;i<temp.length();i++)   //判断产生式右边的符号
        {
            flag = 0;
            if(temp[i] != ' ')
            {
                s += temp[i];
            }
            else
            {
                if(isNotTerminal(s))
                {
                    for(int j=0;j<VNT.size();j++)
                    {
                        if(VNT[j] == s)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if(!flag)
                        VNT.push_back(s);
                }
                else                            //只要不是非终结符，就是终结符
                {
                    for(int j=0;j<VT.size();j++)
                    {
                        if(VT[j] == s)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if(!flag && s != "#")
                        VT.push_back(s);
                }

                s = "";

            }
            
        }
    }

    cout<<"========产生式如下：=========="<<endl;
    for(int i=0;i<pdNum;i++)
    {
        cout<<pd[i].left<<"->"<<pd[i].right<<endl;
        // cout<<pd[i].right<<pd[i].right.length()<<endl;

    }
        
    
    cout<<"========终结符如下：=========="<<endl;
    for(int i=0;i<VT.size();i++)
        cout<<VT[i]<<" ";
    cout<<endl;
    cout<<"========非终结符如下：========"<<endl;
    for(int i=0;i<VNT.size();i++)
        cout<<VNT[i]<<" ";
    cout<<endl;
}

void getFirstX(string x)   //求出first(x)
{
    set<string> temp;    //存储first(x)
    for(int i=0;i<pdNum;i++)   //遍历每一个产生式
    {
        if(x.length() == 1 && pd[i].left == x[0])  //匹配产生式左部
        {
            int empty = 0,count = 0;   //统计空串数目
            string s ="";
            string r = pd[i].right;
            for(int j=0;j<r.length();j++)
            {
                if(r[j] != ' ')
                {
                    s += r[j];
                }
                    
                else
                {
                    if(!isNotTerminal(s))  //如果右侧第一个元素是终结符
                    {
                        temp.insert(s);
                        break;
                    }
                    else                              //如果是非终结符
                    {
                        bool flag = 0;
                        string s1 = "";
                        for(int j=0;j<pd[i].right.length();j++) //遍历右边每一个字符，直到找到第一个不会推导出空串的非终结符
                        {
                            if(r[j] !=' ')
                                s1 += r[j];
                            else
                            {
                                count++;
                                if(!isNotTerminal(s1))  //如果找到了终结符
                                {
                                    temp.insert(s1);
                                    break;
                                }
                                else   //寻找终结符的first集，查找是否能推导出空串
                                {
                                    getFirstX(s1);
                                    bool flag = 0;
                                    
                                    for(iter_set=first[s1].begin(); iter_set!=first[s1].end(); iter_set++)
                                    {
                                        
                                        if(*iter_set == "#")  //如果找到空串
                                        {
                                            empty++;
                                            flag = 1;
                                        }
                                        else
                                            temp.insert(*iter_set);   //将非空串字符加入此first集
                                    }
                                    if(!flag)
                                        break;
                                }
                                s1 = "";
                            }
                            
                        }
                    if(empty == count)   //如果所有右侧字符都含有空串
                        temp.insert("#");
                    }
                    s = "";
                }
            }
            
        }
    }
    first[x] = temp;   //将first（x）加入集合中
    // cout<<x<<"的first集为:";
    // for(iter_set=temp.begin(); iter_set!=temp.end(); iter_set++)
    //     cout<<*iter_set<<" ";
    // cout<<endl;
}


void getAllFirst()   //获取所有first集
{
    int len = VNT.size();
    for(int i=0; i<len; i++)
        getFirstX(VNT[i]);
    //输出first集
    for(int i=0; i<len; i++)
    {
        set<string> temp = first[VNT[i]];
        cout<<VNT[i]<<"的first集为:";
        for(iter_set=temp.begin(); iter_set!=temp.end(); iter_set++)
            cout<<*iter_set<<" ";
        cout<<endl;
    }

}


void getFollowX(string x)   //获取到Follow(x)
{
    set<string> temp;
    string le_start = "";
    le_start += pd[0].left;
    for(int i=0;i<pdNum;i++)
    {
        //将$加入文法开始符号的follow集
        if(x == le_start)
            temp.insert("$");
        string x_start = "";
        x_start += pd[i].left;
        int len = pd[i].right.length();
        int j = 0;
        string s = "";
        for(j;j<len;j++)
        {
            if(pd[i].right[j] != ' ')
                s += pd[i].right[j];
            else
            {
                if(s == x)
                    break;
                    
                s = "";
            }
            
        }
    
        if(j == len)     //如果产生式右部没有匹配到x
            continue;
        if(j != len - 1)  //匹配到x且不是最后一个字符
        {
            string x_next = "";
            // char x_next = pd[i].right[j+1];
            for(int k=j+1;k<len;k++)
            {
                if(pd[i].right[k] != ' ')
                    x_next += pd[i].right[k];
                else
                    break;
            }
            if(!isNotTerminal(x_next))   //x之后第一个字符就是终结符
            {
                temp.insert(x_next);
                continue;
            }
            else  //非终结符
            {
                set<string> x_next_first = first[x_next];
                bool have_empty = 0;
                for(iter_set=x_next_first.begin();iter_set!=x_next_first.end(); iter_set++)
                {
                    if(*iter_set == "#")
                        have_empty = 1;
                    else
                        temp.insert(*iter_set);
                }
                
                if(have_empty && x != x_start)
                {
                    getFollowX(x_start);
                    set<string> x_left_follow = follow[x_start];
                    for(iter_set=x_left_follow.begin();iter_set!=x_left_follow.end(); iter_set++)
                            temp.insert(*iter_set);
                }

            }
        }
        else if(j == len - 1 && x != x_start)  //如果字符是右侧最后一个
        {
            getFollowX(x_start);
            set<string> x_left_follow = follow[x_start];
            for(iter_set=x_left_follow.begin();iter_set!=x_left_follow.end(); iter_set++)
                temp.insert(*iter_set);
        }
    }
    follow[x] = temp;
    // cout<<x<<"的follow集为:";
    // for(iter_set=temp.begin(); iter_set!=temp.end(); iter_set++)
    //     cout<<*iter_set<<" ";
    // cout<<endl;
}

void getAllFollow()   //获取所有的follow集
{
    int len = VNT.size();
    for(int i=0;i<len;i++)
        getFollowX(VNT[i]);
    //输出所有follow集
    for(int i=0; i<len; i++)
    {
        set<string> temp = follow[VNT[i]];
        cout<<VNT[i]<<"的follow集为:";
        for(iter_set=temp.begin(); iter_set!=temp.end(); iter_set++)
            cout<<*iter_set<<" ";
        cout<<endl;
    }

}

void getAnalyzeTable()
{
    //初始化分析表
    for(int i=0;i<VNT.size();i++)
    {
        map<string,int> temp;
        for(int j=0;j<VT.size();j++)
            temp[VT[j]] = -1;         //-1表示空（错误的情况）
        temp["$"] = -1;
        analyzeTable[VNT[i]] = temp;
    }
    //遍历所有产生式生成分析表
    for(int i=0;i<pdNum;i++)
    {
        string pdLeft = "";
        pdLeft += pd[i].left; 
        string right_first = "";
        for(int j=0;j<pd[i].right.length();j++)
        {
            if(pd[i].right[j] != ' ')
                right_first += pd[i].right[j];
            else
                break;
        }
        if(right_first == "#") //如果是空串
        {
            set<string> followA = follow[pdLeft];
            for(iter_set=followA.begin();iter_set!=followA.end(); iter_set++) 
                analyzeTable[pdLeft][*iter_set] = i;
        }
        else if(!isNotTerminal(right_first)) //如果是终结符
            analyzeTable[pdLeft][right_first] = i;
        else   //如果右侧是非终结符
        {
            set<string> first_temp = first[right_first];
            for(iter_set=first_temp.begin(); iter_set!=first_temp.end(); iter_set++)
                analyzeTable[pdLeft][*iter_set] = i;
        }
    }

    //生成同步控制信号

    for(int i=0;i<VNT.size();i++)
    {
        set<string> temp = follow[VNT[i]];
        for(iter_set=temp.begin(); iter_set!=temp.end();iter_set++)
            if(analyzeTable[VNT[i]][*iter_set] == -1)
                analyzeTable[VNT[i]][*iter_set] = -2;   //-2表示同步信息

    }

    map<string,map<string,int> > :: iterator iterMapMap; //生成一个迭代器
    cout<<"==========生成的分析表为============"<<endl;
    // cout<<noshowpos << setw(15) << left<<"//////";
    for(int i=0;i<VT.size();i++)
    {
        // cout<<noshowpos << setw(15) << left<<" ";
        
        if(i % 10 == 0)
        {
            cout<<endl;
            cout<<noshowpos << setw(15) <<left<<" ";
        }
        cout<<noshowpos << setw(15) << left<<VT[i];
        
    }
    // cout<<setiosflags(ios::left)<<setw(6)<<"$"<<endl;  
    cout<<noshowpos << setw(15) << left<<"$"<<endl;  

    for(int i=0;i<VNT.size();i++)
    {
        
        cout<<noshowpos << setw(15) << left<<VNT[i];
        // cout<<VT.size()<<"  sss   "<<endl;
        for(int j=0;j<VT.size();j++)
        {
            string output = "";
            int index1 = analyzeTable[VNT[i]][VT[j]];
            if(j % 10 == 0)
            {
                

                cout<<endl;
                cout<<noshowpos << setw(15) << left<<" ";

            }
            
            if(index1 == -1)
                cout<<noshowpos << setw(15) << left<<"-1";
            else if(index1 == -2)
                cout<<noshowpos << setw(15) << left<<"-2";
            else
            {
                output += pd[index1].left;
                output += "->";
                output += pd[index1].right;
                // output = pd[index1].left + "->" + pd[index1].right;
                cout<<noshowpos << setw(15) << left<<output;
            }
        }
        int index2= analyzeTable[VNT[i]]["$"];
        string output = "";
        
        if(index2 == -1)
            cout<<noshowpos << setw(15) << left<<"-1"<<endl;
        else if(index2 == -2)
            cout<<noshowpos << setw(15) << left<<"-2"<<endl;
        else
        {
            output += pd[index2].left;
            output += "->";
            output += pd[index2].right;            
            cout<<noshowpos << setw(15) << left<<output<<endl;
        }
        
    }
}
//语法分析过程
void initParser()
{
    treeNode startnode = new analyzeNode;
    startnode->name = "$";
    analyzeStack.push(startnode);
    AT.root = new analyzeNode;
    AT.root->name = "S";
    AT.root->level = 0;
    analyzeStack.push(AT.root);
    remainString.push("$");
}

//错误处理
void dealGrammerError()
{
    int len1 = analyzeStack.size(),len2 = remainString.size();
    int minsize = min(len1,len2);
    for(int i=0;i<minsize;i++)
    {
        treeNode topNode = analyzeStack.top();
        string topStack = topNode->name;
        string topString = remainString.top();
        if(!isNotTerminal(topStack) && topStack != topString)
            analyzeStack.pop();
        else if(analyzeTable[topStack][topString] == -1)
        {
            remainString.pop();
            charIndex.pop();
            propertyValue.pop();
        }
            
        else if(analyzeTable[topStack][topString] == -2)
            analyzeStack.pop();
    }

}
//分析符号串
void parserString(vector<string> tokenline)
{
    stack<treeNode> helpNodeStack;  //辅助站，帮助输出
    stack<string> helpRemainStack;
    for(int i=tokenline.size()-1;i>=0;i--)
        remainString.push(tokenline[i]);
    cout<<"分析栈元素"<<"      "<<"剩余符号串元素"<<endl;

    while(remainString.size() != 0)
    {

        string analyzetop = analyzeStack.top() ->name;
        string remaintop = "";
        remaintop += remainString.top();    
        //输出分析栈和剩余字符串
        string stackTemp = "",remainTemp = "";

        int lenan = analyzeStack.size(),lens = remainString.size();
        for(int i=0;i<lenan;i++)
        {
            treeNode analyze = analyzeStack.top();
            stackTemp += analyze ->name;
            analyzeStack.pop();
            helpNodeStack.push(analyze);
        }
        for(int i=0;i<lens;i++)
        {
            string tmp = remainString.top();
            remainTemp += tmp;
            remainString.pop();
            helpRemainStack.push(tmp);
        }
        cout.width(10);
        cout << stackTemp;cout<<"      ";
        cout.width(30);
        cout << remainTemp<< "\n";
        for(int i=0;i<lenan;i++)
        {
            treeNode analyze = helpNodeStack.top();
            helpNodeStack.pop();
            analyzeStack.push(analyze);
        }

        for(int i=0;i<lens;i++)
        {
            string tmp = helpRemainStack.top();
            helpRemainStack.pop();
            remainString.push(tmp);
        }

        if(analyzetop == remaintop && analyzetop == "$") //分析到栈底符号
        {
            cout<<"语法分析完成"<<endl;
            return;
        }
        if(analyzetop == remaintop)   //匹配到终结符
        {
            treeNode topNode = analyzeStack.top();
            topNode->value = propertyValue.top();
            int row = atoi(charIndex.top().c_str());
            topNode->row = row;
            analyzeStack.pop();
            remainString.pop();
            charIndex.pop();
            propertyValue.pop();

        }
        else if(analyzeTable[analyzetop][remaintop] != -1 && analyzeTable[analyzetop][remaintop] != -2)
        {
            int index = analyzeTable[analyzetop][remaintop];
            treeNode topNode = analyzeStack.top();
            topNode->productionIndex = index;
            analyzeStack.pop();
            string pdRight = pd[index].right;
            if(pdRight != "# ")
            {
                int len = pdRight.length();
                string t = "";
                for(int i=len-2; i>=0;i--)
                {
                    if(i == 0)
                    {
                        t += pdRight[i];
                        reverse(t.begin(),t.end());
                        treeNode tempnode = new analyzeNode;
                        tempnode->name = t;
                        tempnode->level = topNode->level + 1;
                        tempnode->value = " ";
                        analyzeStack.push(tempnode);
                        // topNode->child.push_back(tempnode);
                        topNode->child.insert(topNode->child.begin(),tempnode);
                        if(analyzetop == ")" && remaintop != ")")
                        {
                            cout<<"缺少右括号,且位置为："<<charIndex.top()<<"行"<<endl;
                        }
                        t = "";
                    }
                    else if(pdRight[i] != ' ')
                        t += pdRight[i];
                    else
                    {
                        reverse(t.begin(),t.end());
                        treeNode tempnode = new analyzeNode;
                        tempnode->name = t;
                        tempnode->level = topNode->level + 1;
                        tempnode->value = " ";
                        analyzeStack.push(tempnode);
                        topNode->child.insert(topNode->child.begin(),tempnode);
                        if(analyzetop == ")" && remaintop != ")")
                        {
                            cout<<"缺少右括号,且位置为："<<charIndex.top()<<"行"<<endl;
                        }
                       
                        
                        t = "";
                    }
                    // cout<<"t:"<<t<<endl;

                    
                }
            }
            else
            {
                treeNode tempnode = new analyzeNode;
                tempnode->name = "#";
                tempnode->level = topNode->level + 1;
                tempnode->value = " ";
                topNode->child.insert(topNode->child.begin(),tempnode);
            }
           
        }
        else
        {
            
            if(analyzetop == "X" && remaintop == "id")
            {
                cout<<"缺少运算符，且行数为："<<charIndex.top()<<"行"<<endl;
            }
            else if(analyzetop == "V" && remaintop == ";")
            {
                cout<<"缺少操作数，且行数为:"<<charIndex.top()<<"行"<<endl;
            }
            else
            {
                cout<<analyzetop<<"   "<<remaintop<<endl;
                cout<<"发生错误,且位置为："<<charIndex.top()<<"行"<<endl;
            }
                
            dealGrammerError();
            // return;
        }
    }

}

void printTree(analyzeTree& tree)
{
    cout<<"==========语法树如下============"<<endl;
    treeNode root = tree.root;
    stack<treeNode> printStack;
    printStack.push(root);
    while(printStack.size() != 0)
    {
        treeNode topNode = printStack.top();
        for(int i =0;i<topNode->level;i++)
            cout<<"--";
        cout<<topNode->name<<endl;
        // cout<<topNode->name<<topNode->productionIndex<<" "<<topNode->value<<endl;
        printStack.pop();
        int len = topNode->child.size();
        for(int i=len-1;i>=0;i--)
            printStack.push(topNode->child[i]);
    }
}

void parser()
{
    initParser();
    FILE* inputLexfile;
    if((inputLexfile = fopen("lexResult.txt","r")) == NULL)
    {
        cout<<"读入词法分析结果文件错误，编译失败"<<endl;
        return;
    }
    vector<string> index_temp;
    vector<string> value_temp;
    while(fgets(Buffer,buffer_size,inputLexfile))
    {
        // cout<<Buffer<<endl;
        int len = strlen(Buffer);
        string s = "";
        for(int i=1;i<len;i++)
        {
            if(Buffer[i] != ' ')
                s += Buffer[i];
            else
                break;
        }
        int index = 0;
        string index_row = "";
        //找行数
        for(int i=10;i<len;i++)
        {
            if(Buffer[i] == ' ' && Buffer[i-1] == '>')
                index = i+1;
        }
        for(int i=index;i<len-1;i++)
        {
            index_row += Buffer[i];
        }
        //找属性值
        index_temp.push_back(index_row);
        tokenString.push_back(s);
        string value = "";
        for(int i=1;i<len;i++)
        {
            if(Buffer[i] == ',')
            {
                index = i+1;
                break;
            }
                
        }
        bool flag = false;
        for(int i=index;i<len-1;i++)
        {
            if(Buffer[i] != ' ' && !flag)
                value+=Buffer[i];
            if(Buffer[i] != ' ' && Buffer[i+1] == ' ')
                flag = true;
        }
        value_temp.push_back(value);
    }
    for(int i=index_temp.size()-1;i>=0;i--)
        charIndex.push(index_temp[i]);
    for(int i=value_temp.size()-1;i>=0;i--)
        propertyValue.push(value_temp[i]);
    parserString(tokenString);

}
#endif // !HELP_PASER