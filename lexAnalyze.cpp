#include "helpLex.cpp"  //辅助进行判断的函数


void LexAnalyze()
{
    if((fpInput = fopen("test.txt","r")) == NULL)
    {
        cout<<"读入文件名错误，编译失败"<<endl;
        return ;
    }

    while(fgets(Buffer, buffer_size, fpInput))
    {
        int len = strlen(Buffer);
        charNum += len;
        int lexemebegin = 0,forward = 0;         
        row++;
        while(forward != len)
        {
            switch(state)
            {
                case 0:
                {
                    token = "";
                    do
                    {
                        C = getChar(lexemebegin,forward,len);   //消除多余空格
                    } while (C == ' ' || C == '\t');
                    lexemebegin = forward;
                    if(isLetter(C) || C == '_')   //如果首字符是字母，状态设置为1 
                    {
                        state = 1;
                        break;
                    }
                    else if(isDigit(C))   //如果首字符是数字状态设置为2
                    {
                        if(C == '0')     //如果第一个数字是0则需要额外判断是什么进制数
                            state = 20;
                        else
                            state = 2;
                        break;
                    }
                    
                    switch(C)
                    {
                        case '<': state = 8;  break;   //还有<=和<>
                        case '>': state = 9;  break;   //>=
                        case '=': state = 10; break; //==
                        case '+': state = 11; break; //++
                        case '-': state = 12; break; //--
                        case '*': state = 13; break; //*=
                        case '/': state = 14; break; ///=,//,/*
                        case '!': state = 15; break;
                        case '&': state = 16; break; //&&
                        case '|': state = 17; break; //||
                        case '(': case ')': case '[': case ']': case '{' : case '}' :
                        case ',': case ';': 
                        {
                            cout<<"< "<<C<<" , "<<"-"<<" >"<<endl;
                            string temp = "";
                            temp += C;
                            outResultToFile(temp,"-");
                            wordNum[temp]++;
                            state = 0;
                            break;
                        }
                        default: break;  //To do
                    }
                    break;
                }
                case 1: 
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C) || isLetter(C) || C == '_')        //如果继续是数字或者字符
                        state = 1;
                    else                                //如果是其他字符则终止
                    {
                        forward--;
                        state = 0;
                        if(isReserve(token))
                        {
                            cout<<"< "<<token<<" , "<<"keyword"<<" >"<<endl;
                            outResultToFile(token,"keyword");
                            wordNum["keyWord"]++;

                        }
                        else
                        {
                            if(symbolNum == 0)
                            {
                                symbolTable[0] = token;
                                cout<<"< "<<"id"<<" , "<<symbolNum<<" >"<<endl;
                                char t[20] = {0};
                                sprintf(t,"%d",symbolNum);
                                string attribute = t;
                                outResultToFile("id",attribute);
                                symbolNum++;
                                wordNum["id"]++;
                            }
                            else
                            {
                                wordNum["id"]++;
                                int flag = 0;
                                for(int i=0;i<symbolNum;i++)
                                {
                                    if(token == symbolTable[i])
                                    {
                                        cout<<"< "<<"id"<<" , "<<i<<" >"<<endl;
                                        char t[20] = {0};
                                        sprintf(t,"%d",i);
                                        string attribute = t;
                                        outResultToFile("id",attribute);
                                        flag = 1;
                                        break;
                                    }
                                }
                                if(flag == 0)
                                {
                                    symbolTable[symbolNum] = token;
                                    cout<<"< "<<"id"<<" , "<<symbolNum<<" >"<<endl;
                                    char t[20] = {0};
                                    sprintf(t,"%d",symbolNum);
                                    string attribute = t;
                                    outResultToFile("id",attribute);
                                    symbolNum++;
                                }
                            }
                        }
                            
                    }
                    break;
                }
                case 2:
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))
                    {
                        state = 2;
                        break;
                    }
                    else if(isLetter(C))
                    {
                        cout<<"error! loacted in row:"<<row<<endl;
                        cout<<"The reason: this is a wrong number or id"<<endl;
                        addressError(lexemebegin,forward,len);
                        state = 0;
                        break;
                    }
                    else if(C == '.')
                    {
                        state = 3;
                        break;
                    }
                    else if(C == 'E' || C == 'e')
                    {
                        state == 5;
                        break;
                    }
                    else   //识别出整常数
                    {
                        wordNum["num"]++;
                        forward--;
                        state = 0;
                        cout<<"< "<<"num"<<" , "<<token<<" >"<<endl;
                        outResultToFile("num",token);
                    }

                    break;
                }
                case 3:         //小数点状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))                  
                    {
                        state = 4; 
                        break;
                    }
                    else
                    {   
                        cout<<"error! loacted in row:"<<row<<endl;
                        cout<<"The reason: this is a wrong float number"<<endl;
                        addressError(lexemebegin,forward,len);
                        state = 0;
                        break;
                    }
                }

                case 4:
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))
                    {
                        state = 4;
                        break;
                    }
                    else if(C == 'e' || C == 'E')     //如果使用的是科学计数法
                    {
                        state = 5;
                        break;
                    }
                    else              //如果识别到其他字符，则完成对小数的识别
                    {
                        wordNum["num"]++;
                        forward--;
                        state = 0;
                        cout<<"< "<<"num"<<" , "<<token<<" >"<<endl;
                        outResultToFile("num",token);
                        break;
                    }
                }

                case 5:    //处理指数状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))
                    {
                        state = 7;
                        break;
                    }
                    else if(C == '+' || C == '-')
                    {
                        state = 6;
                        break;
                    }
                    else
                    {
                        cout<<"error! loacted in row:"<<row<<endl;
                        cout<<"The reason: Scientific notation is wrong"<<endl;
                        addressError(lexemebegin,forward,len);
                        state = 0;
                        break;
                    }

                }
                case 6:        //例子： 1e-8
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))
                    {
                        state = 7;
                        break;
                    }
                    else
                    {
                        cout<<"error! loacted in row:"<<row<<endl;
                        cout<<"The reason: Scientific notation is wrong"<<endl;
                        addressError(lexemebegin,forward,len);
                        state = 0;
                        break;
                    }
                }

                case 7:        //无符号数
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(isDigit(C))
                    {
                        state = 7;
                        break;
                    }
                    else
                    {
                        wordNum["num"]++;
                        forward--;
                        state = 0;
                        cout<<"< "<<"num"<<" , "<<token<<" >"<<endl;
                        outResultToFile("num",token);
                        break;
                    }
                }

                case 8: //<
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '<')   //移位运算符
                    {
                        state = 0;
                        cout<<"< "<<"<<"<<" , "<<"shift"<<" >"<<endl;
                        outResultToFile("<<","shift");
                        wordNum["operator"]++;
                        break;
                    }
                    else if(C == '=') //<=
                    {
                        state = 0;
                        cout<<"< "<<"<="<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile("<=","relop");
                        wordNum["operator"]++;
                        break;
                    }
                    else 
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"<"<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile("<","relop");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 9: //>状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '>')
                    {
                        state = 0;
                        cout<<"< "<<">>"<<" , "<<"shift"<<" >"<<endl;
                        outResultToFile(">>","shift");
                        wordNum["operator"]++;
                        break;
                    }
                    else if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<">="<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile(">=","relop");
                        wordNum["operator"]++;
                        break;
                    }
                    else 
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<">"<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile(">","relop");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 10: //=状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<"=="<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile("==","relop");
                        wordNum["operator"]++;
                        break;
                    }
                    else
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"="<<" , "<<"assign"<<" >"<<endl;
                        outResultToFile("=","assign");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 11: //+状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '+')
                    {
                        state = 0;
                        cout<<"< "<<"++"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("++","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<"+="<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("+=","operator");
                        wordNum["operator"]++;
                        break;            
                    }
                    else
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"+"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("+","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 12: //-状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '-')
                    {
                        state = 0;
                        cout<<"< "<<"--"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("--","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<"-="<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("-=","operator");
                        wordNum["operator"]++;
                        break;  
                    }
                    else
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"-"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("-","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 13: //  *状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<"*="<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("*=","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"*"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("*","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }
                
                case 14: // /状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if(C == '=')
                    {
                        state = 0;
                        cout<<"< "<<"/="<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("/=","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else if(C == '/')  //处理注释 //
                    {
                        state = 18;
                        break;
                    }
                    else if(C == '*')   //处理注释 /*
                    {
                        state = 19;
                        break;
                    }
                    else
                    {
                        forward--;
                        state = 0;
                        cout<<"< "<<"/"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("/","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 15: // !状态
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    state = 0;
                    if(C == '=')
                    {
                        cout<<"< "<<"!="<<" , "<<"relop"<<" >"<<endl;
                        outResultToFile("!=","relop");
                        wordNum["operator"]++;
                        break;
                    }
                    else
                    {
                        forward--;
                        cout<<"< "<<"!"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("!","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 16: // &
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    state = 0;
                    if(C == '&')
                    {
                        cout<<"< "<<"&&"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("&&","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else
                    {
                        forward--;
                        cout<<"< "<<"&"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("&","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 17: //  |
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    state = 0;
                    if(C == '|')
                    {
                        cout<<"< "<<"||"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("||","operator");
                        wordNum["operator"]++;
                        break;
                    }
                    else
                    {
                        forward--;
                        cout<<"< "<<"|"<<" , "<<"operator"<<" >"<<endl;
                        outResultToFile("|","operator");
                        wordNum["operator"]++;
                        break;
                    }
                }

                case 18: // 处理注释//
                {
                    do
                    {
                        C = getChar(lexemebegin,forward,len);
                    } while (C != '\n' && C != EOF);
                    state = 0;
                    wordNum["annotation"]++;
                    break;
                }

                case 19: //  处理注释/*
                {
                    do
                    {
                        C = getChar(lexemebegin,forward,len);
                        if(C == '\n')
                        {
                            fgets(Buffer, buffer_size, fpInput);
                            len = strlen(Buffer);
                            charNum += len;
                            lexemebegin = 0,forward = 0;
                            row++;
                        }
                    }while(C != '*' && C != EOF );
                    if(C == EOF)
                    {
                        cout<<"error! loacted in row:"<<row<<endl;
                        cout<<"The reason: annotation is not align"<<endl;
                        
                        break;
                    }
                    else
                    {
                        C = getChar(lexemebegin,forward,len);
                        if(C == '/')
                        {
                            state = 0;
                            wordNum["annotation"]++;
                            break; //
                        }
                        else
                        {
                            state = 19;
                            break;
                        }
                    }
                }

                case 20:   //处理0
                {
                    token = cat(token,C);
                    C = getChar(lexemebegin,forward,len);
                    if((C == 'x' || C == 'X') && token.length() == 1)
                    {
                        state = 21;
                        break;
                    }
                    
                    else if(isOct(C))
                    {
                        state = 20;
                        break;
                    }
                    else
                    {
                        if(token.length() == 1)   //识别为十进制的0
                        {
                            if(C == '.')
                            {
                                state = 3;
                                break;
                            }
                            else
                            {
                                forward--;
                                cout<<"< "<<"num"<<" , "<<"0"<<" >"<<endl;
                                outResultToFile("num","0");
                                wordNum["num"]++;
                                state = 0;
                                break;
                            }
                            
                        }
                        else
                        {
                            if(C > '7' && C <= '9' || C >= 'a' && C <= 'f' || C >= 'A' && C <= 'F')
                            {
                                cout<<"error! loacted in row:"<<row<<endl;
                                cout<<"The reason: Octal number is wrong"<<endl;
                                addressError(lexemebegin,forward,len);
                                state = 0;
                                break;
                            }
                            else
                            {
                                forward--;
                                cout<<"< "<<"num"<<" , "<<token<<" >"<<endl;
                                outResultToFile("num",token);
                                wordNum["num"]++;
                                state = 0;
                                break;
                            }
                        }
                    }
                }
                case 21: //处理十六进制数
                {
                    token = cat(token,C);
                    C =getChar(lexemebegin,forward,len);
                    if(isHex(C))
                    {
                        state = 21;
                        break;
                    }
                    else
                    {
                        if((C > 'f' && C <= 'z') || (C > 'F' && C <= 'Z'))
                        {
                            cout<<"error! loacted in row:"<<row<<endl;
                            cout<<"The reason: Hexadecimal number is wrong"<<endl;
                            addressError(lexemebegin,forward,len);
                            state = 0;
                            break;
                        }
                        else
                        {
                            forward--;
                            cout<<"< "<<"num"<<" , "<<token<<" >"<<endl;
                            outResultToFile("num",token);
                            wordNum["num"]++;
                            state = 0;
                            break;
                        }
                        
                    }
                }


            }
        }
    
    }
    
    cout<<"字符总数为："<<charNum<<endl;
    cout<<"代码总共有："<<row<<"行"<<endl;
    cout<<"各类单词总数为："<<endl;
    for(iter_si=wordNum.begin();iter_si!=wordNum.end();iter_si++)
        cout<<iter_si->first<<": "<<iter_si->second<<endl;
    fclose(fpInput);
    fPrint.close();
    
}