#ifndef _SEMANTEME_H
#define _SEMANTEME_H
#include "gramtree.h"
/*enum type
{
    1:int,
    2:float,
    3:array,
    4:const int,
    5:const float,
    6:fun,
    7:struct,
    8:ID
};*/
struct var_type
{
    char *name;
    int type;
    char *struct_name;
    struct var_type *var_next;
};

struct struct_type
{
    char *name;
    struct var_type *var;
    struct struct_type *struct_next;
};

struct dimen{
    int i;
    struct dimen *next;
};
struct array_type
{
    char *name;
    int type;
    char *struct_name;
    struct dimen *d;
    struct array_type *array_next;
};

struct fun_type
{
    char *name;
    int isdef;
    int return_type;
    struct var_type *para;
    struct fun_type *fun_next;
};
struct ast *search_name(struct ast *p, char *name);
void add_sym_type(struct ast *p, char *name, int type, char *struct_name);
void set_sym_type(struct ast *p, char *name, int type,char *struct_name);
void set_node_type(struct ast *p,char* name, int type);
void add_var(char *name, int type,char *struct_name,int line);
void add_digui_var(struct ast *p, int type, char *struct_name);
int exist_var(char *name);
int type_var(char *name);
void set_var_type(char *name, int type);
void del_var(char *name);
void add_arr(char *name, int type, int dimen,char *struct_name);
void del_arr(char *name);
int exist_arr(char *name);
void set_arr_type(char *name, int type,char *struct_name);
int type_arr(char *name);
void add_struct_var(struct ast *p, struct var_type **var, int type);
void add_struct(char *name, struct ast *var);
int exist_struct(char *name);
#endif