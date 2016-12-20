#ifndef INTERMEDIATE_CODE_H
#define INTERMEDIATE_CODE_H

#include "gramtree.h"

char *new_temp();
char *new_label();
int translate_Args(struct ast *args);
int translate_Exp(struct ast *exp, char *place);
int translate_Cond(struct ast *exp, char *label_true, char *label_false);
int translate_Stmt(struct ast *stmt);
int translate_Fun(struct ast *fun, struct ast *compst);
int translate_CompSt(struct ast *compst);
int translate_StmtList(struct ast *stmtlist);
int translate_VarList(struct ast *varlist);

#endif