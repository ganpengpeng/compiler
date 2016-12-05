#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "gramtree.h"

struct ast *new_ast(char *name, int num,...)
{
    va_list valist;
    struct ast *root = (struct ast *)malloc(sizeof(struct ast));
    struct ast *p;
    if (root == 0)
    {
        printf("malloc fail!!!");
        system("pause");
        return root;
    }
    root->name = name;
    va_start(valist, num);
    if (num >= 1)//nonterminal token
    {
        p = va_arg(valist, struct ast *);
        root->line = p->line;
        root->l = p;
        root->type = p->type;
        if(root->type==4)
            root->a = p->a;
        else if(root->type==5)
            root->b = p->b;
        else
            root->id = p->id;
        while (num > 1)
        {
            p->r = va_arg(valist, struct ast *);
            p = p->r;
            num -= 1;
        }
    }
    else if (num != -1)
    {
        root->line = yylineno;
        if (!strcmp(name, "ID")||!strcmp(name, "TYPE"))
        {
            root->id = (char *)malloc(sizeof(char) * 50);
            strcpy(root->id, yytext);
            if (!strcmp(root->id, "int"))
                root->type = 1;
            else if (!strcmp(root->id, "float"))
                root->type = 2;
            else
                root->type = 0;
            root->l = root->r = 0;
        }
        else if (!strcmp(name, "INT"))
        {
            root->a = strtol(yytext,NULL,0);
            root->type = 4;
            root->l = root->r = 0;
        }
        else if (!strcmp(name, "FLOAT"))
        {
            root->b = atof(yytext);
            root->type = 5;
            root->l = root->r = 0;
        }
    }
    else
        root->line = -1;//void production statment
    return root;
}
void print_tree(struct ast *root, int level)
{
    int i;
    if (root != NULL)
    {
        if(root->line!=-1)
            for(i=0; i<level; ++i)
                printf("  ");
        if(root->line!=-1){ 
            printf("%s",root->name);
            if((!strcmp(root->name,"ID"))||(!strcmp(root->name,"TYPE")))printf(": %s",root->id);
            else if(!strcmp(root->name,"INT"))printf(": %d",root->a);
            else if(!strcmp(root->name,"FLOAT"))printf(": %f",root->b);
            else if (root->l != 0)
                printf("(%d)",root->line);
        }
        if(root->line!=-1) printf("\n");
        print_tree(root->l, level + 1);
        print_tree(root->r,level);
    }
}