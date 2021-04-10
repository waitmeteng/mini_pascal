%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "node.h"
#define BUFFERSIZE 1024
extern char buff[BUFFERSIZE];
extern int tok;
extern int line_no;
%}

A [aA]
B [bB]
C [cC]
D [dD]
E [eE]
F [fF]
G [gG]
H [hH]
I [iI]
J [jJ]
K [kK]
L [lL]
M [mM]
N [nN]
O [oO]
P [pP]
Q [qQ]
R [rR]
S [sS]
T [tT]
U [uU]
V [vV]
W [wW]
X [xX]
Y [yY]
Z [zZ]

%%
"//".*$                          ;
{P}{R}{O}{G}{R}{A}{M}            return tok = PROGRAM;
{V}{A}{R}                        return tok = VAR;
{A}{R}{R}{A}{Y}                  {       yylval.node = MakeNode(NODE_ARRAY); return tok = ARRAY;}
{O}{F}                           return tok = OF;
{N}{O}{T}                        {       yylval.node = MakeNode(NODE_NOT); return tok = NOT;}
{I}{N}{T}{E}{G}{E}{R}            {       yylval.node = MakeNode(NODE_TYPEINT); return tok = INTEGER;}
{R}{E}{A}{L}                     {       yylval.node = MakeNode(NODE_TYPEREAL); return tok = REAL;}
{S}{T}{R}{I}{N}{G}               {       yylval.node = MakeNode(NODE_TYPESTRING); return tok = STRING;}
{F}{U}{N}{C}{T}{I}{O}{N}         {       yylval.node = MakeNode(NODE_FUNC); return tok = FUNCTION;}
{P}{R}{O}{C}{E}{D}{U}{R}{E}      {       yylval.node = MakeNode(NODE_PROC); return tok = PROCEDURE;}
{B}{E}{G}{I}{N}                  {       yylval.node = MakeNode(NODE_BEGIN); return tok = PBEGIN;}
{E}{N}{D}                        {       yylval.node = MakeNode(NODE_END); return tok = END;}
{I}{F}                           {       yylval.node = MakeNode(NODE_IF);return tok = IF;}
{T}{H}{E}{N}                     return tok = THEN;
{E}{L}{S}{E}                     return tok = ELSE;
{W}{H}{I}{L}{E}                  {       yylval.node = MakeNode(NODE_WHILE);return tok = WHILE;}
{D}{O}                           return tok = DO; 
{F}{O}{R}                        {       yylval.node = MakeNode(NODE_FOR);return tok = FOR;}
{T}{O}                           return tok = TO;
{D}{O}{W}{N}{T}{O}               return tok = DOWNTO; 
{R}{E}{P}{E}{A}{T}               {       yylval.node = MakeNode(NODE_REPEAT);return tok = REPEAT;}
{U}{N}{T}{I}{L}                  return tok = UNTIL; 
[a-zA-Z_]([a-zA-Z0-9_])*         {       yylval.node = MakeNode(NODE_ID); yylval.node->id = strdup(yytext); return tok = IDENTIFIER;}
"("                              return tok = LPAREN;
")"                              return tok = RPAREN;
";"                              return tok = SEMICOLON;
"."                              return tok = DOT;
","                              return tok = COMMA;
":"                              return tok = COLON;
"["                              return tok = LBRAC;
"]"                              return tok = RBRAC;
":="                             {       yylval.node = MakeNode(NODE_ASSIGN); return tok = ASSIGN;}
"<"                              {       yylval.node = MakeNode(NODE_OP_LT); return tok = LT;}
">"                              {       yylval.node = MakeNode(NODE_OP_GT); return tok = GT;}
"<="                             {       yylval.node = MakeNode(NODE_OP_LE); return tok = LE;}
">="                             {       yylval.node = MakeNode(NODE_OP_GE); return tok = GE;}
"="                              {       yylval.node = MakeNode(NODE_OP_EQ); return tok = EQUAL;}
"!="                             {       yylval.node = MakeNode(NODE_OP_NE); return tok = notEQUAL;}
"+"                              {       yylval.node = MakeNode(NODE_OP_PLUS); return tok = PLUS;}
"-"                              {       yylval.node = MakeNode(NODE_OP_MINUS); return tok = MINUS;}
"*"                              {       yylval.node = MakeNode(NODE_OP_MUL); return tok = MUL;}
"/"                              {       yylval.node = MakeNode(NODE_OP_DIV); return tok = DIV;}
"&&"                             {       yylval.node = MakeNode(NODE_OP_AND); return tok = AND; }
"||"                             {       yylval.node = MakeNode(NODE_OP_OR); return tok = OR; }
".."                             return tok = DOTDOT;

[-+]?[0-9]+              {
             int i;
             if (tok == RPAREN || tok == IDENTIFIER || tok == NUM || tok == RBRAC)
             {
                 if (yytext[0] == '-')
                 {
                     for (i = strlen(yytext) - 1; i >= 1; i--) unput(yytext[i]);
                     yytext[1] = '\0';
                     yylval.node = MakeNode(NODE_OP_MINUS);
                     return tok = MINUS;
                 }
                 if (yytext[0] == '+')
                 {
                     for (i = strlen(yytext) - 1; i >= 1; i--) unput(yytext[i]);
                     yytext[1] = '\0';
                     yylval.node = MakeNode(NODE_OP_PLUS);
                     return tok = PLUS;
                 }
             }
             yylval.node = MakeNode(NODE_INT);
             yylval.node->number = atof(yytext); 
             return tok = NUM;
}
[-+]?[0-9]+(E[+-]?[0-9]+)?|[-+]?[0-9]+"."[0-9]+(E[+-]?[0-9]+)?    { 
         int i;
             if (tok == RPAREN || tok == IDENTIFIER || tok == NUM || tok == RBRAC)
             {
                 if (yytext[0] == '-')
                 {
                     for (i = strlen(yytext) - 1; i >= 1; i--) unput(yytext[i]);
                     yytext[1] = '\0';
                     yylval.node = MakeNode(NODE_OP_MINUS);
                     return tok = MINUS;
                 }
                 if (yytext[0] == '+')
                 {
                     for (i = strlen(yytext) - 1; i >= 1; i--) unput(yytext[i]);
                     yytext[1] = '\0';
                     yylval.node = MakeNode(NODE_OP_PLUS);
                     return tok = PLUS;
                 }
             }     
        yylval.node = MakeNode(NODE_REAL); 
        yylval.node->number = atof(yytext);
        return tok = NUM;
}
\n                               {line_no++;}
\"      {
          int c;
          buff[0] = '"';
          int i = 1;
          while ((c = input()))
          {
	      if (i >= BUFFERSIZE - 1)
              {
                  buff[i] = '\0';
                  fprintf(stderr, "scanner detects long string at line %d\n", line_no);
                  return 0;
              }	
              buff[i++] = c;
              if (c == '"')
              {
		          buff[i] = '\0';
		          yytext = buff;
                          yylval.node = MakeNode(NODE_STRING); 
                          yylval.node->stringconst = strdup(yytext);
		          return STRINGCONST;
              }
              else if (c == '\n')
              {
                  buff[--i] = '\0';
                  line_no++;
                  fprintf(stderr, "scanner detects cross string at line %d\n", line_no);
                  return 0;
              }
              else if (c == '\\')
              {
                  i--;
		          if ((c = input()) == 'n') {
                      buff[i++] = '\n';
                  } else if (c == 't') {
                      buff[i++] = '\t';
                  } else {
		              buff[i++] = c;
                  }
              }
          }
	}
"/*"    { int c;
          char buff[BUFFERSIZE];
          buff[0] = '/';
          buff[1] = '*';
          int i = 2;
          while ((c = input()))
          {
              if (i >= BUFFERSIZE - 1)
              {
                  buff[i] = '\0';
                  fprintf(stderr, "scanner detects long comment at line %d\n", line_no);
                  return 0;
              }	
              buff[i++] = c;
              if (c == '*')
              {
                  if ((c = input()) == '/')
                  {
                      break;
                  } else {
                      unput(c);
                  }
              }
              else if (c == '\n')
                  line_no++;
          }
        }

[ \t\f]    ;
\r         ;
.    {fprintf(stderr, "scanner detects unknown token at line %d\n", line_no); return 0;}
%%

