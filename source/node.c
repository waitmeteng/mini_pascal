#include <stdlib.h>
#include <stdio.h>
#include "node.h"

struct Node* MakeNode(NodeType t)
{
    struct Node *node = malloc(sizeof(struct Node));
    node->nodeType = t;
    node->id = NULL;
    node->stringconst = NULL;
    node->number = 0;
    //node->visited = 0;
    node->parent = NULL;
    node->leftmostSibling = node;
    node->leftmostChild = NULL;
    node->rightSibling = NULL;
    return node;
}

struct Node* CopyNode(struct Node* orig)
{
    struct Node *node = MakeNode(orig->nodeType);
    node->leftmostChild = orig->leftmostChild;
    return node;
}

void DeleteNode(struct Node* node)
{
    if (node != NULL)
    {
        if (node->id != NULL)
            free(node->id);
        if (node->stringconst != NULL)
            free(node->stringconst);
        free(node);
    }
}

void MakeSiblings(struct Node* x, struct Node* y)
{
    if (x == NULL || y == NULL) {
        fprintf(stderr, "%s, %d\n", __func__, __LINE__);
        return;
    }
    /* Find the rightmost Node in x list */
    struct Node* xsibs = x;
    while (xsibs->rightSibling != NULL) xsibs = xsibs->rightSibling;
    /* Join the lists */
    struct Node* ysibs = y->leftmostSibling;
    xsibs->rightSibling = ysibs;
    /* Set pointers for the new siblings */
    ysibs->leftmostSibling = xsibs->leftmostSibling;
    ysibs->parent = xsibs->parent;
    while (ysibs->rightSibling != NULL)
    {
        ysibs = ysibs->rightSibling;
        ysibs->leftmostSibling = xsibs->leftmostSibling;
        ysibs->parent = xsibs->parent;
    }
}

void AdoptChildren(struct Node* x, struct Node* y)
{
    if (x == NULL || y == NULL) {
        fprintf(stderr, "%s, %d\n", __func__, __LINE__);
        return;
    }

    if (x->leftmostChild != NULL)
        MakeSiblings(x->leftmostChild, y);
    else 
    {
        struct Node* ysibs = y->leftmostSibling;
        x->leftmostChild = ysibs;
        while (ysibs != NULL)
        {
            ysibs->parent = x;
            ysibs = ysibs->rightSibling;
        }
    }
}

void MakeFamily(struct Node* father, struct Node* kid1, struct Node* kid2)
{
    if (father == NULL || kid1 == NULL || kid2 == NULL) {
        fprintf(stderr, "%s, %d\n", __func__, __LINE__);
        return;
    }
    MakeSiblings(kid1, kid2);
    AdoptChildren(father, kid1);
}

void PrintTree(struct Node* father, int indent)
{
    if (father == NULL) {
        fprintf(stderr, "%s, %d\n", __func__, __LINE__);
        return;
    }
    static char blank[1024];
    int i;
    for(i=0; i<indent; i++)
        blank[i] = ' ';
    blank[indent] = '\0';

    switch(father->nodeType) {
        case NODE_IF:
            printf("%sIF\n", blank);
            indent++;
            break;
        case NODE_NOT:
            printf("%sNOT\n", blank);
            indent++;
            break;
        case NODE_WHILE:
            printf("%sWHILE\n", blank);
            indent++;
            break;
        case NODE_REPEAT:
            printf("%sREPEAT\n", blank);
            indent++;
            break;
        case NODE_FOR:
            printf("%sFOR\n", blank);
            indent++;
            break;
        case NODE_ID:
            printf("%s%s\n", blank, father->id);
            indent++;
            break;
        case NODE_ASSIGN:
            printf("%s:=\n", blank);
            indent++;
            break;
        case NODE_OP_PLUS:
            printf("%s+\n", blank);
            indent++;
            break;
        case NODE_OP_MINUS:
            printf("%s-\n", blank);
            indent++;
            break;
        case NODE_OP_MUL:
            printf("%s*\n", blank);
            indent++;
            break;
        case NODE_OP_DIV:
            printf("%s/\n", blank);
            indent++;
            break;
        case NODE_OP_GT:
            printf("%s>\n", blank);
            indent++;
            break;
        case NODE_OP_LT:
            printf("%s<\n", blank);
            indent++;
            break;
        case NODE_OP_EQ:
            printf("%s=\n", blank);
            indent++;
            break;
        case NODE_OP_GE:
            printf("%s>=\n", blank);
            indent++;
            break;
        case NODE_OP_LE:
            printf("%s<=\n", blank);
            indent++;
            break;
        case NODE_OP_NE:
            printf("%s!=\n", blank);
            indent++;
            break;
        case NODE_OP_AND:
            printf("%s&&\n", blank);
            indent++;
            break;
        case NODE_OP_OR:
            printf("%s||\n", blank);
            indent++;
            break;
        case NODE_INT:
            printf("%s%d\n", blank, (int)father->number);
            break;
        case NODE_REAL:
            printf("%s%f\n", blank, father->number);
            break;
        case NODE_STRING:
            printf("%s%s\n", blank, father->stringconst);
            break;
        case NODE_BLOCK:
            printf("%sBLOCK\n", blank);
            indent++;
            break;
        case NODE_VAR_DECL:
            printf("%sVAR_DECL\n", blank);
            indent++;
            break;
        case NODE_TYPEINT:
            printf("%sINTEGER\n", blank);
            break;
        case NODE_TYPEREAL:
            printf("%sREAL\n", blank);
            break;
        case NODE_TYPESTRING:
            printf("%sSTRING\n", blank);
            break;
        case NODE_ARRAY:
            printf("%sARRAY\n", blank);
            indent++;
            break;
        case NODE_PARAS:
            printf("%sPARAMETERS\n", blank);
            indent++;
            break;
        case NODE_PROGRAM:
            printf("%sPROGRAM\n", blank);
            indent++;
            break;
        case NODE_FUNC:
            printf("%sFUNCTION\n", blank);
            indent++;
            break;
        case NODE_PROC:
            printf("%sPROCEDURE\n", blank);
            indent++;
            break;
        case NODE_CALL:
            printf("%sCALL\n", blank);
            indent++;
            break;
    }

    struct Node* children = father->leftmostChild;
    while (children != NULL)
    {
        do {
            PrintTree(children, indent);
            children = children->rightSibling;
        } while (children != NULL);
    }
}
