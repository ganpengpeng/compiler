#ifndef _SEMANTEME_H
#define _SEMANTEME_H
#include "gramtree.h"
/*enum type
{
    1:int,
    2:float,
    3:array,
    4:c_int,
    5:c_float,
    6:fun,
    7:struct,
    8:ID
};*/
struct var_type
{
    char *name;
    int type;
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
void set_sym_type(struct ast *p,char* name, int type);
void set_node_type(struct ast *p,char* name, int type);
void add_var(char *name, int type);
int exist_var(char *name);
int type_var(char *name);
void set_var_type(char *name, int type);
void del_var(char *name);
void add_arr(char *name, int type, int dimen);
int exist_arr(char *name);
void set_arr_type(char *name, int type);
int type_arr(char *name);
void add_struct(char *name, struct var_type *var);
int exist_struct(char *name);
#endif