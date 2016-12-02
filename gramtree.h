#ifndef GRAMTREE_H
#define GRAMTREE_H

#include <stdarg.h>
#include <string.h>
extern int yylineno;
extern char *yytext;
struct ast
{
    int line;
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
void print_tree(struct ast *, int level);

#endif