#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#define DIM_NUM 10

enum StdType {TypeInt, TypeReal, TypeString, TypeNone};
enum FuncType {TypeFunc, TypeProc, TypeVar};
enum SemanticCheckType {FUNCTION_CALL, OP_CALL, ASSIGN_CALL, ID_CALL};
struct Parameters
{
    char *name;
    enum StdType type;
    int arrayDim;
    int upperIndex[DIM_NUM];
    int lowerIndex[DIM_NUM];
};
typedef struct Attribute
{
    char* name;
    enum StdType type;
    enum FuncType funcType;
    int arrayDim;
    int upperIndex[DIM_NUM];
    int lowerIndex[DIM_NUM];
#define PARAS_NUM 10
    int parasNum;
    struct Parameters paras[PARAS_NUM];
}attribute;

typedef struct entry *link;
struct entry {
    attribute attr;
    int index;
    link next;
    link prev;
};


typedef struct symtab *stlink;

struct symtab {
    link p_head;
    link p_tail;
    int index;
    stlink next;
};

void displayEntry(stlink st);
void processNode(struct Node *node);
void MakeSymtab(void);
void importPrintLib(void);
link searchEntry(char *string, stlink st);
link checkEntry(char *string, stlink st);
int isOperatorType(NodeType nodeType);
#endif


