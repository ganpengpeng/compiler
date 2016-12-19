#include "intermediate_code.h"

char *new_temp()
{
    static int count = 0;
    char *temp = (char *)malloc(sizeof(char) * 5);
    sprintf(temp, "t%d", count++);
    return temp;
}

char *new_label()
{
    static int count = 0;
    char *temp = (char *)malloc(sizeof(char) * 10);
    sprintf(temp, "label%d", count++);
    return temp;
}

char *translate_Exp(struct ast *exp,char *place)
{
    if(!strcmp(exp->l->name,"ID")){
        char *temp = (char *)malloc(sizeof(char) * (strlen(place) + strlen(exp->l->id) + 10));
        sprintf(temp, "%s := %s", place, exp->l->id);
        return temp;
    }
    else if(!strcmp(exp->l->name,"INT")){
        char *temp = (char *)malloc(sizeof(char) * (strlen(place) + 15));
        sprintf(temp, "%s := #%d", place, exp->l->a);
        return temp;
    }
    else if(!strcmp(exp->l->name,"MINUS")){
        char *t1 = new_temp();
        char *code1 = translate_Exp(exp->l->r, t1);
        char *code2 = (char *)malloc(sizeof(char) * (strlen(place) + strlen(t1) + 15));
        sprintf(code2, "%s := #0 - %s", place, t1);
        char *temp = (char *)malloc(sizeof(char) * (strlen(code1) + strlen(code2) + 5));
        sprintf(temp, "%s\n%s", code1, code2);
        free(code1);
        free(code2);
        free(t1);
        return temp;
    }
    else if(!strcmp(exp->l->name,"Exp")){
        if(!strcmp(exp->l->r,"ASSIGNOP")){
            char *t1 = new_temp();
            char *var_name = exp->l->id;
            char *code1 = translate_Exp(exp->l->r->r, t1);
            char *code2 = (char *)malloc(sizeof(char) * (strlen(var_name) * 2 + strlen(place) + strlen(t1) + 10));
            sprintf(code2, "%s := %s\n%s := %s", var_name, t1, place, var_name);
            char *temp = (char *)malloc(sizeof(char) * (strlen(code1) + strlen(code2) + 5));
            sprintf(temp, "%s\n%s", code1, code2);
            free(code1);
            free(code2);
            free(t1);
            return temp;
        }
        else if(!strcmp(exp->l->r,"PLUS")){
            char *t1 = new_temp();
            char *t2 = new_temp();
            char *code1 = translate_Exp(exp->l, t1);
            char *code2 = translate_Exp(exp->l->r->r, t2);
            char *code3 = (char *)malloc(sizeof(char) * (strlen(t2) + strlen(place) + strlen(t1) + 10));
            sprintf(code3, "%s := %s + %s", place, t1, t2);
            char *temp = (char *)malloc(sizeof(char) * (strlen(code1) + strlen(code2) + strlen(code3)+ 10));
            sprintf(temp, "%s\n%s\n%s", code1, code2,code3);
            free(code1);
            free(code2);
            free(code3);
            free(t1);
            free(t2);
            return temp;
        }
        else{
            
        }
    }
}