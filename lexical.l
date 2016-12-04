%{
	#include "syntax.tab.h"
	extern int flag;
	int yycolumn=1;
	#define YY_USER_ACTION\
	yylloc.first_line = yylloc.last_line = yylineno;\
	yylloc.first_column = yycolumn;\
	yylloc.last_column = yycolumn + yyleng;\
	yycolumn += yyleng;
%}
%option yylineno
TYPE int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
PLUS \+
MINUS -
INT [1-9]+[0-9]*|0[0-9]+|0[Xx][a-zA-Z0-9]+|0
FLOAT   [0-9]+\.[0-9]*|[0-9]+\.[0-9]*[eE][+-][1-9]+[0-9]*
ID [a-z_A-Z][a-zA-Z_0-9]*
SPACE [\t]*
EOL \n
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
%%
{TYPE}		{yylval.p=new_ast("TYPE",0);return TYPE;}
{STRUCT}	{yylval.p=new_ast("STRUCT",0);return STRUCT;}
{RETURN}	{yylval.p=new_ast("RETURN",0);return RETURN;}
{IF}		{yylval.p=new_ast("IF",0);return IF;}
{ELSE}		{yylval.p=new_ast("ELSE",0);return ELSE;}
{WHILE}		{yylval.p=new_ast("WHILE",0);return WHILE;}
{PLUS}		{yylval.p=new_ast("PLUS",0);return PLUS;}
{MINUS}		{yylval.p=new_ast("MINUS",0);return MINUS;}
{INT}		{
				yylval.p=new_ast("INT",0);
				char *c=yytext;
				if(*c=='0'){
					if(yyleng==1) return INT;
					int count=yyleng;
					c+=1;
					if(*c=='x'||*c=='X'){
						c+=1;
						count-=2;
						while(count--){
							if(*c>='g'&&*c<='z'||*c>='G'&&*c<='Z')
							{
								printf("Syntax error at Line %d\n",yylineno);flag=1;
								flag=1;
								return INT;
							}
							else c+=1;
						}
					}
					else{
						count-=1;
						while(count--){
							if(*c=='8'||*c=='9')
							{
								printf("Syntax error at Line %d\n",yylineno);flag=1;
								flag=1;
								return INT;
							}
							else c+=1;
						}
					}
				}
				return INT;
			}
{FLOAT}		{
				
				yylval.p=new_ast("FLOAT",0);return FLOAT;
			}
{ID}		{yylval.p=new_ast("ID",0);return ID;}
{SPACE}		{/*skip space*/yycolumn+=3;}
" "			{}
{EOL}		{yycolumn = 1;}
{SEMI}		{yylval.p=new_ast("SEMI",0);return SEMI;}
{COMMA}		{yylval.p=new_ast("COMMA",0);return COMMA;}
{ASSIGNOP}	{yylval.p=new_ast("ASSIGNOP",0);return ASSIGNOP;}
{RELOP}		{yylval.p=new_ast("RELOP",0);return RELOP;}
{STAR}		{yylval.p=new_ast("STAR",0);return STAR;}
{DIV}		{yylval.p=new_ast("DIV",0);return DIV;}
{AND}		{yylval.p=new_ast("AND",0);return AND;}
{OR}		{yylval.p=new_ast("OR",0);return OR;}
{DOT}		{yylval.p=new_ast("DOT",0);return DOT;}
{NOT}		{yylval.p=new_ast("NOT",0);return NOT;}
{LP}		{yylval.p=new_ast("LP",0);return LP;}
{RP}		{yylval.p=new_ast("RP",0);return RP;}
{LB}		{yylval.p=new_ast("LB",0);return LB;}
{RB}		{yylval.p=new_ast("RB",0);return RB;}
{LC}		{yylval.p=new_ast("LC",0);return LC;}
{RC}		{yylval.p=new_ast("RC",0);return RC;}
"//".*		{/*single line comment*/}
"/*"		{
				char c=input();
				while(1){
					if(c=='*'&&(c=input())=='/')
						break;
					else c=input();
				}
			}
"*/".*		{printf("Syntax error at Line %d\n",yylineno);flag=1;}
.			{printf("Syntax error at Line %d : Mysterious character \'%s\'\n",yylineno,yytext);flag=1;}
%%