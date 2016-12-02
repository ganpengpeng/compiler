#include "semanteme.h"
#include "gramtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct var_type * var_type_p;
typedef struct struct_type * struct_type_p;
typedef struct array_type * array_type_p;
typedef struct fun_type * fun_type_p;
var_type_p var_p = NULL;
struct_type_p struct_p = NULL;
array_type_p array_p = NULL;
fun_type_p fun_p = NULL;
int search_name(struct ast* p,char *name)
{
    if(!p)
        return 0;
    if(!strcmp(p->name,name)){
        return 1;
    }
    int i=search_name(p->l, name);
    int j=search_name(p->r, name);
    if(i||j)
        return 1;
    return 0;
}
void set_sym_type(struct ast* p,char *name,int type)
{
    if(!p)
        return;
    if(!strcmp(p->name,name)){
        set_var_type(p->id, type);
        set_arr_type(p->id, type);
    }
    set_name_type(p->l, name,type);
    set_name_type(p->r, name,type);
}
void set_node_type(struct ast* p,char *name,int type)
{
    if(!p)
        return;
    if(!strcmp(p->name,name)){
        p->type = type;
    }
    set_name_type(p->l, name,type);
    set_name_type(p->r, name,type);
}
void add_var(char *name, int type)
{
    var_type_p temp = (var_type_p)malloc(sizeof(struct var_type));
    temp->name = name;
    temp->type = type;
    temp->var_next = var_p;
    var_p = temp;
}
int exist_var(char *name)
{
    for (var_type_p p = var_p; p; p = p->var_next)
    {
        if (!strcmp(name, p->name))
            return 1;
    }
    return 0;
}
int type_var(char *name)
{
    for (var_type_p p = var_p; p; p = p->var_next)
    {
        if (!strcmp(name, p->name))
            return p->type;
    }
    return 0;
}
void set_var_type(char *name,int type)
{
    for (var_type_p p = var_p; p; p = p->var_next)
    {
        if (!strcmp(name, p->name))
            p->type = type;
    }
}
void del_var(char *name)
{
    if(!var_p)
        return;
    if (!strcmp(name, var_p->name))
    {
        var_type_p temp = var_p;
        var_p = var_p->var_next;
        free(temp);
        return;
    }
    for (var_type_p p = var_p; p->var_next; p = p->var_next)
    {
        if (!strcmp(name, p->var_next->name)){
            var_type_p temp = p->var_next;
            p->var_next = p->var_next->var_next;
            free(temp);
        }
    }
    return;
}

void add_arr(char *name,int type,int dimen)
{
    array_type_p temp = array_p;
    for (; temp; temp = temp->array_next)
    {
        if (!strcmp(temp->name, name))
        {
            struct dimen *d = temp->d;
            while (d->next != 0)
                d = d->next;
            d->next = (struct dimen *)malloc(sizeof(struct dimen));
            d->next->i = dimen;
            d->next->next = 0;
            return;
        }
    }
    temp = (array_type_p)malloc(sizeof(struct array_type));
    temp->name = name;
    temp->type = type;
    temp->array_next = array_p;
    array_p = temp;
    temp->d = (struct dimen *)malloc(sizeof(struct dimen));
    temp->d->i = dimen;
    temp->d->next = 0;
}
int exist_arr(char *name)
{
    array_type_p temp = array_p;
    for (; temp; temp = temp->array_next)
    {
        if (!strcmp(temp->name, name))
        {
            return 1;
        }
    }
    return 0;
}
void set_arr_type(char *name,int type)
{
    for (array_type_p p = array_p; p; p = p->array_next)
    {
        if (!strcmp(name, p->name))
            p->type = type;
    }
}
int type_arr(char *name)
{
    array_type_p temp = array_p;
    for (; temp; temp = temp->array_next)
    {
        if (!strcmp(temp->name, name))
        {
            return temp->type;
        }
    }
    return 0;
}

void add_struct(char *name,struct var_type *var)
{
    struct_type_p temp = (struct_type_p)malloc(sizeof(struct struct_type));
    temp->name = name;
    temp->var = var;
    temp->struct_next = struct_p;
    struct_p = temp;
}
int exist_struct(char *name)
{
    struct_type_p temp = struct_p;
    for (; temp; temp = temp->struct_next)
    {
        if (!strcmp(temp->name, name))
        {
            return 1;
        }
    }
    return 0;
}