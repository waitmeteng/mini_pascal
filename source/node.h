#ifndef __NODE_H__
#define __NODE_H__

typedef enum 
{
    NODE_BEGIN,
    NODE_END,
    NODE_IF,
    NODE_NOT,
    NODE_WHILE,
    NODE_REPEAT,
    NODE_FOR,
    NODE_ID,
    NODE_ASSIGN,
    NODE_OP_LT,
    NODE_OP_GT,
    NODE_OP_LE,
    NODE_OP_GE,
    NODE_OP_EQ,
    NODE_OP_NE,
    NODE_OP_PLUS,
    NODE_OP_MINUS,
    NODE_OP_MUL,
    NODE_OP_DIV,
    NODE_OP_AND,
    NODE_OP_OR,
    NODE_INT,
    NODE_REAL,
    NODE_STRING,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_TYPEINT,
    NODE_TYPEREAL,
    NODE_TYPESTRING,
    NODE_PROGRAM,
    NODE_FUNC,
    NODE_PROC,
    NODE_ARRAY,
    NODE_PARAS,
    NODE_CALL,
    NODE_DOTDOT,
    NODE_NULL,

    NODE_TOTAL_NUM
}NodeType;

struct Node
{
    NodeType nodeType;
    char *id;
    char *stringconst;
    double number;
    struct Node *parent;
    struct Node *leftmostSibling;
    struct Node *leftmostChild;
    struct Node *rightSibling;
};

struct Node* MakeNode(NodeType t);
struct Node* CopyNode(struct Node* orig);
void DeleteNode(struct Node* node);
void MakeSiblings(struct Node* x, struct Node* y);
void AdoptChildren(struct Node* x, struct Node* y);
void MakeFamily(struct Node* father, struct Node* kid1, struct Node* kid2);
void PrintTree(struct Node* father, int indent);
#endif
