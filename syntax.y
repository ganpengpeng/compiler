%{
    #include "semanteme.h"
    #include "intermediate_code.h"
    #include "lex.yy.c"
    //control print syntax tree or not
    int flag = 0;
    //sign redef var at semanteme.c
    int flag1 = 0;
    int flag2 = 0;
    //mark func is exist and def or not
    int flag3 = 0;
    extern char buf1[50];
    char buf2[50];
%}
%union{
    struct ast* p;
}
%locations
%error-verbose
%token <p> INT
%token <p> FLOAT
%token <p> AND ASSIGNOP COMMA DIV DOT ELSE
%token <p> ID IF LP RP LB RB LC RC
%token <p> MINUS NOT OR PLUS RELOP SEMI RETURN
%token <p> STAR STRUCT TYPE WHILE
%type <p> Program ExtDefList ExtDef ExtDecList
%type <p> Specifier StructSpecifier OptTag
%type <p> Tag VarDec FunDec VarList Args
%type <p> ParamDec CompSt StmtList Exp
%type <p> Stmt DefList Def DecList Dec
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%left UMINUS
%right NOT
%left LP RP LB RB DOT
%%
Program:
    ExtDefList {$$=new_ast("Program",1,$1);if(!flag) print_tree($$,1);};
ExtDefList:
    ExtDef ExtDefList {$$=new_ast("ExtDefList",2,$1,$2);}
    | {$$=new_ast("ExtDefList",-1);};
ExtDef:
    Specifier ExtDecList SEMI {
        $$=new_ast("ExtDef",3,$1,$2,$3);
        if(flag1){
            printf("%s", buf1);
            flag1 = 0;
        }
        if(!strcmp($1->l->name,"TYPE"))
            if(!strcmp($1->l->id,"int"))
            {//int variables
                printf("specifier int\n");
                add_sym_type($2,"ID",1,0);
            }
            else{//float variables
                printf("ExtDef:specifier float\n");
                add_sym_type($2,"ID",2,0);
            }
        else{//struct variables
            printf("specifier struct %s\n",$1->id);
            add_sym_type($2,"ID",7,$1->id);
        }
    }
    | Specifier SEMI {$$=new_ast("ExtDef",2,$1,$2);}
    | Specifier FunDec SEMI{
        //func declare check
        char *fun_name = $2->l->id;
        int return_type = $1->l->type;
        struct ast *varlist_p = 0;
        if (!strcmp($2->l->r->r->name, "VarList"))
            varlist_p = $2->l->r->r;
        if(exist_fun(fun_name)){
            int i = match_fun(varlist_p, fun_name);
            if(i!=1){
                printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", $2->line, fun_name);
            }
        }
        else {
            add_fun(fun_name, 0, return_type, varlist_p);
        }
    }
    | Specifier FunDec CompSt {
        char *fun_name = $2->l->id;
        int return_type = $1->l->type;
        int ture_return_type = 0;
        struct ast *varlist_p = 0;
        flag3 = 0;
        //printf("fun_name:%s,return_type:%d,ture_return_type:%d,line:%d\n", fun_name, return_type,ture_return_type,$2->line);
        if (!strcmp($2->l->r->r->name, "VarList"))
            varlist_p = $2->l->r->r;
        if (exist_fun(fun_name))
        {
            if(isdef_fun(fun_name)){
                printf("Error type 4 at line %d: Redefined function \"%s\".\n", $2->line, fun_name);
                flag3 = 1;
            }
        }
        else{//add fun here
            add_fun(fun_name, 1, return_type, varlist_p);
        }
        struct ast *return_p = search_name($3,"RETURN");
        if(!flag3){
            if (return_p)
            {
                //printf("1\n");
                ture_return_type = return_p->r->type;
                if (!strcmp(return_p->r->l->name, "ID"))
                {//return id,judge if id at func paralist
                    ture_return_type = return_p->r->type;
                    if(exist_fun_para(fun_name,return_p->r->l->id)){
                        ture_return_type = exist_fun_para(fun_name,return_p->r->l->id);
                        printf("%d..2\n",ture_return_type);
                        flag2 = 0;
                    }
                    else{
                        //printf("3\n");
                        if(flag2){
                            printf("%s", buf2);
                            flag2 = 0;
                        }
                    }
                }
                else if(flag2){
                    printf("%s", buf2);
                    flag2 = 0;
                    ture_return_type = return_p->r->type;
                }
                //printf("4\n");
            }
            else if(flag2){
                printf("%s", buf2);
                flag2 = 0;
            }
            if(!return_p){
                printf("Error type 8 at line %d: Function \"%s\" has no return statement.\n", $3->l->r->r->r->line, fun_name);
            }
            else if(return_type==1){
                if(ture_return_type!=1&&ture_return_type!=4){
                    printf("%d,%d\n", return_type, ture_return_type);
                    printf("Error type 8 at line %d: Type mismatched for return.\n", return_p->line);
                }
            }
            else if(ture_return_type!=2&&ture_return_type!=5){
                printf("Error type 8 at line %d: Type mismatched for return.\n", return_p->line);
            }
        }
        flag3 = 0;
        $$ = new_ast("ExtDef", 3, $1, $2, $3);
        //printf("ExtDef end\n");
    };
ExtDecList:
    VarDec {$$=new_ast("ExtDecList",1,$1);}
    | VarDec COMMA ExtDecList {$$=new_ast("ExtDecList",3,$1,$2,$3);};
Specifier:
    TYPE {$$=new_ast("Specifier",1,$1);}
    | StructSpecifier {$$=new_ast("Specifier",1,$1);};
StructSpecifier:
    STRUCT OptTag LC DefList RC {
        flag1 = 0;
        if (exist_var($2->l->id) || exist_arr($2->l->id))
        {
            printf("Error type 3 at line %d: Redefined variable \"%s\".\n", $1->line, $1->id);
            }
            else if(exist_struct($2->l->id)){
                printf("Error type 16 at line %d: Duplicated name \"%s\".\n", $2->line, $2->l->id);
            }
            else{
                add_struct($2->l->id, $4);
                //printf("%s\n", $2->l->id);
            }
            $$ = new_ast("StructSpecifier", 5, $1, $2, $3, $4, $5);
            $$->type = 7;
            $$->id = $2->l->id;
        }
    | STRUCT Tag { 
        $$ = new_ast("StructSpecifier", 2, $1, $2);
        //printf("%s\n", $2->id);
        if (!exist_struct($2->id))
        {
            printf("Error type 17 at line %d: Undefined structure \"%s\".\n", $2->line, $2->id);
        }
    };
OptTag:
    ID {$$=new_ast("OptTag",1,$1);}
    | {$$=new_ast("OptTag",-1);};
Tag:
    ID {$$=new_ast("Tag",1,$1);};
VarDec:
    ID {
        //$1->type = 0;
        $$=new_ast("VarDec",1,$1);
        /*
        /if (exist_var($1->id) || exist_arr($1->id))
        {
            flag = 1;
            printf("Error type 3 at line %d: Redefined variable \"%s\".\n", $1->line, $1->id);
        }
        else
            add_var($1->id, 0);*/
    }
    | VarDec LB INT RB {
        $1->type = 3;
        $$ = new_ast("VarDec", 4, $1, $2, $3, $4);
        //del_var($1->l->id);
        add_arr($1->id, 0, $3->a,0);
    };
FunDec:
    ID LP VarList RP {$$=new_ast("FunDec",4,$1,$2,$3,$4);}
    | ID LP RP {$$=new_ast("FunDec",3,$1,$2,$3);};
VarList:
    ParamDec COMMA VarList {$$=new_ast("VarList",3,$1,$2,$3);}
    | ParamDec {$$=new_ast("VarList",1,$1);};
ParamDec:
    Specifier VarDec {$$=new_ast("ParamDec",2,$1,$2);};
CompSt:
    LC DefList StmtList RC {
        if (flag1)
        {
            printf("%s", buf1);
            flag1 = 0;
        }
        $$=new_ast("CompSt",4,$1,$2,$3,$4);
    };
StmtList:
    Stmt StmtList {
        $$ = new_ast("StmtList", 2, $1, $2);
    }
    | { $$ = new_ast("StmtList", -1); };
Stmt:
    Exp SEMI {$$=new_ast("Stmt",2,$1,$2);}
    | CompSt {$$=new_ast("Stmt",1,$1);}
    | RETURN Exp SEMI {$$=new_ast("Stmt",3,$1,$2,$3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=new_ast("Stmt",5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=new_ast("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
    | WHILE LP Exp RP Stmt {$$=new_ast("Stmt",5,$1,$2,$3,$4,$5);}
    | error SEMI {yyerrok;};
DefList:
    Def DefList {
    if ($1->l->l->type == 1)
    {
        //printf("qianmian\n");
        add_sym_type($1->l->r, "ID", 1, 0);
        //printf("houmian\n");
        }
        else if($1->l->l->type==2){
            add_sym_type($1->l->r, "ID", 2, 0);
        }
        else if(exist_struct($1->l->l->l->r->id)){
            add_sym_type($1->l->r, "ID", 7, $1->l->l->l->r->id);
        }
        $$=new_ast("DefList",2,$1,$2);
    }
    | { $$ = new_ast("DefList", -1); };
Def:
    Specifier DecList SEMI {
        $$=new_ast("Def",3,$1,$2,$3);
        /*if(!strcmp($1->l->name,"TYPE"))
            if(!strcmp($1->l->id,"int"))
            {//int variables
                printf("specifier int\n");
                add_sym_type($2,"ID",1,0);
            }
            else{//float variables
                printf("Def:specifier float\n");
                add_sym_type($2,"ID",2,0);
            }
        else{//struct variables
            printf("specifier struct\n");
            add_sym_type($2,"ID",7,$1->id);
        }*/
    }
    | error SEMI {yyerrok;};
DecList:
    Dec {$$=new_ast("DecList",1,$1);}
    | Dec COMMA DecList {$$=new_ast("DecList",3,$1,$2,$3);};
Dec:
    VarDec {$$=new_ast("Dec",1,$1);}
    | VarDec ASSIGNOP Exp {
        $$=new_ast("Dec",3,$1,$2,$3);
    };
Exp : Exp ASSIGNOP Exp {
        if ($1->type == 4 || $1->type == 5)
        {
            printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", $1->line);
        }
        else if($1->type&&$3->type&&$1->type!=$3->type){
            printf("Error type 5 at line %d: Type mismatched for assignment.\n",$1->line);
        }
        else if($1->type&&$3->type&&$1->type==7&&$3->type==7){
            char *type1, *type2;
            type_var($1->id, &type1);
            type_var($3->id, &type2);
            if (strcmp(type1, type2))
            {
                printf("Error type 5 at line %d: Type mismatched for assignment.\n", $1->line);
            }
        }
        $$ = new_ast("Exp", 3, $1, $2, $3);
        $$->type = 0;
    }
    | Exp AND Exp { $$ = new_ast("Exp", 3, $1, $2, $3); }
    | Exp OR Exp { $$ = new_ast("Exp", 3, $1, $2, $3); }
    | Exp RELOP Exp { $$ = new_ast("Exp", 3, $1, $2, $3); }
    | Exp PLUS Exp
    {
        $$ = new_ast("Exp", 3, $1, $2, $3);
        //printf("$1->type:%d, $3->type:%d\n", $1->type, $3->type);
        if ($1->type && $3->type && $1->type != $3->type)
            if(!($1->type == 1&&$3->type==4||$1->type == 4&&$3->type==1||$1->type == 2&&$3->type==5||$1->type == 5&&$3->type==2))
            {
                $$->type = 0;
                printf("Error type 7 at line %d: Type mismatched for operands.\n", $1->line);
            }
    }
    | Exp MINUS Exp {$$=new_ast("Exp",3,$1,$2,$3);}
    | Exp STAR Exp {$$=new_ast("Exp",3,$1,$2,$3);}
    | Exp DIV Exp {$$=new_ast("Exp",3,$1,$2,$3);}
    | LP Exp RP {$$=new_ast("Exp",3,$1,$2,$3);}
    | MINUS Exp %prec UMINUS {$$=new_ast("Exp",2,$1,$2);}
    | NOT Exp {$$=new_ast("Exp",2,$1,$2);}
    | ID LP Args RP {
        int i = 0;
        if (exist_var($1->id) || exist_arr($1->id) || exist_struct($1->id))
        {
            printf("Error type 11 at line %d: \"%s\" is not a function.\n", $1->line, $1->id);
        }
        else if(!exist_fun($1->id)){
            printf("Error type 2 at line %d: Undefined function \"%s\".\n", $1->line,$1->id);
        }
        else
            i = match_fun($3->l, $1->id);
        if(i==2){
            printf("Error type 9 at line %d: There is argument int, but fuction float.\n", $3->line);
        }
        else if(i==3){
            printf("Error type 9 at line %d: There is argument float, but fuction int.\n", $3->line);
        }
        else if(i==4){
            printf("Error type 9 at line %d: \"%s\" has too many arguments.\n", $3->line, $1->id);
        }
        else if(i==5){
            printf("Error type 9 at line %d: arguments of \"%s\" are not enough.\n", $3->line, $1->id);
        }
        $$ = new_ast("Exp", 4, $1, $2, $3, $4);
    }
    | ID LP RP {$$=new_ast("Exp",3,$1,$2,$3);}
    | Exp LB Exp RB {
        $$ = new_ast("Exp", 4, $1, $2, $3, $4);
        if($1->type==4){
            printf("Error type 10 at line %d: \"%d\" is not an array.\n", $1->line, $1->a);
            $$->type = 0;
            //flag = 1;
        }
        else if($1->type==5) {
            printf("Error type 10 at line %d: \"%f\" is not an array.\n", $1->line, $1->b);
            $$->type = 0;
            //flag = 1;
        }
        else if(exist_var($1->id)){
            printf("Error type 10 at line %d: \"%s\" is not an array.\n", $1->line, $1->id);
            $$->type = 0;
            //flag = 1;
        }
        else if(!exist_arr($1->id)){
            printf("Error type 1 at line %d: Undefined variable \"%s\".\n", $1->line, $1->id);
            $$->type = 0;
            //flag = 1;
        }
        else if(exist_arr($1->id))
            $$->type = type_arr($1->id);
        else
            $$->type = 0;
        if($3->type==5){
            $$->type = 0;
            printf("Error type 12 at line %d: \"%f\" is not an intger.\n", $3->line,$3->b);
        }
        else if($3->type==2){
            struct ast *temp = search_name($3, "ID");
            if(temp){
                $$->type = 0;
                printf("Error type 12 at line %d: \"%s\" is not an intger.\n", $3->line,temp->id);
            }
        }
    }
    | Exp DOT ID {
        $$=new_ast("Exp",3,$1,$2,$3);
        //printf("dot---\n");
        char *temp;
        if (type_var($1->l->id, &temp) != 7)
        {
            printf("Error type 13 at line %d: Illegal use of \".\".\n", $2->line);
        }
        else if(!exist_struct_field($1->l->id,$3->id)){
            printf("Error type 14 at line %d: Non-existent field \"%s\".\n", $3->line, $3->id);
        }
    }
    | ID {
        if (!exist_var($1->id) && !exist_arr($1->id) && !exist_struct($1->id))
        {
            memset(buf2, 0, 50);
            sprintf(buf2,"Error type 1 at line %d: Undefined variable \"%s\".\n", $1->line, $1->id);
            flag2 = 1;
            $1->type = 0;
            //flag = 1;
        }
        else if(exist_var($1->id)){
            char *temp;
            $1->type = type_var($1->id, &temp);
        }
        else
            $1->type = type_arr($1->id);
        $$ = new_ast("Exp", 1, $1);
    }
    | INT {
        $$=new_ast("Exp",1,$1);
    }
    | FLOAT {$$=new_ast("Exp",1,$1);};
Args:
    Exp COMMA Args {$$=new_ast("Args",3,$1,$2,$3);}
    | Exp {$$=new_ast("Args",1,$1);};
%%
yyerror(const char *msg)
{
    printf("Syntax error at line %d : %s\n", yylineno, msg);
    flag = 1;
}