#ifndef GRAMTREE_H
#define GRAMTREE_H

extern int yylineno;
extern char *yytext;
struct ast
{
    int line;
    int type;//1:int var,2:float var,3:array,4:c_int,5:c_float,6:fun,7:struct
    char *name;
    struct ast *l;
    struct ast *r;
    union
    {
        char *id;
        int a;
        float b;
    };
};

struct ast *new_ast(char *name, int num,...);
void print_tree(struct ast * p, int level);

#endif