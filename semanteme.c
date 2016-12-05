#include "semanteme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct var_type* var_type_p;
typedef struct struct_type* struct_type_p;
typedef struct array_type* array_type_p;
typedef struct fun_type* fun_type_p;
var_type_p var_p = NULL;
struct_type_p struct_p = NULL;
array_type_p array_p = NULL;
fun_type_p fun_p = NULL;
char buf1[50];
extern int flag1;
struct ast *search_name(struct ast *p, char *name)
{//down to find name and return pointer to node
    if(!p)
        return 0;
    if(!strcmp(p->name,name)){
        return p;
    }
    struct ast* i=search_name(p->l, name);
    struct ast* j=search_name(p->r, name);
    if(i)
        return i;
    if(j)
        return j;
    return 0;
}
void add_sym_type(struct ast* p,char *name,int type,char* struct_name)
{
    if(!p)
        return;
    //printf("%s\n", p->name);
    if (!strcmp(p->name, name))
    {
        if(type==7){
            add_var(p->id, type,struct_name,p->line);
            set_arr_type(p->id, type,struct_name);
        }
        else if(type==1){
            printf("func:add_sym_type, add %s:int\n", p->id);
            add_var(p->id, type, struct_name, p->line);
            set_arr_type(p->id, type,struct_name);
        }
        else if(type==2){
            add_var(p->id, type,struct_name,p->line);
            set_arr_type(p->id, type,struct_name);
        }
    }
    add_sym_type(p->l, name,type,struct_name);
    add_sym_type(p->r, name,type,struct_name);
}
void set_sym_type(struct ast* p,char *name,int type,char *struct_name)
{
    if(!p)
        return;
    if(!strcmp(p->name,name)){
        set_var_type(p->id, type);
        set_arr_type(p->id, type,struct_name);
    }
    set_sym_type(p->l, name,type,struct_name);
    set_sym_type(p->r, name,type,struct_name);
}
void set_node_type(struct ast* p,char *name,int type)
{
    if(!p)
        return;
    if(!strcmp(p->name,name)){
        p->type = type;
    }
    set_node_type(p->l, name,type);
    set_node_type(p->r, name,type);
}
void add_var(char *name, int type,char *struct_name,int line)
{
    if(exist_arr(name)){
        return;
    }
    if(exist_var(name)){
        memset(buf1, 0, 50);
        sprintf(buf1, "Error type 3 at line %d: Redefined variable \"%s\".\n", line, name);
        flag1 = 0;
        //printf("%s\n", buf1);
        return;
    }
    else if(exist_fun(name)){
        printf("Error type 3 at line %d: \"%s\" was a function.\n", line, name);
        return;
    }
    var_type_p temp = (var_type_p)malloc(sizeof(struct var_type));
    temp->name = name;
    temp->type = type;
    temp->struct_name = struct_name;
    temp->var_next = var_p;
    var_p = temp;
    printf("add var: %s, type: %d\n", name, type);
}
void add_digui_var(struct ast *p, int type,char *struct_name){
    if(!p)
        return;
    if(!strcmp(p->name,"ID")){
        //del_var(p->id);
        printf("digui,%s:%d\n", p->id, type);
        var_type_p temp = (var_type_p)malloc(sizeof(struct var_type));
        temp->name = p->id;
        temp->type = type;
        temp->struct_name = struct_name;
        temp->var_next = var_p;
        var_p = temp;
    }
    add_digui_var(p->l, type,struct_name);
    add_digui_var(p->r, type,struct_name);
    return;
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
        if (!strcmp(name, p->name)){
            if(!p->type){
                p->type = type;
                printf("set var: %s, type: %d\n", name, type);
            }
            else
                printf("var: %s already has a type: %d\n", name, p->type);
        }
    }
}
void del_var(char *name)
{
    printf("func:del_var\n");
    if (!var_p)
        return;
    if (!strcmp(name, var_p->name))
    {
        var_type_p temp = var_p;
        var_p = var_p->var_next;
        free(temp);
        printf("del var: %s\n", name);
        return;
    }
    for (var_type_p p = var_p; p->var_next; p = p->var_next)
    {
        if (!strcmp(name, p->var_next->name)){
            var_type_p temp = p->var_next;
            p->var_next = p->var_next->var_next;
            free(temp);
            printf("del var %s\n", name);
            return;
        }
    }
    return;
}

void add_arr(char *name,int type,int dimen,char *struct_name)
{
    printf("add arr: %s, dimen: %d\n",name,dimen);
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
void del_arr(char *name){
    printf("del_arr: %s", name);
    array_type_p temp = array_p;
    for (; temp->array_next;temp=temp->array_next){
        if(!strcmp(temp->array_next->name,name)){
            array_type_p p = temp->array_next;
            temp->array_next = temp->array_next->array_next;
            struct dimen *dp = p->d;
            while(dp){
                struct dimen *dpt = dp->next;
                free(dp);
                dp = dpt;
            }
            free(p);
        }
    }
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
void set_arr_type(char *name,int type,char *struct_name)
{
    for (array_type_p p = array_p; p; p = p->array_next)
    {
        if (!strcmp(name, p->name)){
            if(!p->type){
                p->type = type;
                p->struct_name = struct_name;
                printf("set arr: %s, type: %d\n", name, type);
            }
            else printf("arr: %s already has a type: %d\n", name, p->type);
        }
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
int exist_struct_var(var_type_p *var, char *name)
{
    for (var_type_p p = *var; p; p = p->var_next)
    {
        if (!strcmp(name, p->name))
            return 1;
    }
    return 0;
}
void add_struct_var(struct ast *p, var_type_p *var,int type){
    if(!p)
        return;
    if(!strcmp(p->name,"ID")){
        del_var(p->id);
        //del_arr(p->id);
        if(exist_struct_var(var,p->id))
            printf("Error type 15 at line %d: Redefined field \"%s\".\n", p->line, p->id);
        else {
            printf("struct var, %s:%d\n", p->id, type);
            var_type_p temp = (var_type_p)malloc(sizeof(struct var_type));
            temp->name = p->id;
            temp->type = type;
            temp->var_next = *var;
            *var = temp;
        }
    }
    add_struct_var(p->l, var,type);
    add_struct_var(p->r, var,type);
    return;
}
void add_struct(char *struct_name,struct ast* deflist_p)
{
    struct_type_p temp = (struct_type_p)malloc(sizeof(struct struct_type));
    temp->name = struct_name;
    temp->struct_next = struct_p;
    struct_p = temp;
    struct ast *def_p = deflist_p->l;
    while (def_p != 0)
    {
        if (def_p->l->l->type==1)
        {//int
            //printf("add_struct_var:1\n");
            add_struct_var(def_p->l->r, &(temp->var), 1);
        }
        else{
            //printf("add_struct_var:2\n");
            add_struct_var(def_p->l->r, &(temp->var),2);
        }
        //if(def_p->r)
            def_p = def_p->r->l;
        //else
            //def_p = 0;
    }
}
int exist_struct_field(char *name,char *field)
{
    for (var_type_p p = var_p; p;p=p->var_next){
        if(!strcmp(p->name,name)){
            char *struct_name = p->struct_name;
            for (struct_type_p sp = struct_p; sp;sp=sp->struct_next){
                if(!strcmp(sp->name,struct_name)){
                    var_type_p field_p = sp->var;
                    for (; field_p; field_p = field_p->var_next){
                        if(!strcmp(field_p->name,field)){
                            return 1;
                        }
                    }
                    return 0;
                }
            }
        }
    }
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
}/*
void add_fun_para(struct ast *p, var_type_p *var,int type){
    if(!p)
        return;
    if(!strcmp(p->name,"ID")){
        del_var(p->id);
        //del_arr(p->id);
        if(exist_struct_var(var,p->id))
            printf("Error type 15 at line %d: Redefined field \"%s\".\n", p->line, p->id);
        else {
            printf("struct var, %s:%d\n", p->id, type);
            var_type_p temp = (var_type_p)malloc(sizeof(struct var_type));
            temp->name = p->id;
            temp->type = type;
            temp->var_next = *var;
            *var = temp;
        }
    }
    add_struct_var(p->l, var,type);
    add_struct_var(p->r, var,type);
    return;
}*/
void add_fun(char *fun_name,int isdef,int return_type,struct ast* varlist_p)
{
    fun_type_p temp = (fun_type_p)malloc(sizeof(struct fun_type));
    temp->name = fun_name;
    temp->isdef = isdef;
    temp->return_type = return_type;
    temp->fun_next = fun_p;
    fun_p = temp;
    //void node
    temp->para = (var_type_p)malloc(sizeof(struct var_type));
    temp->para->var_next = 0;
    if (!varlist_p)
        return;
    else
    {
        struct ast *para_p = varlist_p->l;
        var_type_p temp_var = temp->para;
        while (para_p != 0)
        {
            if (para_p->l->l->type==1)
            {//int
                //printf("add_struct_var:1\n");
                temp_var->var_next=(var_type_p)malloc(sizeof(struct var_type));
                temp_var->var_next->name = para_p->l->r->l->id;
                temp_var->var_next->type = 1;
                temp_var->var_next->var_next = 0;
                //printf("%s:1\n", temp_var->var_next->name);
                temp_var = temp_var->var_next;
            }
            else{
                //printf("add_struct_var:2\n");
                temp_var->var_next=(var_type_p)malloc(sizeof(struct var_type));
                temp_var->var_next->name = para_p->l->r->l->id;
                temp_var->var_next->type = 2;
                temp_var->var_next->var_next = 0;
                //printf("%s:1\n", temp_var->var_next->name);
                temp_var = temp_var->var_next;
            }
            if(para_p->r)
                para_p = para_p->r->r->l;
            else
                para_p = 0;
        }
    }
    printf("fun_name:%s,return_type:%d\n", temp->name, temp->return_type);
    var_type_p printf_p = temp->para->var_next;
    while (printf_p){
        printf("para_name:%s,type:%d\n", printf_p->name, printf_p->type);
        printf_p = printf_p->var_next;
    }
}
int match_fun(struct ast *exp_p,char *fun_name){
    var_type_p para_p = 0;
    for (fun_type_p p = fun_p; p; p = p->fun_next)
    {
        if(!strcmp(p->name,fun_name)){
            para_p = p->para->var_next;
            break;
        }
    }
    if(!para_p&&!exp_p)
        return 1;
    while (exp_p && para_p)
    {
        printf("exp_p->type:%d, para_p->type:%d\n", exp_p->type, para_p->type);
        if (exp_p->type == para_p->type || (exp_p->type - 3) == para_p->type)
        {
            if(exp_p->r)
                exp_p = exp_p->r->r->l;
            else
                exp_p = 0;
            para_p = para_p->var_next;
        }
        else
            break;
    }
    if(!exp_p)
        printf("exp_p: 0\n");
    if (!exp_p && !para_p)
        return 1;
    if(exp_p&&para_p){
        if(exp_p->type==1)
        printf("Error type 9 at line %d: There is argument int, but fuction float.\n", exp_p->line);
        else 
        printf("Error type 9 at line %d: There is argument float, but fuction int.\n", exp_p->line);
    }
    else if(exp_p){
        printf("Error type 9 at line %d: \"%s\" has too many arguments.\n", exp_p->line, fun_name);
    }
    else
        printf("Error type 9 at line %d: arguments of \"%s\" are not enough.\n", exp_p->line, fun_name);
    return 0;
}
int exist_fun(char *fun_name){
    for (fun_type_p p = fun_p; p; p = p->fun_next){
        if(!strcmp(p->name,fun_name)){
            return 1;
        }
    }
    return 0;
}
int exist_fun_para(char *fun_name,char *para_name){
    var_type_p para_p = 0;
    for (fun_type_p p = fun_p; p; p = p->fun_next)
    {
        if(!strcmp(p->name,fun_name)){
            para_p = p->para->var_next;
        }
    }
    if(!para_p)
        return 0;
    for (; para_p; para_p = para_p->var_next)
    {
        if(!strcmp(para_p->name,para_name)){
            return para_p->type;
        }
    }
    return 0;
}
//int cmpa_fun()