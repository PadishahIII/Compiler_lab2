#include <stdio.h>
#include "syntax.tab.h"
//#include "lex.yy.c"
#include "tnode.h"

int Error = 0;
int main(int argc, char **argv)
{
    if (argc <= 1)
        return 0;
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror(argv[1]);
        return 1;
    }
    nodeNum = 0;
    memset(nodeList, NULL, sizeof(tnode) * LISTSIZE);
    memset(IsChild, 0, sizeof(int) * LISTSIZE);
    Error = 0;

    yyrestart(file);
    // yydebug = 1;
    yyparse();
    fclose(file);

    if (!Error)
        return 0;
    for (int i = 0; i < nodeNum; i++)
    {
        if (IsChild[i] != 1)
        {
            Preorder(nodeList[i], 0);
        }
    }
    return 0;
}

yyerror(char *msg)
{
    Error = 1;
    fprintf(stderr, "Error Type B at Line %d,%d : %s \'%s\'\n", yylloc.first_line, yylloc.first_column, msg, yytext);
}

//[0-9]+\.[0-9]+ | ([0-9]+\.[0-9]*|[0-9]*\.[0-9]+)[eE][\+\-]?[0-9]+
// INT 0 | [1-9][0-9]{1,31} | 0[0-7]{1,32} | 0[xX][0-9a-fA-F]{1,32}
