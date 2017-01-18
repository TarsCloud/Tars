/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

%option outfile="tars.lex.cpp"
%option   yylineno
%{
#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <errno.h>
#include <math.h>

#define YYSTYPE GrammarBasePtr

#include "parse.h"
#include "tars.tab.hpp"

using namespace std;

extern "C"
{
    int yywrap()
    {
        return 1;
    }
}

struct include_file_state
{
    YY_BUFFER_STATE state;
    string file;
};

#define MAX_INCLUDE_DEPTH 200
include_file_state include_file_stack[MAX_INCLUDE_DEPTH];
int include_file_stack_ptr = 0;

%}

tars_identifiers         [[:alpha:]_][[:alnum:]_]*
tars_anychar             .
tars_const_int           (\+|-)?((0[0-7]+)|(0x[[:xdigit:]]+)|([[:digit:]]+))
tars_float_literal       (\+|-)?(([[:digit:]]*\.[[:digit:]]+)|([[:digit:]]+\.))
tars_float_exp           (e|E)(\+|-)?[[:digit:]]+
tars_const_float         (({tars_float_literal}{tars_float_exp}?)|((\+|-)?[[:digit:]]+{tars_float_exp}))[fF]?

%x INCL

%%

"#include"[ \t]*   { BEGIN(INCL); }

<INCL>"\"".*"\""   {
    if ( include_file_stack_ptr >= MAX_INCLUDE_DEPTH )
    {
        g_parse->error("Includes nested too deeply" );
    }

    string file;
    bool b = g_parse->getFilePath( yytext, file);
    g_parse->currentContextPtr()->addInclude(file);

    //该文件没解析过
    if(b)
    {
        include_file_stack[include_file_stack_ptr].state = YY_CURRENT_BUFFER;
        include_file_stack[include_file_stack_ptr].file  = file;
        include_file_stack_ptr++;

        yyin = fopen( file.c_str(), "r" );
        if ( !yyin )
        {
            g_parse->error("can't open file:" + file);
        }

        yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );

        g_parse->pushFile(file);
    }
    BEGIN(INITIAL);
}

<<EOF>> {
    --include_file_stack_ptr;
    if ( include_file_stack_ptr < 0 )
    {
        include_file_stack_ptr = 0;
        yyterminate();
    }
    else
    {
        yy_delete_buffer( YY_CURRENT_BUFFER );
        fclose(yyin);
        yy_switch_to_buffer(include_file_stack[include_file_stack_ptr].state );
        g_parse->popFile();
    }
}

"::" {
    return TARS_SCOPE_DELIMITER;
}

"//" {
    // C++的注释风格
    bool end = false;
    while(!end)
    {
        int input = yyinput();
        if(input == '\n')
    	{
    	    g_parse->nextLine();
    	}
    	
    	if(input == '\n' || input == EOF)
    	{
    	    end = true;
    	}
    }
}

"/*" {
    // C的注释风格
    bool end = false;
    string comment = yytext + 2;
    while(!end)
    {
        int input = yyinput();
        switch(input)
        {
            case '*':
            {
                int nextInput = yyinput();
                if(nextInput == '/')
                {
                    end = true;
                }
                else
                {
                    comment += static_cast<char>(input);
                    unput(nextInput);
                }
                break;
            }
            case '\n':
            {
                comment += static_cast<char>(input);
                g_parse->nextLine();
                break;
            }
            case EOF:
            {
                //到文件尾部, 但是没有出现*/
                g_parse->error("EOF in comment");
                end = true; 
                break;
            }
            default:
            {
                comment += static_cast<char>(input);
                break;
            }
        }
    }
    if(comment[0] == '*')
    {
        //注释内容暂时未处理
    }
}

{tars_identifiers}    {
    StringGrammarPtr ident  = new StringGrammar;
    ident->v            = yytext;
    yylval              = ident;
    return g_parse->checkKeyword(ident->v);
}

{tars_identifiers}[[:space:]]*"(" {
    StringGrammarPtr ident  = new StringGrammar;
    ident->v            = yytext;
    ident->v.erase(ident->v.find_first_of(" \t\v\n\r\f("));

    yylval = ident;

    return TARS_OP;
}

\"  {
    StringGrammarPtr str = new StringGrammar;
    bool end = false;
    while(!end)
    {
    	int input = yyinput();
    	switch(input)
    	{
    	    case '"':
    	    {
    	        end = true;
    	        break;
    	    }
    	    case '\n':
    	    {
    	        g_parse->error("newline in string");
    	        break;
    	    }
    	    case EOF:
    	    {
    	        g_parse->error("EOF in string");
    	        break;
    	    }
    	    case '\\':
    	    {
    	        static string specialChars = "nrtvfab?";
    	        static string octalChars = "0123";
    	        
    	        char nextInput = static_cast<char>(yyinput());
    	        if(nextInput == '\\' || nextInput == '"' || nextInput == '\'')
    	        {
    	            str->v += nextInput;
    	        }
    	        else if(specialChars.find(nextInput) != string::npos)
                {
                    str->v += '\\';
                    str->v += nextInput;
                }
                else if(octalChars.find(nextInput) != string::npos)
                {
                    static string octalDigits = "01234567";
                    
                    unsigned short us = nextInput - '0';
                    if(octalDigits.find_first_of(nextInput = static_cast<char>(yyinput())) != string::npos)
                    {
                        us = us * 8 + nextInput - '0';
                        if(octalDigits.find_first_of(nextInput = static_cast<char>(yyinput())) != string::npos)
                        {
                            us = us * 8 + nextInput - '0';
                        }
                        else
                        {
                            unput(nextInput);
                        }
                    }
                    else
                    {
                        unput(nextInput);
                    }

                    if(us == 0)
                    {
                        g_parse->error("illegal NUL character in string constant");
                    }
                    str->v += static_cast<char>(us);
                }
                else if(nextInput == 'x')
                {
                    long long ull = 0;
                    while(isxdigit(nextInput = static_cast<char>(yyinput())))
                    {
                        ull *= 16;
                        if(isdigit(nextInput))
                        {
                            ull += nextInput - '0';
                        }
                        else if(islower(nextInput))
                        {
                            ull += nextInput - 'a' + 10;
                        }
                        else
                        {
                            ull += nextInput - 'A' + 10;
                        }
                    }

                    unput(nextInput);

                    if(ull == 0)
                    {
                        g_parse->error("illegal NUL character in string constant");
                    }
                    str->v += static_cast<char>(ull);
                }
                else
                {
                    str->v += static_cast<char>(input);
                }

                break;
    	    }
    	    default:
    	    {
    	        str->v += static_cast<char>(input);
    	        break;
    	    }
    	}
    }
    yylval = str;
    return TARS_STRING_LITERAL;
}

{tars_const_int} {
    errno = 0;
    IntergerGrammarPtr ptr = new IntergerGrammar;
    yylval = ptr;
    
    string value = yytext;
    const char* beg = value.c_str();
    char* end = 0;

    ptr->v = strtoll(beg, &end, 0);
    if(!(errno == 0 && beg != end))
    {
        assert(ptr->v != 0);
        string err = "integer constant `";
        err += value;
        err += "' out of range";
        g_parse->error(err);
    }
    
    return TARS_CONST_INTEGER;
}

{tars_const_float} {
    errno = 0;
    FloatGrammarPtr ptr = new FloatGrammar;
    yylval = ptr;
    
    string value(yytext);
    
    char lastChar = value[value.size() - 1];
    if(lastChar == 'f' || lastChar == 'F')
    {
    	value = value.substr(0, value.size() - 1);
    }
    
    ptr->v = strtod(value.c_str(), 0);
    if((errno == ERANGE) && (ptr->v == HUGE_VAL || ptr->v == -HUGE_VAL))
    {
        string err = "float point constant `";
        err += value;
        err += "' too large (overflow)";
        g_parse->error(err);
    }
    else if(errno == ERANGE && ptr->v == 0)
    {
        string err = "float point constant `";
        err += value;
        err += "' too small (underflow)";
        g_parse->error(err);
    }
    return TARS_CONST_FLOAT;
}

[[:space:]]     {
    if(yytext[0] == '\n')
    {
        g_parse->nextLine();
    }
}

{tars_anychar}       {
    if(yytext[0] < 32 || yytext[0] > 126)
    {
        stringstream s;
        s << "illegal input character: '\\";
        s.width(3);
        s.fill('0');
        s << oct << static_cast<int>(static_cast<unsigned char>(yytext[0]));
        s << "'";
        
        g_parse->error(s.str());
        return BAD_CHAR;
    }
    return yytext[0];
}

%%

