#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stlink st_head = NULL;
stlink st_tail = NULL;
stlink st_opened = NULL;
int semanticErrorCount = 0;

char *displayString[4] =
{
    "integer",
    "real",
    "string",
    "none"
};

void addEntry(attribute data, stlink st) {
    link p_node = malloc(sizeof *p_node);
    memcpy(&p_node->attr, &data, sizeof(attribute));
    if(p_node) {
    	link p_current = st->p_head;
    	if (st->p_head == NULL) {
	        p_node->prev = NULL;
	        p_node->next = NULL;
	        st->p_head = p_node;
                st->p_tail = p_node;
    	} else {
      		while(p_current->next!=NULL) {
        	    p_current = p_current->next;
      		}
		    p_current->next = p_node;
                    st->p_tail = p_node;
		    p_node->next = NULL;
		    p_node->prev = p_current;
    	}
        //printf("Add new symbol \'%s\' (%s) to symbol table\n", data.name, displayString[data.type]);
    } else {
        printf("Memory allocation error!\n");
    }
}

link searchEntry(char *string, stlink st)
{
    stlink temp = st;
    while (temp != NULL) {
        link p_current = temp->p_head;
        if (p_current) {
    	    while (p_current != NULL) {
    		if (strcmp(p_current->attr.name, string) == 0)
                {
                    return p_current;
                } else {
    		    p_current = p_current->next;
    		}
            }
    	}
        temp = temp->next;
    }
    return NULL;
}

link checkEntry(char *string, stlink st)
{
    link p_current = st->p_head;
    int num = 0;
    if (p_current) {
    	while (p_current != NULL) {
            if (strcmp(p_current->attr.name, string) == 0)
            {
                p_current->index = num;
                return p_current;
            } else {
    		    p_current = p_current->next;
                num++;
            }
        }
    }
    return NULL;
}

enum StdType getEntryType(char *string, stlink st)
{
    stlink temp = st;
    while (temp != NULL) {
        link p_current = temp->p_tail;
        if (p_current) {
    	    while (p_current != NULL) {
    		if (strcmp(p_current->attr.name, string) == 0)
                {
                    return p_current->attr.type;
                } else {
    		    p_current = p_current->prev;
    		}
            }
    	}
        temp = temp->next;
    }
    return TypeNone;
}

void displayEntry(stlink st) {
    if (st == NULL) {
        printf("The symbol table is empty!\n");
        return;
    }
    
    link p_current =  st->p_head;
    if (p_current) {
        while(p_current != NULL) {
            printf("(%s, %s)", p_current->attr.name, displayString[p_current->attr.type]);
            if (p_current->next != NULL) printf(" -> ");
	    else printf("\n");
            p_current = p_current->next;
        }
    } else {
        printf("The doubly linked list is empty!\n");
    }
}

#include "node.h"
enum StdType convertStdType(NodeType nodeType) {
    switch (nodeType) {
        case NODE_TYPEINT:
        case NODE_INT:
            return TypeInt;
        case NODE_TYPEREAL:
        case NODE_REAL:
            return TypeReal;
        case NODE_TYPESTRING:
        case NODE_STRING:
            return TypeString;
        default:
            return TypeNone;
    }
} 

int isOperatorType(NodeType nodeType) {
     switch(nodeType) {
         case NODE_OP_LT:
         case NODE_OP_GT:
         case NODE_OP_LE:
         case NODE_OP_GE:
         case NODE_OP_EQ:
         case NODE_OP_NE:
         case NODE_OP_PLUS:
         case NODE_OP_MINUS:
         case NODE_OP_MUL:
         case NODE_OP_DIV:
             return 1;
         default:
             return 0;
     }
}

void indexCheck(struct Node *node, char *name) {
    while(node) {
        link p_temp = NULL; 
        if (node->id) p_temp = searchEntry(node->id, st_opened); 
        if (p_temp != NULL) { 
            if (p_temp->attr.type != TypeInt) {
                printf("Error: array subscript is not an integer for \'%s\'\n", name);
                semanticErrorCount++;
                return;
            } 
        } else if (node->nodeType == NODE_CALL) {
            p_temp = searchEntry(node->leftmostChild->id, st_opened);
            if (p_temp->attr.type != TypeInt) {
                printf("Error: array subscript is not an integer for \'%s\'\n", name);
                semanticErrorCount++;
                return;
            } 
        } else {
            if (isOperatorType(node->nodeType)) {
                indexCheck(node->leftmostChild, name); 
            } else {
                if (node->nodeType != NODE_INT) {
                   printf("Error: array subscript is not an integer for \'%s\'\n", name);
                   semanticErrorCount++;
                   return;
                }
            }
        }             
        node = node->rightSibling;
    }    
}

int findParasIndexNum(struct Node *node) {
    struct Node *children = node->leftmostChild;
    int num = 0;
    while (children) {
        num++;
	children = children->rightSibling;
    }
    return num;
}

enum StdType errorType;
int errorFlag;
void typeCheck(struct Node *node) {
    link p_temp = NULL; 
    link p_temp1 = NULL;    
    struct Node *temp;
    struct Node *temp1;
    enum StdType stdType;
    int dim = 0;
    int dim1 = 0;

    if (node->id) p_temp = searchEntry(node->id, st_opened); 
    if (p_temp != NULL) { 
        stdType = p_temp->attr.type;
        if (stdType == TypeNone) {
            printf("Error: procedure call \'%s\' doesn\'t have return value\n", node->id);
            semanticErrorCount++;
            return;
        }
    } else {
        stdType = convertStdType(node->nodeType);
    }

    if (errorType != TypeNone && stdType != TypeNone && stdType != errorType) {
        errorFlag = 1;
        return;
    }
    if (!isOperatorType(node->nodeType))
        errorType = stdType;
    if (node->nodeType != NODE_CALL && node->id == NULL) {
        if (node->leftmostChild != NULL) {
            temp = node->leftmostChild;
	    while (temp != NULL) {
                typeCheck(temp);
                temp = temp->rightSibling;
	    }
        }
    } else if (node->nodeType == NODE_CALL) {
        p_temp = searchEntry(node->leftmostChild->id, st_opened);
        if (p_temp) { 
            if (p_temp->attr.funcType == TypeProc) {
                printf("Error: procedure call \'%s\' doesn\'t have return value\n", node->leftmostChild->id);
                semanticErrorCount++;
                return;
            }
            if (p_temp->attr.funcType == TypeFunc) {
                typeCheck(node->leftmostChild);
            }
        } else {
            printf("Error: Undeclared symbol: \'%s\'\n", node->leftmostChild->id);
            semanticErrorCount++;
            return;
        } 
    } 
}

void semanticCheck(struct Node *node, enum SemanticCheckType checkType)
{
    int parasCount = 0;
    struct Node *temp;
    struct Node *temp1;
    enum StdType stdType;
    link p_temp = NULL;
    link p_temp1 = NULL;
    int i, dim1;
    switch (checkType) {
        case FUNCTION_CALL:
            p_temp = searchEntry(node->id, st_opened); 
            if (p_temp->attr.funcType != TypeFunc && p_temp->attr.funcType != TypeProc) {
                printf("Error: called object \'%s\' is not a function\n", node->id);
                semanticErrorCount++;
                return;
            }
            i = 0;
            temp = node->rightSibling;
            while (temp) {
                parasCount++;
                temp = temp->rightSibling;
            }
            if (parasCount < p_temp->attr.parasNum) {
                printf("Error: too few arguments to function \'%s\'\n", p_temp->attr.name);
                semanticErrorCount++;
                return;
            } else if (parasCount > p_temp->attr.parasNum) {
                printf("Error: too many arguments to function \'%s\'\n", p_temp->attr.name);
                semanticErrorCount++;
                return;
            } else {
                temp = node->rightSibling;
                while (temp) {
                    if (temp->id) {
                        temp1 = temp->leftmostChild;
                        p_temp1 = searchEntry(temp->id, st_opened); 
                        //printf("%s(%s, %d, %d)\n", p_temp->attr.name, temp->id, p_temp1->attr.arrayDim, findParasIndexNum(temp));
		        if (getEntryType(temp->id, st_opened) != p_temp->attr.paras[i].type) {
                            printf("Error: incompatible type for argument %d of \'%s\'\n", i+1, p_temp->attr.name);
                            semanticErrorCount++;
                            return;
                        }
                        if (p_temp1->attr.arrayDim - findParasIndexNum(temp) != p_temp->attr.paras[i].arrayDim) {
                            printf("Error: incompatible type for argument %d of \'%s\'\n", i+1, p_temp->attr.name);
                            semanticErrorCount++;
                            return;
                        }
                    } else if (isOperatorType(temp->nodeType)) {
                        errorFlag = 0;
                        errorType = TypeNone;
                        typeCheck(temp);
                        if (errorType != p_temp->attr.paras[i].type) {
                            printf("Error: incompatible type for argument %d of \'%s\'\n", i+1, p_temp->attr.name);
                            semanticErrorCount++;
                            return;
                        }
                    } else if (temp->nodeType == NODE_CALL) {
                        p_temp1 = searchEntry(temp->leftmostChild->id, st_opened); 
                        if (p_temp1->attr.type != p_temp->attr.paras[i].type || p_temp->attr.paras[i].arrayDim != 0) {
                            printf("Error: incompatible type for argument %d of \'%s\'\n", i+1, p_temp->attr.name);
                            semanticErrorCount++;
                            return;
                        }
                    } else {
                        //printf("%s(%d)\n", p_temp->attr.name, (int)temp->number);
                        if (convertStdType(temp->nodeType) != p_temp->attr.paras[i].type || p_temp->attr.paras[i].arrayDim != 0) {
                            printf("Error: incompatible type for argument %d of \'%s\'\n", i+1, p_temp->attr.name);
                            semanticErrorCount++;
                            return;
                        }
                    }
		    temp = temp->rightSibling;
                    i++;
	        }
            }
            break;
        case OP_CALL:
        case ASSIGN_CALL:
            errorFlag = 0;
            errorType = TypeNone;
            typeCheck(node);
            if (errorFlag) {
                 printf("Error: incompatible type for operation or assignment\n");
                 semanticErrorCount++;
            }
            if (checkType == OP_CALL) {
                if (errorFlag == 0 && errorType == TypeString) {
                    printf("Error: operation does not support string type\n");
                    semanticErrorCount++;
                }
            }
            break;
        case ID_CALL:
            i = 0, dim1 = 0;
            temp = node->leftmostChild;
            p_temp = searchEntry(node->id, st_opened);
            while (temp) {
                if (temp->id) {
                    p_temp1 = searchEntry(temp->id, st_opened);
                    if (p_temp1->attr.type != TypeInt && p_temp1->attr.arrayDim == 0) {
                        printf("Error: array subscript is not an integer for \'%s\'\n", node->id);
                        semanticErrorCount++;
                        return;
                    } else if (p_temp1->attr.arrayDim != 0) {
                        temp1 = temp->leftmostChild;
                        while(temp1) {
                            dim1++;
                            temp1 = temp1->rightSibling;
                        }
                        if (dim1 != p_temp1->attr.arrayDim) {
                            printf("Error: array subscript is not an integer for \'%s\'\n", node->id);
                            semanticErrorCount++;
                            return;
                        }
                    }
                } else {
                    if (node->parent->nodeType != NODE_FOR) {
                        if (temp->nodeType == NODE_CALL) {
                            p_temp1 = searchEntry(temp->leftmostChild->id, st_opened);
                            if (p_temp1->attr.type != TypeInt) {
                                printf("Error: array subscript is not an integer for \'%s\'\n", node->id);
                                semanticErrorCount++;
                                return;
                            }
                        } else if (isOperatorType(temp->nodeType)) {
                            indexCheck(temp->leftmostChild, node->id); 
                        } else if (temp->nodeType != NODE_INT) {
                            printf("Error: array subscript is not an integer for \'%s\'\n", node->id);
                            semanticErrorCount++;
                            return;
                        } else if (temp->nodeType == NODE_INT && temp->number < 0) {
                            printf("Error: array subscript is not a positive integer for \'%s\'\n", node->id);
                            semanticErrorCount++;
                            return;
                        } else if (temp->nodeType == NODE_INT && (temp->number > p_temp->attr.upperIndex[i] || temp->number < p_temp->attr.lowerIndex[i])) {
                            printf("Error: array subscript is out of bound for \'%s\'\n", node->id);
                            semanticErrorCount++;
                            return;
                        }
                    }
                }
                temp = temp->rightSibling;
                i++;
            }
            //p_temp = searchEntry(node->id, st_opened); 
          
            if (node->parent->nodeType != NODE_FOR && i != p_temp->attr.arrayDim) {
                printf("Error: dimension of array \'%s\' is not matched\n", node->id);
                semanticErrorCount++;
                return;
            }
            break;
    }
}

enum StdType findStdType(struct Node *node) {
    struct Node *last = node;
    while (last->rightSibling != NULL)
	last = last->rightSibling;
    return  convertStdType(last->nodeType); 
}

int findArrayDim(struct Node *node, int lowerIndex[DIM_NUM], int upperIndex[DIM_NUM]) {
    struct Node *last = node;
    int dim = 0;
    while (last != NULL) {
        if (last->nodeType == NODE_ARRAY) {
            lowerIndex[dim] = (int)last->leftmostChild->number;
            upperIndex[dim] = (int)last->leftmostChild->rightSibling->number;
            dim++; 
        }
	    last = last->rightSibling;
    }
    return dim;
}

void MakeSymtab(void) {
    stlink p_st = malloc(sizeof *p_st);
    if(p_st) {
        p_st->p_head = NULL;
        p_st->p_tail = NULL;
    	if (st_opened == NULL) {
	    p_st->next = NULL;
        p_st->index = 0;
	    st_head = p_st;
            st_opened = p_st;
            st_tail = p_st;
    	} else {
            if (st_head == st_opened) {
                p_st->index = st_opened->index + 1;
                p_st->next = st_opened;
                st_head = p_st;
                st_opened = p_st;
            } else {
                stlink p_current = st_head;
                while (p_current->next != st_opened)
                    p_current = p_current->next;
                p_st->index = p_current->index + 1;
                p_st->next = p_current->next;
                p_current->next = p_st;
                st_opened = p_st;
            }
    	}
        //printf("A new symbol table is created...\n");
    } else {
        printf("Memory allocation error!\n");
    }
}

void OpenScope(void) {
    //printf("New scope is opened...\n");
    MakeSymtab();
}

void CloseScope(void) {
    //printf("The scope is closed...\n");
    if (st_opened->next != NULL)
        st_opened = st_opened->next;
}

void importPrintLib(void) {
    attribute data;

    data.name = "printInt";
    data.type = TypeNone;   
    data.funcType = TypeProc;
    data.arrayDim = 0;
    data.paras[0].arrayDim = 0;
    data.paras[0].type = TypeInt;	            
    data.parasNum = 1;
    addEntry(data, st_tail);   

    data.name = "printlnInt";
    data.paras[0].type = TypeInt;
    addEntry(data, st_tail); 
    data.name = "printReal";
    data.paras[0].type = TypeReal;
    addEntry(data, st_tail); 
    data.name = "printlnReal";
    data.paras[0].type = TypeReal;
    addEntry(data, st_tail); 
    data.name = "printString";
    data.paras[0].type = TypeString;
    addEntry(data, st_tail);  
    data.name = "printlnString";
    data.paras[0].type = TypeString;
    addEntry(data, st_tail);  
    data.name = "readInt";
    data.type = TypeInt;   
    data.funcType = TypeFunc;
    data.paras[0].type = TypeNone;
    addEntry(data, st_tail); 
}

void processNode(struct Node *node) {

    struct Node * children = node->leftmostChild;
    struct Node *temp;
    link p_temp = NULL;
    enum StdType stdType;
    int count = 0;
    int error = 0;
    int dimension;
    attribute data;
		switch(node->nodeType) {
		    case NODE_FUNC:
		    case NODE_PROC:
		        OpenScope(); 
		        /* start to get return type */
		        if (children->id) {
		            data.name = strdup(children->id);
		            data.type = TypeNone;   // procedure has no return value
		            data.funcType = TypeProc;
		        }
		        if (node->nodeType == NODE_FUNC) { // function should have return value
		            temp = children->rightSibling;
		            while (temp->nodeType != NODE_TYPEINT && temp->nodeType != NODE_TYPEREAL && temp->nodeType != NODE_TYPESTRING) 
                                temp = temp->rightSibling;
		            data.type = convertStdType(temp->nodeType); 
		            data.funcType = TypeFunc;
                            data.arrayDim = 0;
		        }
		        /* end of getting return type */
		        /* start to get parameters */
		        count = 0;
		        while (children) {
		            if (children->nodeType == NODE_PARAS)
		            {
		                temp = children->leftmostChild;
		                stdType = findStdType(temp);
		                while (temp->rightSibling != NULL) {
				            if (temp->id) {
                                dimension = findArrayDim(temp, data.paras[count].lowerIndex, data.paras[count].upperIndex);
                                /*if (dimension != 0) {
                                    int i;
                                    for (i = 0; i < dimension; i++)
                                        printf("%d: (%d, %d)\n", i, data.paras[count].lowerIndex[i], data.paras[count].upperIndex[i]);
                                }*/
				                data.paras[count].name = strdup(temp->id);
                                data.paras[count].arrayDim = dimension;
		                        data.paras[count++].type = stdType;
				            }
		                    temp = temp->rightSibling;
				        }
		            }
		            children = children->rightSibling;
		        }
		        data.parasNum = count;
		        /* end of getting parameters */
			    addEntry(data, st_tail);   // add information to symbol table
		        /* print information */
		        int i;
		        //printf("Declare function %s (return type: %s): ", data.name, displayString[data.type]);
		        //for (i = 0; i < count; i++) {
		        //    if (data.paras[i].name)
		        //        printf(" (%s, %s, %d)", data.paras[i].name, displayString[data.paras[i].type], data.paras[i].arrayDim); 
		        //}
		        //printf("\n");
		        break;
		    case NODE_VAR_DECL:
		    case NODE_PARAS:
		        /* put declared variables into symbol table */
		        stdType = findStdType(children);
		        dimension = findArrayDim(children, data.lowerIndex, data.upperIndex);
		        while (children->rightSibling != NULL) {
		            if (children->id) {
		                if (checkEntry(children->id, st_opened) != NULL) {
		                    printf("Error: redeclaration of \'%s\'\n", children->id);
                                    semanticErrorCount++;
		                } else {
		                    data.name = strdup(children->id);
		                    data.type = stdType;
		                    data.funcType = TypeVar;
		                    data.arrayDim = dimension;
                            /*if (dimension != 0) {
                                int i;
                                for (i = 0; i < dimension; i++)
                                    printf("%d: (%d, %d)\n", i, data.lowerIndex[i], data.upperIndex[i]);
                            }*/
				            addEntry(data, st_opened);
		                }
		            }
				children = children->rightSibling;
			    }
		        break;
		    case NODE_CALL:
		        /* when call function or procedure, check if the function name and parameters are declared */
		        error = 0;
		        while (children) {
		            if (children->id) {
		                if (searchEntry(children->id, st_opened) == NULL) {
		                    printf("Error: Undeclared symbol: \'%s\'\n", children->id);
                                    semanticErrorCount++;
		                    error++;
		                } else {
		                    //printf("Find declared symbol: %s (%s)\n", children->id, displayString[getEntryType(children->id, st_opened)]);
		                }
		            }
		            children = children->rightSibling;
		        }
		        if (error == 0) /* if symbol is not declared, don't do semantic check */
		            semanticCheck(node->leftmostChild, FUNCTION_CALL);
		        break;
		    case NODE_OP_PLUS:
		    case NODE_OP_MINUS:
		    case NODE_OP_MUL:
		    case NODE_OP_DIV:
		    case NODE_OP_LT:
		    case NODE_OP_GT:
		    case NODE_OP_LE:
		    case NODE_OP_GE:
		    case NODE_OP_EQ:
		    case NODE_OP_NE:
                    case NODE_ASSIGN:
		        error = 0;
		        while (children) {
		            if (children->id) {
		                if ((searchEntry(children->id, st_opened)) == NULL) {
		                    printf("Error: Undeclared symbol: \'%s\'\n", children->id);
                                    semanticErrorCount++;
		                    error++;
		                } else {
		                    //printf("Find declared symbol: %s (%s)\n", children->id, displayString[getEntryType(children->id, st_opened)]);
		                }
		            }
		            children = children->rightSibling;
		        }
		        if (error == 0) { /* if symbol is not declared, don't do semantic check */
                            if (node->nodeType == NODE_ASSIGN) semanticCheck(node, ASSIGN_CALL);
		            else semanticCheck(node, OP_CALL);
		        }
		        break;
                    case NODE_ID:
                            p_temp = searchEntry(node->id, st_opened);
                            if (p_temp && p_temp->attr.funcType == TypeVar && node->parent->nodeType != NODE_CALL
                                 && node->parent->nodeType != NODE_VAR_DECL && node->parent->nodeType != NODE_PARAS) {
                                semanticCheck(node, ID_CALL);
                            } 
                        break;
		}

		if (node->leftmostChild != NULL) {
		    struct Node * children = node->leftmostChild;
		    while (children != NULL) {
		        processNode(children);
		        children = children->rightSibling;
		    }
		}

		if (node->nodeType == NODE_FUNC || node->nodeType == NODE_PROC) {
		    //displayEntry(st_opened);
		    CloseScope();
		}
}

