%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "symtab.h"
#include "codegene.h"
#define   BUFFERSIZE   1024
char buff[BUFFERSIZE];
int tok = -1;
int line_no = 1;
struct Node* ASTRoot;
extern stlink st_head;
extern stlink st_opened;
extern int semanticErrorCount;
extern FILE *yyin;
%}

%union
{
    struct Node *node;
}

%token PROGRAM VAR LPAREN RPAREN COMMA SEMICOLON COLON 
LBRAC RBRAC OF DOTDOT DOT THEN ELSE DO UNTIL TO DOWNTO

%token <node> NUM IDENTIFIER ASSIGN IF WHILE PLUS MINUS MUL DIV PBEGIN END INTEGER REAL STRING ARRAY FUNCTION PROCEDURE STRINGCONST NOT LT GT LE GE EQUAL notEQUAL REPEAT FOR AND OR
%type <node> goal prog factor term simple_expression expression variable statement relop addop compound_expression
%type <node> mulop statement_list optional_statements compound_statement declarations identifier_list type tail
%type <node> standard_type subprogram_declarations subprogram_declaration subprogram_head arguments parameter_list expression_list procedure_statement

%%
goal
    : prog   { ASTRoot = $$; }
    ;

prog: PROGRAM IDENTIFIER LPAREN identifier_list RPAREN SEMICOLON
      declarations
      subprogram_declarations
      compound_statement
      DOT
      { $$ = MakeNode(NODE_PROGRAM);
          AdoptChildren($$, $2);
          AdoptChildren($2, $4);
          AdoptChildren($$, $7);
          AdoptChildren($$, $8);
          AdoptChildren($$, $9); }
  ;
identifier_list: identifier_list COMMA IDENTIFIER  { MakeSiblings($1, $3);}
  | IDENTIFIER 
  ;
declarations: declarations VAR identifier_list COLON type SEMICOLON { $$ = MakeNode(NODE_VAR_DECL); 
                                                                      if ($1->nodeType != NODE_NULL) MakeSiblings($$, $1);
                                                                      AdoptChildren($$, $3); 
                                                                      AdoptChildren($$, $5); }
  |     { $$ = MakeNode(NODE_NULL); }
  ;
type: standard_type
  | ARRAY LBRAC NUM DOTDOT NUM RBRAC OF type  { MakeSiblings($1, $8); $$ = $1; MakeFamily($1, $3, $5); }
  ;

standard_type: INTEGER
  | REAL
  | STRING
  ;     
subprogram_declarations: subprogram_declarations subprogram_declaration SEMICOLON  { $$ = $2; if ($1->nodeType != NODE_NULL) MakeSiblings($$, $1); }
  |   { $$ = MakeNode(NODE_NULL); }
  ;

subprogram_declaration: subprogram_head
                        declarations
                        compound_statement     { AdoptChildren($$, $2); AdoptChildren($$, $3); } 
  ;
                        
subprogram_head: FUNCTION IDENTIFIER arguments COLON standard_type SEMICOLON  { $$ = $1; AdoptChildren($$, $2); AdoptChildren($$, $3); AdoptChildren($$, $5); } 
  | PROCEDURE IDENTIFIER arguments SEMICOLON                                  { $$ = $1; AdoptChildren($$, $2); AdoptChildren($$, $3); }
  ;
arguments: LPAREN parameter_list RPAREN    { $$ = $2; }
  |   { $$ = MakeNode(NODE_NULL); }
  ;
parameter_list: optional_var identifier_list COLON type   { $$ = MakeNode(NODE_PARAS); AdoptChildren($$, $2); AdoptChildren($$, $4); } 
  | optional_var identifier_list COLON type SEMICOLON parameter_list   { $$ = MakeNode(NODE_PARAS); AdoptChildren($$, $2); AdoptChildren($$, $4); MakeSiblings($$, $6); } 
  ;

optional_var: VAR
  |
  ;

compound_statement: PBEGIN
                    optional_statements
                    END                   {$$ = MakeNode(NODE_BLOCK); DeleteNode($1); AdoptChildren($$, $2); }
  ;
optional_statements: statement_list
  ;

statement_list: statement_list SEMICOLON statement   { if ($3->nodeType != NODE_NULL) MakeSiblings($1, $3);}
  | statement
  ;
statement: variable ASSIGN expression           {MakeFamily($2, $1, $3); $$ = $2;}
  | procedure_statement
  | compound_statement
  | IF expression THEN statement ELSE statement  {MakeFamily($1, $2, $4); MakeSiblings($4, $6);}
  | IF compound_expression THEN statement ELSE statement   {MakeFamily($1, $2, $4); MakeSiblings($4, $6);}
  | WHILE expression DO statement                {MakeFamily($1, $2, $4);}
  | WHILE compound_expression DO statement       {MakeFamily($1, $2, $4);}
  | REPEAT statement UNTIL expression            {MakeFamily($1, $4, $2);}
  | REPEAT statement UNTIL compound_expression    {MakeFamily($1, $4, $2);}
  | FOR variable ASSIGN NUM TO NUM DO statement   {MakeFamily($1, $2, $8); MakeFamily($2, $4, $6);}
  | FOR variable ASSIGN NUM DOWNTO NUM DO statement   {MakeFamily($1, $2, $8); MakeFamily($2, $4, $6);}
  | { $$ = MakeNode(NODE_NULL); }
  ;

variable: IDENTIFIER tail          { if ($2->nodeType != NODE_NULL) AdoptChildren($1, $2); }
;
tail: LBRAC expression RBRAC tail   { if ($4->nodeType != NODE_NULL) MakeSiblings($2, $4); $$ = $2; }
  |  { $$ = MakeNode(NODE_NULL); }
  ;
procedure_statement: IDENTIFIER               { $$ = MakeNode(NODE_CALL); AdoptChildren($$, $1); }
  | IDENTIFIER LPAREN expression_list RPAREN  { $$ = MakeNode(NODE_CALL); MakeFamily($$, $1, $3); }
  ;
expression_list: expression              
  | expression_list COMMA expression    { MakeSiblings($1, $3); }
  ;
expression: simple_expression       
  | simple_expression relop simple_expression  {MakeFamily($2, $1, $3); $$ = $2;}
  ;
compound_expression: expression AND expression {MakeFamily($2, $1, $3); $$ = $2;}
  | expression OR expression {MakeFamily($2, $1, $3); $$ = $2;}
  ;
simple_expression: term             
  | simple_expression addop term   {MakeFamily($2, $1, $3); $$ = $2;}
  ;
term: factor                
  | term mulop factor     {MakeFamily($2, $1, $3); $$ = $2;}
  ;
factor: IDENTIFIER tail           { if ($2->nodeType != NODE_NULL) AdoptChildren($1, $2); }
  | IDENTIFIER LPAREN expression_list RPAREN     { $$ = MakeNode(NODE_CALL); MakeFamily($$, $1, $3); }
  | NUM                                       
  | STRINGCONST
  | NOT factor                  {AdoptChildren($1, $2);}
  | LPAREN expression RPAREN    {$$ = $2;}  
  ;
addop: PLUS 
  | MINUS
  ;
relop: LT
  | GT
  | EQUAL
  | LE
  | GE
  | notEQUAL
  ;
mulop: MUL
  | DIV
  ;
%%

int main(int argc, char** argv)  
{  
    stlink st;
    char output[128];
    char *delim = ".";
    char *pch;
    if (argc == 2) {
        yyin = fopen(argv[1], "r");
        pch = strtok(argv[1], delim);
        if (yyparse() != 0) return -1;
    } else if (argc == 3) {
        yyin = fopen(argv[2], "r");
        pch = strtok(argv[2], delim);
        if (yyparse() != 0) return -1;
    } else {
        fprintf(stderr, "usage: ./mini_pascal input.p\n");
        return -1;
    }
      
    //printf("********************************\n"
    //       "*     abstract syntax tree     *\n"
    //       "********************************\n");
    if (argc == 3 && strcmp(argv[1], "-ast") == 0) 
        PrintTree(ASTRoot, 0);

    //printf("********************************\n"
    //       "*       semantic checking      *\n"
    //       "********************************\n");
    MakeSymtab();
    importPrintLib();
    processNode(ASTRoot);
    //printf("********************************\n"
    //       "*        symbol tables         *\n"
    //       "********************************\n");
    //st = st_head;
    //while (st) {
    //    printf("%d: ", st->index);
    //    displayEntry(st);
    //    st = st->next;
    //}

    //printf("********************************\n"
    //       "*        create j file         *\n"
    //       "********************************\n");
    strcpy(output, pch);
    strcat(output, ".j");
    if (semanticErrorCount == 0) codeGenerateMain(ASTRoot, output);
    return 0;  
}  
  
int yyerror(char *s)  
{  
    printf("%s at line %d\n", s, line_no);  
    return 0;  
}  

int yywrap()
{
    return 1;
}

