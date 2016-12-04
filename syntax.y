%{
    #include "semanteme.h"
    #include "lex.yy.c"
    int flag = 0;
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
    ExtDefList {$$=new_ast("Program",1,$1);/*if(!flag) print_tree($$,1);*/};
ExtDefList:
    ExtDef ExtDefList {$$=new_ast("ExtDefList",2,$1,$2);}
    | {$$=new_ast("ExtDefList",-1);};
ExtDef:
    Specifier ExtDecList SEMI {
        $$=new_ast("ExtDef",3,$1,$2,$3);
        if(!strcmp($1->l->name,"TYPE"))
            if(!strcmp($1->l->id,"int"))
            {//int variables
                printf("specifier int\n");
                set_sym_type($2,"ID",1);
            }
            else{//float variables
                printf("specifier float\n");
                set_sym_type($2,"ID",2);
            }
        else{//struct variables
            printf("specifier struct\n");
            set_sym_type($2,"ID",7);
        }
    }
    | Specifier SEMI {$$=new_ast("ExtDef",2,$1,$2);}
    | Specifier FunDec CompSt {$$=new_ast("ExtDef",3,$1,$2,$3);};
ExtDecList:
    VarDec {$$=new_ast("ExtDecList",1,$1);}
    | VarDec COMMA ExtDecList {$$=new_ast("ExtDecList",3,$1,$2,$3);};
Specifier:
    TYPE {$$=new_ast("Specifier",1,$1);}
    | StructSpecifier {$$=new_ast("Specifier",1,$1);};
StructSpecifier:
    STRUCT OptTag LC DefList RC {$$=new_ast("StructSpecifier",5,$1,$2,$3,$4,$5);}
    | STRUCT Tag {$$=new_ast("StructSpecifier",2,$1,$2);};
OptTag:
    ID {$$=new_ast("OptTag",1,$1);}
    | {$$=new_ast("OptTag",-1);};
Tag:
    ID {$$=new_ast("Tag",1,$1);};
VarDec:
    ID {
        $1->type = 0;
        $$=new_ast("VarDec",1,$1);
        if (exist_var($1->id) || exist_arr($1->id))
        {
            flag = 1;
            printf("Error type 3 at line %d: Redefined variable \"%s\".\n", $1->line, $1->id);
        }
        else
            add_var($1->id, 0);
    }
    | VarDec LB INT RB {
        $1->type = 3;
        $$ = new_ast("VarDec", 4, $1, $2, $3, $4);
        del_var($1->l->id);
        add_arr($1->id, 0, $3->a);
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
    LC DefList StmtList RC {$$=new_ast("CompSt",4,$1,$2,$3,$4);};
StmtList:
    Stmt StmtList {$$=new_ast("StmtList",2,$1,$2);}
    | {$$=new_ast("StmtList",-1);};
Stmt:
    Exp SEMI {$$=new_ast("Stmt",2,$1,$2);}
    | CompSt {$$=new_ast("Stmt",1,$1);}
    | RETURN Exp SEMI {$$=new_ast("Stmt",3,$1,$2,$3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=new_ast("Stmt",5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=new_ast("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
    | WHILE LP Exp RP Stmt {$$=new_ast("Stmt",5,$1,$2,$3,$4,$5);}
    | error SEMI {yyerrok;};
DefList:
    Def DefList {$$=new_ast("DefList",2,$1,$2);}
    | {$$=new_ast("DefList",-1);};
Def:
    Specifier DecList SEMI {
        $$=new_ast("Def",3,$1,$2,$3);
        if(!strcmp($1->l->name,"TYPE"))
            if(!strcmp($1->l->id,"int"))
            {//int variables
                printf("specifier int\n");
                set_sym_type($2,"ID",1);
            }
            else{//float variables
                printf("specifier float\n");
                set_sym_type($2,"ID",2);
            }
        else{//struct variables
            printf("specifier struct\n");
            set_sym_type($2,"ID",7);
        }
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
        if($1->type==4||$1->type==5){
            printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", $1->line);
        }
        else if($1->type&&$3->type&&$1->type!=$3->type){
            printf("Error type 5 at line %d: Type mismatched for assignment.\n",$1->line);
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
        if($1->type&&$3->type&&$1->type!=$3->type){
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
        
        $$=new_ast("Exp",4,$1,$2,$3,$4);
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
    | Exp DOT ID {$$=new_ast("Exp",3,$1,$2,$3);}
    | ID {
        if(!exist_var($1->id)&&!exist_arr($1->id)){
            printf("Error type 1 at line %d: Undefined variable \"%s\".\n", $1->line, $1->id);
            $1->type = 0;
            //flag = 1;
        }
        else if(exist_var($1->id))
            $1->type = type_var($1->id);
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