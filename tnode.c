#include "tnode.h"
#include <stdio.h>

Tree newTree(char *type, int num, int numOfChild, ...)
{
    tnode rootNode = (tnode)malloc(sizeof(struct Tnode));
    tnode child;
    if (!rootNode)
    {
        yyerror("malloc tnode error");
        exit(-1);
    }
    rootNode->type = type;
    rootNode->num = num;
    rootNode->leftchild = rootNode->next = NULL;

    va_list param_list;
    va_start(param_list, numOfChild); //初始化参 数列表

    if (numOfChild > 0) //非终结符 union字段为空
    {
        child = va_arg(param_list, tnode);
        if (child->num >= 0)
            IsChild[child->num] = 1;
        rootNode->lineno = child->lineno;
        rootNode->leftchild = child;

        rootNode->content = child->content; //传递第一个孩子节点的语义值
        rootNode->tag = child->tag;
        if (numOfChild >= 2)
        {
            for (int i = 0; i < numOfChild - 1; i++)
            {
                child->next = va_arg(param_list, tnode);
                child = child->next;
                if (child->num >= 0)
                    IsChild[child->num] = 1;
            }
        }
    }
    else
    { //终结符 或 空的语法单元
        rootNode->lineno = va_arg(param_list, int);
        if (!strcmp(type, "INT"))
        {
            rootNode->numtype = "int";
            rootNode->intval = atoi(yytext);
        }
        else if (!strcmp(type, "FLOAT"))
        {
            rootNode->numtype = "float";
            // rootNode->content = yytext;
            rootNode->fltval = atof(yytext);
        }
        else
        {
            char *str = (char *)malloc(strlen(yytext) + 2);
            memset(str, 0, strlen(yytext) + 2);
            strncpy(str, yytext, strlen(yytext) + 1);
            rootNode->content = str;
        }
    }
    return rootNode;
}

void Preorder(Tree node, int level)
{
    if (node != NULL)
    {
        for (int i = 0; i < level; i++)
        {
            printf("  ");
        }
        if (node->lineno >= 0) //空语法单元的lineno为-1
        {
            printf("%s", node->type);
            if ((!strcmp(node->type, "ID")) || (!strcmp(node->type, "TYPE")))
            {
                printf(": %s", node->ID_TYPE);
            }
            else if (!strcmp(node->type, "INT"))
            {
                printf(": %d", node->intval);
            }
            else if (!strcmp(node->type, "FLOAT"))
            {
                printf(": %f", node->fltval);
            }
            else
            {
                printf(" (%d)", node->lineno);
            }
        }
        printf("\n");
        Preorder(node->leftchild, level + 1);
        Preorder(node->next, level);
    }
}

/**********************变量符号表操作******************/
void newvar(int num, ...)
{
    va_list valist;
    va_start(valist, num);
    var *res = (var *)malloc(sizeof(var));
    tnode temp = (tnode)malloc(sizeof(tnode));
    if (instruct && LCnum)
    {
        //在结构体中且未闭合
        res->instruct = 1;
        res->structno = structno;
    }
    else
    {
        res->instruct = 0;
        res->structno = -1; // TODO:
    }
    temp = va_arg(valist, tnode);
    res->type = temp->content;
    temp = va_arg(valist, tnode);
    res->name = temp->content;

    vartail->next = res;
    vartail = res;
}
int findvar(tnode val)
{
    var *temp = varhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            if (instruct && LCnum)
            {
                if (!temp->instruct)
                {
                    printf("Error type X at line %d:Struct field and Variable use the same name.\n", yylineno);
                }
                else if (temp->instruct && temp->structno != structno)
                {
                    printf("Error type Y at line %d:Different struct use the same field name\n", yylineno);
                }
                else
                {
                    return 1; //同一结构体域名重复
                }
            }
            else //全局变量
            {
                if (temp->instruct)
                {
                    printf("Error type X at line %d:Struct field and Variable use the same name.\n ", yylineno);
                }
                else
                {
                    return 1; //变量重复定义
                }
            } // if(instruct)
        }     // if(!strcmp)
        temp = temp->next;
    } // while
    return 0;
}
char *typevar(tnode val)
{
    var *temp = varhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}
int checkleft(tnode val)
{
    tnode temp = val->leftchild;
    if (temp == NULL)
    {
        return 0; //失败
    }
    tnode firstchild = val->leftchild;
    int childnum = 0;
    while (temp != NULL)
    {
        childnum++;
        temp = temp->next;
    }
    if (childnum == 1 && !strcmp(firstchild->type, "ID"))
    {
        return 1;
    }
    else if (childnum == 4 && !strcmp(firstchild->type, "Exp") && !strcmp(firstchild->next->type, "LB") && !strcmp(firstchild->next->next->type, "Exp") && !strcmp(firstchild->next->next->next->type, "RB"))
    {
        return 1;
    }
    else if (childnum == 3 && !strcmp(firstchild->type, "Exp") && !strcmp(firstchild->next->type, "DOT") && !strcmp(firstchild->next->next->type, "ID"))
    {
        return 1;
    }
    else
        return 0;
}

/***************函数符号表******************/
void newfunc(int num, ...)
{
    int i;
    va_list valist;
    va_start(valist, num);
    tnode temp = (tnode)malloc(sizeof(struct Tnode));
    switch (num)
    {
    case 1:
        if (instruct && LCnum)
        {
            functail->instruct = 1;
            functail->structno = structno;
        }
        else
        {
            functail->instruct = 0;
            functail->structno = 0;
        }
        temp = va_arg(valist, tnode);
        functail->rtype = temp->content; //返回值类型
        functail->type = temp->numtype;  // 语法单元的类型
        for (i = 0; i < rnum; i++)
        {
            if (rtype[i] == NULL || strcmp(rtype[i], functail->rtype))
            {
                printf("Error type 12 at line %d:Function return type error(%s->%s)\n", yylineno, functail->rtype, rtype[i]);
            }
        }
        functail->tag = 1;
        func *new = (func *)malloc(sizeof(func));
        functail->next = new;
        functail = new;
        break;
    case 2:
        temp = va_arg(valist, tnode);
        functail->name = temp->content; //函数名
        temp = va_arg(valist, tnode);
        functail->va_num = 0;
        getdeftype(temp);
        break;
    default:
        break;
    }
}
void getdeftype(tnode val)
{
    if (val != NULL)
    {
        if (!strcmp(val->type, "ParamDec"))
        {
            functail->va_type[functail->va_num] = val->leftchild->content;
            functail->va_num++;
            return;
        }
        tnode child = val->leftchild;
        while (child != NULL)
        {
            getdeftype(child);
            child = child->next;
        }
    }
    return;
}
void getreatype(tnode val)
{
    if (val != NULL)
    {
        if (!strcmp(val->type, "Exp"))
        {
            va_type[va_num] = val->numtype;
            va_num++;
            return;
        }
        tnode child = val->leftchild;
        while (child != NULL)
        {
            getreatype(child);
            child = child->next;
        }
    }
}
void getrtype(tnode val) //实际返回值类型
{
    rtype[rnum] = val->numtype;
    rnum++;
}
int checkrtype(tnode ID, tnode Args)
{
    va_num = 0;
    getreatype(Args);
    func *temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1)
    {
        if (!strcmp(temp->name, ID->content))
        {
            break;
        }
        temp = temp->next;
    }
    if (va_num != temp->va_num)
    {
        return 1;
    }
    for (int i = 0; i < temp->va_num; i++)
    {
        if (temp->va_type[i] == NULL || va_type[i] == NULL || strcmp(temp->va_type[i], va_type[i]) != 0)
        {
            return 1;
        }
    }
    return 0; //成功
}
int findfunc(tnode val)
{ //函数是否已定义
    func *temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1)
    {
        if (!strcmp(temp->name, val->content))
        {
            if (instruct && LCnum)
            {
                if (!temp->instruct)
                {
                    printf("Error type X at line %d:Struct field and Variable use the same name.\n", yylineno);
                }
                else if (temp->instruct && temp->structno != structno)
                {
                    printf("Error type Y at line %d:Different struct use the same field name\n", yylineno);
                }
                else
                {
                    //同一结构体中域名重复
                    printf("Error type 15 at line %d:Redefined field", yylineno); // TODO:
                    return 1;
                } // if(!temp->instruct)
            }     // if(instruct)
            else
            {
                if (temp->instruct)
                {
                    printf("Error type X at line %d:Struct field and Variable use the same name.\n", yylineno);
                }
                else
                {
                    return 1; // TODO:
                }
            } // if(instruct)
        }     // if(!strcmp)
        temp = temp->next;
    } // while
    return 0;
}
char *typefunc(tnode val)
{
    func *temp = funchead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            char *args = (char *)malloc(sizeof(char) * 100);
            memset(args, 0, sizeof(char) * 100);
            int i = 0;
            char *pi = args;
            while (i < temp->va_num && pi < args + 99)
            {
                memcpy(pi, temp->va_type[i], strlen(temp->va_type));
                pi += strlen(temp->va_type[i]);
                memcpy(pi, ",", 1);
                pi += 1;
                i++;
            }
            *(pi - 1) = 0;
            return args; //返回:"arg1,arg2,arg3"
        }
        temp = temp->next;
    }
    return NULL;
}
int numfunc(tnode val)
{
    func *temp = funchead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            return temp->va_num;
        }
        temp = temp->next;
    }
    return 0;
}
//检查函数是否重定义
int checkmultifuncstr(char *str)
{
    func *pos = funchead->next;
    while (pos != NULL)
    {
        if (!strcmp(pos->name, str))
        {
            return 0; //已定义
        }
        pos = pos->next;
    }
    return 1;
}

/************************数组符号表*******************/
void newarray(int num, ...)
{
    va_list valist;
    va_start(valist, num);
    array *res = (array *)malloc(sizeof(array));
    tnode temp = (tnode)malloc(sizeof(struct Tnode));
    if (instruct && LCnum)
    {
        res->instruct = 1;
        res->structno = structno;
    }
    else
    {
        res->instruct = 0;
        res->structno = 0; // TODO:
    }
    temp = va_arg(valist, tnode);
    res->type = temp->content;
    temp = va_arg(valist, tnode);
    res->name = temp->content;
    arraytail->next = res;
    arraytail = res;
}
int findarray(tnode val)
{
    array *temp = arrayhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
char *typearray(tnode val)
{
    array *temp = arrayhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}
/******************结构体符号表***************/
void newstruct(int num, ...)
{
    va_list valist;
    va_start(valist, num);
    struct_ *res = (struct_ *)malloc(sizeof(struct_));
    tnode temp = va_arg(valist, tnode);
    res->name = temp->content;
    structtail->next = res;
    structtail = res;
}
int findstruct(tnode val) //检查结构体是否已定义 content
{
    struct_ *temp = (struct_ *)malloc(sizeof(struct_));
    temp = structhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, val->content))
        {
            return 1;
        }
        temp = temp->next;
    }
    if (findvar(val) == 1) //结构体名称不和变量名冲突
    {
        return 0;
    }
    return 0;
}
int findstructstr(char *str)
{
    if (str == NULL)
        return;
    struct_ *temp = (struct_ *)malloc(sizeof(struct_));
    temp = structhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, str))
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
struct_ *getstructstr(char *str)
{
    if (str == NULL)
        return;
    struct_ *temp = (struct_ *)malloc(sizeof(struct_));
    temp = structhead->next;
    while (temp != NULL)
    {
        if (!strcmp(temp->name, str))
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}
void addstructfield(struct_ *structpos, tnode node)
{
    if (node != NULL)
    {
        if (!strcmp(node->type, "ID"))
        {
            //加入新field
            fieldlist temp = (fieldlist)malloc(sizeof(struct fieldlist_));
            if (structpos->fields == NULL)
            {
                structpos->fields = temp;
                fieldtail = structpos->fields;
            }
            else
            {
                fieldtail->next = temp;
                fieldtail = temp;
            }
            fieldtail->name = node->content;
            fieldtail->next = NULL;
        }
        tnode child = node->leftchild;
        while (child != NULL)
        {
            addstructfield(structpos, child);
            child = child->next;
        }
    }
}
int checkstructfield(struct_ *structpos, char *name)
{ //检查域是否存在
    if (structpos == NULL)
        return 0; //不是结构体
    fieldlist tail = structpos->fields;
    while (tail != NULL)
    {
        if (!strcmp(tail->name, name))
        {
            return 1;
        }
        tail = tail->next;
    }
    return 0;
}
//检查结构体中是否有重复定义的域
char *checkmultifield(struct_ *structpos)
{
    fieldlist tail = structpos->fields;
    while (tail != NULL)
    {
        fieldlist pos = tail->next;
        while (pos != NULL)
        {
            if (!strcmp(tail->name, pos->name))
            {
                return tail->name; //重复定义
            }
            pos = pos->next;
        }
        tail = tail->next;
    }
    return NULL;
}

int Error = 0;
void yyerror(char *msg)
{
    Error = 1;
    fprintf(stderr, "Error Type B at Line %d : %s \'%s\'\n", yylineno, msg, yytext);
}
extern int yydebug;
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

    instruct = 0; // TODO:
    LCnum = 0;
    structno = 0;
    //初始化符号表
    varhead = (var *)malloc(sizeof(var));
    vartail = varhead;
    funchead = (func *)malloc(sizeof(func));
    functail = (func *)malloc(sizeof(func));
    funchead->next = functail;
    functail->va_num = 0;
    arrayhead = (array *)malloc(sizeof(array));
    arraytail = arrayhead;
    structhead = (struct_ *)malloc(sizeof(struct_));
    structtail = structhead;
    rnum = 0;
    // structnum = 0;

    yyrestart(file);
    // yydebug = 1;
    yyparse();
    fclose(file);

    return 0;
}
