#ifndef __CODEGENE_H__
#define __CODEGENE_H__

#include "node.h"
#include "symtab.h"
extern stlink st_head;
extern stlink st_opened;
extern stlink st_tail;
int isArithOpers(struct Node *node);
void codeGenerateMain(struct Node *node, char output[]);
void processArithmeticOp(FILE *pFile, struct Node *root, stlink st, int insideFunc, int isReal, int operationStackTop, int operationStackBottom);
void processBlock(FILE *pFile, struct Node *startNode, stlink st, int insideFunc);
void processCall(FILE *pFile, struct Node *node, stlink st, int insideFunc, int isFunc);
void functionCall(FILE *pFile, struct Node *proc, int isFunc, stlink st, int insideFunc, int operationStackTop);
void processIf(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label);
void processWhile(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label, int isfor);
void ifCondition(FILE *pFile, struct Node *cond, stlink st, int insideFunc, int label);
void ifThenBody(FILE *pFile, struct Node *thenNode, int isWhile, stlink st, int insideFunc, int label, int isfor);
void ifElseBody(FILE *pFile, struct Node *elseNode, stlink st, int insideFunc, int label);
#endif
