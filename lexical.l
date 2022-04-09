%{
    #include "tnode.h"
    #include<stdlib.h>
    #include<stdio.h>
    #include "syntax.tab.h"
    extern int Error;
    //int yycolumn = 1;
    //#define YY_USER_ACTION \
    //    yylineno=yylloc.last_line=yycolumn;\
    //    yylloc.first_column = yycolumn;\
    //    yylloc.last_column =  yycolumn+yyleng-1;\
    //    yycolumn+=yyleng;
%}

%option yylineno

INT 0|[1-9][0-9]{0,31}|0[0-7]{1,32}|0[xX][0-9a-fA-F]{1,32}|0[bB][01]{1,32}
INT_ERROR 0[0-7]*[89]+[0-7]*|0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-fA-F]*|0[bB][01]*[2-9]+[01]*
FLOAT [0-9]+\.[0-9]+|([0-9]+\.[0-9]*|[0-9]*\.[0-9]+)[eE][\+\-]?[0-9]+
ID [_a-zA-Z]+[_a-zA-Z0-9]{0,31}
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE "int"|"float"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

COMMENT ("//".*\n?)|("/*"([*]*(([^*/])+([/])*)*)*"*/")
SPACE [ \f\r\t\v]+

%%

{COMMENT} {}
{SPACE} {}
\n {}
{TYPE} {yylval.type_tree=newTree("TYPE",-1,0,yylineno);return TYPE;}
{STRUCT} {
    yylval.type_tree=newTree("STRUCT",-1,0,yylineno);
    structno++;
    instruct=1;
    return STRUCT;
}
{RETURN} {yylval.type_tree=newTree("RETURN",-1,0,yylineno);return RETURN;}
{IF} {yylval.type_tree=newTree("IF",-1,0,yylineno);return IF;}
{ELSE} {yylval.type_tree=newTree("ELSE",-1,0,yylineno);return ELSE;}
{WHILE} {yylval.type_tree=newTree("WHILE",-1,0,yylineno);return WHILE;}

{INT} {yylval.type_tree=newTree("INT",-1,0,yylineno); return INT;}
{INT_ERROR} {printf("Error Type A at Line %d: Invaild Integer format \'%s\'\n",yylineno,yytext);Error=1;}
{FLOAT} {yylval.type_tree=newTree("FLOAT",-1,0,yylineno);return FLOAT;}

{SEMI} {yylval.type_tree=newTree("SEMI",-1,0,yylineno);return SEMI;}
{COMMA} {yylval.type_tree=newTree("COMMA",-1,0,yylineno);return COMMA;}
{ASSIGNOP} {yylval.type_tree=newTree("ASSIGNOP",-1,0,yylineno);return ASSIGNOP;}
{RELOP} {yylval.type_tree=newTree("RELOP",-1,0,yylineno);return RELOP;}
{PLUS} {yylval.type_tree=newTree("PLUS",-1,0,yylineno);return PLUS;}
{MINUS} {yylval.type_tree=newTree("MINUS",-1,0,yylineno);return MINUS;}
{STAR} {yylval.type_tree=newTree("STAR",-1,0,yylineno);return STAR;}
{DIV} {yylval.type_tree=newTree("DIV",-1,0,yylineno);return DIV;}
{AND} {yylval.type_tree=newTree("AND",-1,0,yylineno);return AND;}
{OR} {yylval.type_tree=newTree("OR",-1,0,yylineno);return OR;}
{DOT} {yylval.type_tree=newTree("DOT",-1,0,yylineno);return DOT;}
{NOT} {yylval.type_tree=newTree("NOT",-1,0,yylineno);return NOT;}
{LP} {yylval.type_tree=newTree("LP",-1,0,yylineno);return LP;}
{RP} {yylval.type_tree=newTree("RP",-1,0,yylineno);return RP;}
{LB} {yylval.type_tree=newTree("LB",-1,0,yylineno);return LB;}
{RB} {yylval.type_tree=newTree("RB",-1,0,yylineno);return RB;}
{LC} {
    yylval.type_tree=newTree("LC",-1,0,yylineno);
    if(instruct){
        LCnum++;
    }
    //TODO:
    return LC;
}
{RC} {
    yylval.type_tree=newTree("RC",-1,0,yylineno);
    if(instruct){
        LCnum--;
    }
    return RC;
}

{ID} {yylval.type_tree=newTree("ID",-1,0,yylineno);return ID;}

. {printf("Error Type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);Error=1;}
%%

int yywrap(){
    return 1;
}

