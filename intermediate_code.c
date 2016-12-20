#include "intermediate_code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;

char *new_temp()
{
    static int count = 1;
    char *temp = (char *)malloc(sizeof(char) * 5);
    sprintf(temp, "t%d", count++);
    return temp;
}

char *new_label()
{
    static int count = 1;
    char *temp = (char *)malloc(sizeof(char) * 10);
    sprintf(temp, "label%d", count++);
    return temp;
}

int translate_Args(struct ast *args)
{
    //control arg
    if (args->l->r == 0){
        //Exp
        char *t1 = new_temp();
        translate_Exp(args->l, t1);
        fprintf(fp, "ARG %s\n", t1);
        free(t1);
    }
    else{
        //Exp COMMA Args
        char *t1 = new_temp();
        translate_Args(args->l->r->r);
        translate_Exp(args->l, t1);
        free(t1);
    }
    return 0;
}

int translate_Exp(struct ast *exp, char *place)
{
    if(!strcmp(exp->l->name,"ID")){
        if(exp->l->r==0){
            fprintf(fp, "%s := %s\n", place, exp->l->id);
        }
        else if(!strcmp(exp->l->r->r->name,"Args")){
            //call function with args
            char *func_name = exp->l->id;
            if(!strcmp(func_name,"write")){
                char *t1 = new_temp();
                translate_Exp(exp->l->r->r->l, t1);
                fprintf(fp, "WRITE %s\n", t1);
                free(t1);
            }
            else{
                //analyse args
                char *t1 = new_temp();
                translate_Exp(exp->l->r->r->l, t1);
                translate_Args(exp->l->r->r);
                fprintf(fp, "%s := CALL %s\n", place, exp->l->id);
                free(t1);
            }
        }
        else{
            //call function without args
            char *func_name = exp->l->id;
            char *temp = 0;
            if (!strcmp(func_name, "read"))
            {
                fprintf(fp, "READ %s\n", place);
            }
            else fprintf(fp, "%s := CALL %s\n", place, func_name);
        }
    }
    else if(!strcmp(exp->l->name,"INT")){
        fprintf(fp, "%s := #%d\n", place, exp->l->a);
    }
    else if(!strcmp(exp->l->name,"MINUS")){
        char *t1 = new_temp();
        translate_Exp(exp->l->r, t1);
        fprintf(fp, "%s := #0 - %s\n", place, t1);
        free(t1);
    }
    else if(!strcmp(exp->l->name,"Exp")){
        if(!strcmp(exp->l->r->name,"ASSIGNOP")){
            char *t1 = new_temp();
            char *var_name = exp->l->id;
            translate_Exp(exp->l->r->r, t1);
            fprintf(fp, "%s := %s\n", var_name, t1);
            if(place)
                fprintf(fp, "%s := %s\n", place, var_name);
            free(t1);
        }
        else if(!strcmp(exp->l->r->name,"PLUS")){
            char *t1 = new_temp();
            char *t2 = new_temp();
            translate_Exp(exp->l, t1);
            translate_Exp(exp->l->r->r, t2);
            fprintf(fp, "%s := %s + %s\n", place, t1, t2);
            free(t1);
            free(t2);
        }
        else if(!strcmp(exp->l->r->name,"MINUS")){
            char *t1 = new_temp();
            char *t2 = new_temp();
            translate_Exp(exp->l, t1);
            translate_Exp(exp->l->r->r, t2);
            fprintf(fp, "%s := %s - %s\n", place, t1, t2);
            free(t1);
            free(t2);
        }
        else if(!strcmp(exp->l->r->name,"STAR")){
            char *t1 = new_temp();
            char *t2 = new_temp();
            translate_Exp(exp->l, t1);
            translate_Exp(exp->l->r->r, t2);
            fprintf(fp, "%s := %s * %s\n", place, t1, t2);
            free(t1);
            free(t2);
        }
        else{//relop
            char *label1 = new_label();
            char *label2 = new_label();
            fprintf(fp, "%s := #0\n", place);
            translate_Cond(exp, label1, label2);
            fprintf(fp, "LABEL %s :\n%s := #1\n", label1, place);
            free(label1);
            free(label2);
        }
    }
    return 0;
}

int translate_Cond(struct ast *exp,char *label_true,char *label_false)
{
    //only consider relop
    if(!strcmp(exp->l->name,"Exp")&&!strcmp(exp->l->r->name,"RELOP")){
        char *t1 = new_temp();
        char *t2 = new_temp();
        translate_Exp(exp->l, t1);
        translate_Exp(exp->l->r->r, t2);
        char *op = exp->l->r->id;
        fprintf(fp, "IF %s %s %s GOTO %s\n", t1, op, t2, label_true);
        fprintf(fp, "GOTO %s\n", label_false);
        free(t1);
        free(t2);
    }
    return 0;
}

int translate_Stmt(struct ast *stmt)
{
    if(!strcmp(stmt->l->name,"Exp")){
        translate_Exp(stmt->l, 0);
    }
    else if(!strcmp(stmt->l->name,"CompSt")){
        translate_CompSt(stmt->l);
    }
    else if(!strcmp(stmt->l->name,"RETURN")){
        char *t1 = new_temp();
        translate_Exp(stmt->l->r, t1);
        fprintf(fp, "RETURN %s\n", t1);
        free(t1);
    }
    else if(!strcmp(stmt->l->name,"WHILE")){
        char *label1 = new_label();
        char *label2 = new_label();
        char *label3 = new_label();
        fprintf(fp, "LABEL %s :\n", label1);
        translate_Cond(stmt->l->r->r, label2, label3);
        fprintf(fp, "LABEL %s :\n", label2);
        translate_Stmt(stmt->l->r->r->r->r);
        fprintf(fp, "GOTO %s\nLABEL %s :\n", label1, label3);
        free(label1);
        free(label2);
        free(label3);
    }
    else if(!strcmp(stmt->l->name,"IF")){
        if(stmt->l->r->r->r->r->r==0){
            char *label1 = new_label();
            char *label2 = new_label();
            translate_Cond(stmt->l->r->r, label1, label2);
            fprintf(fp, "LABEL %s :\n", label1);
            translate_Stmt(stmt->l->r->r->r->r);
            fprintf(fp, "LABEL %s :\n", label2);
            free(label1);
            free(label2);
        }
        else{
            char *label1 = new_label();
            char *label2 = new_label();
            char *label3 = new_label();
            translate_Cond(stmt->l->r->r, label1, label2);
            fprintf(fp, "LABEL %s :\n", label1);
            translate_Stmt(stmt->l->r->r->r->r);
            fprintf(fp, "GOTO %s\n", label3);
            fprintf(fp, "LABEL %s :\n", label2);
            translate_Stmt(stmt->l->r->r->r->r->r->r);
            fprintf(fp, "LABEL %s :\n", label3);
            free(label1);
            free(label2);
            free(label3);
        }
    }
    return 0;
}

int translate_Fun(struct ast *fun,struct ast *compst)
{
    static int flag = 0;
    if(!flag)
        fp = fopen("intermediate_code.ir", "w+");
    else
        fp = fopen("intermediate_code.ir", "a+");
    flag++;
    fprintf(fp, "FUNCTION %s :\n", fun->l->id);
    if(fun->l->r->r->r)
        translate_VarList(fun->l->r->r);
    translate_CompSt(compst);
    fprintf(fp, "\n");
    fclose(fp);
}

int translate_VarList(struct ast *varlist)
{
    //control arg
    if (varlist->l->r == 0){
        //Exp
        fprintf(fp, "PARAM %s\n", varlist->l->l->r->l->id);
    }
    else{
        //Exp COMMA Args
        translate_VarList(varlist->l->r->r);
        fprintf(fp, "PARAM %s\n", varlist->l->l->r->l->id);
    }
    return 0;
}
int translate_CompSt(struct ast *compst)
{
    translate_StmtList(compst->l->r->r);
}

int translate_StmtList(struct ast *stmtlist)
{
    if(stmtlist->l!=0){
        translate_Stmt(stmtlist->l);
        translate_StmtList(stmtlist->l->r);
    }
    return 0;
}
