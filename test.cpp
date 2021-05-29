#include<bits/stdc++.h>
using namespace std;
struct grammerNode {
    string name;
    string type;
    vector<grammerNode*> child;
};
typedef grammerNode* Gnode;    //语法树节点
struct grammerTree {
    Gnode root;
}AST;
Gnode nowFather;
Gnode a(int x){
    Gnode x1 = new grammerNode;
    x1->name ="2";
    nowFather->child.push_back(x1);
    return nowFather;

}
struct tester
{
    
};
int main()
{
    int Ti = 0;
	string result = "t";
    char idT = Ti + '0';
    result += idT;
    cout<<result;


}