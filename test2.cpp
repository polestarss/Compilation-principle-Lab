#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<iomanip>
#include<algorithm>
#include<string>
#include<vector>
#include<cstring> 
#include<map>
#include<set>
using namespace std;

constexpr auto Buffer_len = 2048;
constexpr auto Half_Buffer_len = 1024;

//表达式于记号表
vector<pair<string, string> > MaketTable;
//记录每一个单词的数量
map<string, int> NumOfWord;
//记录错误的位置
set<int> Errorlocation;  
//记录程序中的字符总数
long int char_num;
//记录源程序中的语句行数
long int statement_lines;
//符号表
map<string,int> SymbolTable;
//符号表指针
int SymbolPtr = 0;
//提取字符
char ExtractingWords(int& StartPointer, int& ForwardPointer, int& cnt, char* Buffer);
//将字符连接在token之后
void cat(string& token, const char& ch);
//判断是否是字符
bool Is_letter(const char& ch);
//判断是否是数字
bool Is_digit(const char& ch);
//向前指针后退一个字符
void retract(const char* Buffer, int& ForwardPointer);
//判断是否是关键字
string CheckKey(const string &token);
//文件输出分析后的单词
void FileOutResult(const string& str,string sttribute = "-");
//错误处理(方法：记录当前错误地行号，并且跳过当前地字符)
void error();
//输出统计结果
void ShowResult();   
//分析后的结果输出到该文件
ofstream F_Out("result.txt", ios::out);

int main()
{
	//记号表存储初始化与数据读取
	ifstream F_MaketTable("Makert_table.txt", ios::in);

	string expression = "", mark = "";
	if (!F_MaketTable.is_open())
		cout << "Open file failure" << endl;
	while (!F_MaketTable.eof()){            // 若未到文件结束一直循环
		F_MaketTable >> expression >> mark;
		MaketTable.push_back(make_pair(expression, mark));
	}
	F_MaketTable.close();   //关闭文件

	char_num = 0, statement_lines = 1;
	char ch;

	/**变量声明
	* left_len : 左缓冲区数量剩余容量
	* right_len : 右缓冲区数量剩余容量
	* StartPointer : 开始指针
	* ForwardPointer : 向前指针
	* state : 状态
	*/
	int cnt = 0, StartPointer = 0, ForwardPointer = Buffer_len - 1, state = 0;
	char Buffer[Buffer_len];  //缓冲区数组
	memset(Buffer, '\0', sizeof(Buffer));

	//将输入流定向于文件
	if (NULL == freopen("test.txt", "r", stdin)) {
		cout << "test.txt打开失败" << endl;
		cout << "FAILED!" << endl;
		return 0;
	}

	F_Out << "词法分析结果如下所示:" << endl;
	F_Out << "[Line " << statement_lines << "]:" << endl;
	string token;	//存储临时提取的单词
	do {
		switch (state) {
		case 0:{
			token = "";
			//反复调用TakeWord,去掉空格
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			} while (ch == ' ' || ch == '\t');
			switch (ch) {
				//设置标识符状态
				case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':
				case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':
				case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case '_':
				case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':
				case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':
				case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':state = 1; break;
				//设置常数状态
				case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
				state = 2; break;
				//设置运算符状态
				case '<':state = 8; break;
				case '>':state = 9; break;
				/*  ?/:/^/&/|/%/(/)/[/]/{/}/,/;  */
				case'?':case':':case'^':case'%':case'(':case')':case'[':case']':case'{':
				case'}':case',':case';':case '~':{
					string temp_ch = "";
					temp_ch += ch;
					NumOfWord[temp_ch] += 1;   //统计单词个数
					FileOutResult(temp_ch);
					state = 0;
					break;
				}
				case '=':state = 10; break;    //等号或者判断是否相等时的逻辑符号
				case '+':state = 11; break;    //加或者加等或者加加
				case '-':state = 12; break;    //减或者减等或者减减
				case '*':state = 13; break;    //乘或者乘等符号
				case '/':state = 14; break;    //解释符号或者除号处理
				case '!':state = 17; break;    //否或者不等号处理
				case '\n':state = 18; break;   //回车处理
				case '"':state = 19; break;    //（字符串）语句处理
				case '&':state = 20; break;    //逻辑与或者取地址
				case '|':state = 21; break;    //或与逻辑或
				case '#':state = 22; break;    //头文件等处理
				case '\'':state = 23; break;   //处理单引号标志
				default:state = 24; break;     
			}
			break;
		}
		case 1: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			if (Is_letter(ch) || Is_digit(ch)) state = 1;
			else {
				//后退一步前向指针
				retract(Buffer,ForwardPointer);
				//转向读取新的单词
				state = 0;
				//判断是否是关键字
				string ans = CheckKey(token);
				//是关键字时，输出到文件中
				if (ans != "") {
					NumOfWord[token] += 1;   //统计单词个数
					FileOutResult(ans);
				}
				//不是关键字而是标识符时
				else {
					NumOfWord["id"] += 1;   //统计单词个数
					string attribute = "";
					auto mptr = SymbolTable.find(token);
					//存在时，即提取出属性（入口指针）
					if (mptr != SymbolTable.end()) {
						attribute = to_string(mptr->second);
					}
					//不存在时，插入，并分配入口指针
					else {
						SymbolTable.insert(pair<string,int>(token, SymbolPtr));
						attribute = to_string(SymbolPtr);
						SymbolPtr++;
					}
					FileOutResult(CheckKey("id"),token);
				}
			}
			break;
		}
		case 2: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				//后面跟着数字
				case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state = 2; break;
				//小数
				case '.':state = 3; break;
				//科学计数法
				case 'E':state = 5; break;
				default: {
					NumOfWord["num"] += 1;   //统计单词个数
					//后退一步前向指针
					retract(Buffer, ForwardPointer);
					//转向读取新的单词
					state = 0;
					FileOutResult(CheckKey("num"),token);
					break;
				}
			}
			break;
		}
		case 3: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			if (Is_digit(ch)) { state = 4; break; }   //如果是数字，则跳转到状态4 
			//遇到错误 
			else {
				error();
				state = 0;
			}
			break;
		}
		case 4: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state = 4; break;
				case 'E':case 'e':state = 5; break;    //后面为数字或者字母E or e 
				default: {
					retract(Buffer, ForwardPointer);
					state = 0;
					NumOfWord["num"] += 1;   //统计单词个数
					FileOutResult(CheckKey("num"),token);
					break;
				}
			}
			break;
		}
		case 5: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state = 7; break;
				case '+':case'-':state = 6; break;    //后面跟着+ or -或者数字时 
				default: {
					retract(Buffer, ForwardPointer);   //回退一个 
					error();   //记录错误行号 
					state = 0;
					break;
				}
			}
			break;
		}
		case 6: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			if (Is_digit(ch)) { state = 7; break; }   //遇到数字，则跳转到状态7 
			else {
				retract(Buffer, ForwardPointer);    //回退一个 
				error();       //记录错误行号 
				state = 0;
				break;
			}
			break;
		}
		case 7: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			if (Is_digit(ch)) { state = 7; break; }
			else {
				NumOfWord["num"] += 1;   //统计单词个数
				FileOutResult(CheckKey("num"),token);
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			break;
		}
		case 8: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				//存在小于或者小于等于地两种情况时 
				case '<':cat(token, ch); NumOfWord[token] += 1; state = 0; FileOutResult(CheckKey("<<")); break;
				case '=':cat(token, ch); NumOfWord[token] += 1; state = 0; FileOutResult(CheckKey("<=")); break;
				default: {
					NumOfWord[token] += 1;
					FileOutResult(CheckKey("<"));
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			break;
		}
		case 9: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				//存在大于或者大于等于时 
				case '>':cat(token, ch); NumOfWord[token] += 1; state = 0; FileOutResult(CheckKey(">>")); break;
				case '=':cat(token, ch); NumOfWord[token] += 1; state = 0; FileOutResult(CheckKey(">=")); break;
				default: {
					NumOfWord[token] += 1;
					FileOutResult(CheckKey(">"));
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			break;
		}
		case 10: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			//存在等于或者判断是否相等地符号时 
			if (ch == '=') {
				cat(token, ch); NumOfWord[token] += 1;state = 0; FileOutResult(CheckKey("=="));
			}
			else {
				NumOfWord["assign-op"] += 1;
				FileOutResult(CheckKey("="));
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			break;
		}
		case 11: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				//判断加号或者加+=时 
				case '+':cat(token, ch);state = 0; FileOutResult(CheckKey("++")); break;
				case '=':cat(token, ch);state = 0; FileOutResult(CheckKey("+=")); break;
				default: {
					FileOutResult(CheckKey("+"));
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			NumOfWord[token] += 1;
			break;
		}
		case 12: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				//判断-=或者减号时 
				case '-':cat(token, ch); state = 0; FileOutResult(CheckKey("--")); break;
				case '=':cat(token, ch); state = 0; FileOutResult(CheckKey("-=")); break;
				default: {
					FileOutResult(CheckKey("-"));
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			NumOfWord[token] += 1;
			break;
		}
		case 13:{
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			//判断*或者*=时 
			if (ch == '=') {
				cat(token, ch); state = 0; FileOutResult(CheckKey("*="));
			}
			else {
				FileOutResult(CheckKey("*"));
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			NumOfWord[token] += 1;
			break;
		}
		case 14: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			switch (ch) {
				case'*':state = 15; break;   //为“/* ”时 
				case'/':state = 16; break;   //为“// ”时 
				case'=':cat(token, ch); NumOfWord[token] += 1; state = 0; FileOutResult(CheckKey("/=")); break;  //为“/= ”时 
				default: {
					NumOfWord[token] += 1;
					FileOutResult("/");
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			break;
		}
		case 15: {
			//判断"*/" 
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				if (ch == '\n') {    //遇到回车时，即换一行，行数增加一 
					NumOfWord["enter"] += 1;
					statement_lines++;
					F_Out << "[Line " << statement_lines << "]:" << endl;     //输出每一行的行号 
				}
			} while (ch != '*' && ch != EOF);
			//遇到文件结束符时 
			if (ch == EOF) {
				error();
				break;
			}
			else {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				if (ch == '/') state = 0;
				else
					state = 15;
				break;
			}
		}
		case 16: {
			//判断“// ”的结束 
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			} while (ch != '\n' && ch != EOF);
			retract(Buffer, ForwardPointer);
			state = 0;
			break;
		}
		case 17: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			//为“!= ”时 
			if (ch == '=') {
				cat(token, ch); state = 0; FileOutResult(CheckKey("!="));
			}
			//为 “！”时 
			else {
				FileOutResult(CheckKey("!"));
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			NumOfWord[token] += 1;   //统计单词个数
			break;
		}
		case 18: {
			//遇见回车时 
			NumOfWord["enter"] += 1;
			++statement_lines;
			F_Out << "[Line " << statement_lines << "]:" << endl;
			state = 0;
			break;
		}
		case 19: {
			//遇见双引号时 
			string str = "";
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				str += ch;
			} while (ch != '"' && ch != '\n' && ch != EOF);
			if (ch == '"') {
				str.erase(str.end() - 1);
				FileOutResult(CheckKey("\"\""),str);
				NumOfWord["literal"] += 1;
			}
			else {
				Errorlocation.insert(statement_lines);    //记录该错误所在的行号
				retract(Buffer, ForwardPointer);  //回退一步，进行相应处理
			}
			state = 0;
			break;
		}
		case 20: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			//为“&& ”时 
			if (ch == '&') {
				cat(token, ch); state = 0; FileOutResult(CheckKey("&&"));
			}
			//为 “& ”时 
			else {
				FileOutResult(CheckKey("&"));
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			NumOfWord[token] += 1;
			break;
		}
		case 21: {
			cat(token, ch);
			ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
			//为““|| ”时 
			if (ch == '|') {
				cat(token, ch); state = 0; FileOutResult(CheckKey("||"));
			}
			//为“|”时 
			else {
				FileOutResult(CheckKey("|"));
				retract(Buffer, ForwardPointer);
				state = 0;
			}
			NumOfWord[token] += 1;
			break;
		}
		case 22: {
			//头文件的处理 
			string str = "";
			cat(token, ch);
			NumOfWord[token] += 1;
			FileOutResult(CheckKey(token));
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				str += ch;
			} while (Is_letter(ch) && ch != EOF);
			if(ch == EOF){
				retract(Buffer, ForwardPointer);
				state = 0;
				break;
			}
			str.erase(str.end() - 1);
			retract(Buffer, ForwardPointer);
			//为头文件时或者为宏定义时
			int flag = 0;
			if (str == "include" || str == "define") {
				flag = 1;
				NumOfWord[str] += 1;
				FileOutResult(CheckKey(str));
				//消除空格
				do {
					ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				} while (ch == ' ');

				//自定义文件头
				if (str == "include" && (ch == '"' || ch == '<')) {
					str = "";
					do {
						ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
						str += ch;
					} while (ch != '\n' && ch != EOF);
					if (ch == EOF) {
						retract(Buffer, ForwardPointer);
						state = 0;
						break;
					}
					retract(Buffer, ForwardPointer);
					str.erase(str.end() - 1);
					str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
					auto p = str.find('.');
					if (p == str.npos) {
						flag = 0;
					}
					else {
						if (p + 2 < str.size() && str[p + 1] == 'h' && (str[p + 2] == '\"' || str[p + 2] == '>')) {
							str.erase(str.end() - 1);
							NumOfWord["header_file"] += 1;
							FileOutResult("header_file",str);
						}
						else
							flag = 0;
					}
				}
				else if (str == "define") {
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
				else {
					flag = 0;
				}
			}
			//两者都不是时，即错误
			if(flag == 0) {
				error();
			}
			state = 0;
			break;
		}
		case 23:{
			//单引号的处理 
			string str = "";
			do {
				ch = ExtractingWords(StartPointer, ForwardPointer, cnt, Buffer);
				str += ch;
			} while (ch != '\'' && ch != '\n' && ch != EOF);
			if (ch == '\'') {
				str.erase(str.end() - 1);
				FileOutResult(CheckKey("''"), str);
				NumOfWord["ch"] += 1;
			}
			else {
				Errorlocation.insert(statement_lines);    //记录该错误所在的行号
				if (ch == EOF) {
					retract(Buffer, ForwardPointer);
					state = 0;
					break;
				}
			}
			state = 0;
			break;
		}
		case 24: {
			//错误处理 
			error();
			state = 0;
			break;
		}
		}
	} while (ch != EOF);
	fclose(stdin);//关闭文件

	//输出统计信息到输出文件中
	ShowResult(); 
	F_Out.close();
	cout << "Lexical Analysis Succeed!" << endl;
	return 0;
}

char ExtractingWords(int& StartPointer, int& ForwardPointer, int& cnt, char* Buffer)
{
	char ch;   //存储临时读入的字符
	//向前指针在左半区的终点时

	if (ForwardPointer == Half_Buffer_len - 1) {
		while (cnt < Buffer_len && (ch = getchar()) != EOF) {
			Buffer[cnt++] = ch;
		}
		//读完文件时，一般的缓冲区没有填满时，即处于结束状态
		if (ch == EOF) {
			Buffer[cnt++] = EOF;
		}
		cnt = 0;
		++ForwardPointer;
	}
	//向前指针在右半区的终点时
	else if (ForwardPointer == Buffer_len - 1) {
		while (cnt < Half_Buffer_len && (ch = getchar()) != EOF) {
			Buffer[cnt++] = ch;
		}
		//读完文件时，一般的缓冲区没有填满时，即处于结束状态
		if (ch == EOF) {
			Buffer[cnt++] = EOF;
		}
		ForwardPointer = 0;
	}
	else {
		++ForwardPointer;
	}
	if(Buffer[ForwardPointer] == '\n' || Buffer[ForwardPointer] == '\t')
		char_num += 2;  //字符总数加二
	else
		char_num ++;   //字符总数加一
	return Buffer[ForwardPointer];
}

 
void cat(string& token, const char& ch)
{
	token += ch;
}

bool Is_letter(const char& ch)
{
	return (isalpha(ch) || ch == '_');
}

bool Is_digit(const char& ch)
{
	return isdigit(ch);
}

void retract(const char* Buffer,int &ForwardPointer)
{
	//当目前所读的字母为回车或者制表符号的时候 ，字符数目 -2  
	if (Buffer[ForwardPointer] == '\n' || Buffer[ForwardPointer] == '\t')
		char_num -= 2;
	else
		--char_num;  //字符总数减一
	--ForwardPointer;
	return;
}

string CheckKey(const string & token)
{
	string str = "";
	for (auto it : MaketTable) {
		if (it.first == token)
			return it.second;
	}
	return str;
}

void FileOutResult(const string &str, string attribute)  
{
	//输出格式按照 “<记号，属性>”输出 
	F_Out <<"	< ' "<< str <<" '  ,  "<< attribute <<" >" << endl;
}

void error()
{
	Errorlocation.insert(statement_lines);    //记录该错误所在的行号
	return;
}

void ShowResult()
{
	F_Out << endl << "-------以上源程序语句分析后的统计结果如下所示-------" << endl;
	F_Out << "语句行数：" << statement_lines << endl << endl;
	F_Out << "字符总数：" << char_num << endl << endl;
	F_Out << "单词数目(单词，单词数量)：" << endl;
	for (auto it : NumOfWord) {
		F_Out << "( “ "<< it.first <<" ” , " << it.second <<" ) "<< endl;
	}
	F_Out << endl;
	if (Errorlocation.empty())
		F_Out << "源程序中不存在词法错误信息" << endl;
	else {
		F_Out << "源程序中存在的词法错误，行号如下所示：" << endl << "Line: ";
		for (auto it : Errorlocation)
			F_Out << "  " << it;
	}
	return;
}
