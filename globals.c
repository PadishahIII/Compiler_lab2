#include "tnode.h"

int nodeNum;
tnode nodeList[LISTSIZE];
int IsChild[LISTSIZE];
int va_num;
char *va_type[10];
char *rtype[10];
int rnum;
int instruct; //当前是否是结构体的域
int LCnum;    //花括号是否闭合
int structno; //当前是第几个结构体
var *varhead, *vartail;
func *funchead, *functail;
array *arrayhead, *arraytail;
struct_ *structhead, *structtail;
// fieldlist structfield[100];
// char **structname[100];
// int structnum;
fieldlist fieldtail;
struct_ *currentstruct;
