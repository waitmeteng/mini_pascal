#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "codegene.h"

char *className;
int current_label = 0;
int return_label = 0;

void buildStaticField(FILE *pFile) {
    link p_current =  st_tail->p_head;
    char str[256];
    int i;

    while(p_current != NULL) {

        if (p_current->attr.funcType == TypeVar) {
            memset(str, 0, sizeof str);
            for (i = 0; i < p_current->attr.arrayDim; i++)
                str[i] = '[';
            if (p_current->attr.type == TypeInt) {
                str[p_current->attr.arrayDim] = 'I';
                str[p_current->attr.arrayDim + 1] = '\0';
            }
            if (p_current->attr.type == TypeReal) {
                str[p_current->attr.arrayDim] = 'F';
                str[p_current->attr.arrayDim + 1] = '\0';
            }
            if (p_current->attr.type == TypeString) {
                strcpy(&str[p_current->attr.arrayDim], "Ljava/lang/String;");
            }
            
            fprintf(pFile, ".field public static %s %s\n", p_current->attr.name, str);
        }

        p_current = p_current->next;
    }
}

void initialStaticField (FILE *pFile) {
    link p_current =  st_tail->p_head;
    char str[256];
    int i;
    
    fprintf(pFile, "\n.method public static vinit()V\n");
    fprintf(pFile, "\t.limit locals 100\n");
    fprintf(pFile, "\t.limit stack 100\n");
    while(p_current != NULL) {

        if (p_current->attr.funcType == TypeVar) {
            memset(str, 0, sizeof str);
            if (p_current->attr.arrayDim == 0 && p_current->attr.type == TypeInt) {
                fprintf(pFile, "\tldc 0\n");
                fprintf(pFile, "\tputstatic %s/%s I\n", className, p_current->attr.name);
            } else if (p_current->attr.arrayDim == 0 && p_current->attr.type == TypeReal) {
                fprintf(pFile, "\tldc 0.0\n");
                fprintf(pFile, "\tputstatic %s/%s F\n", className, p_current->attr.name);
            } else if (p_current->attr.arrayDim == 0 && p_current->attr.type == TypeString) {
                fprintf(pFile, "\tldc \"\"\n");
                fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, p_current->attr.name);
            } else if (p_current->attr.arrayDim != 0 && p_current->attr.type == TypeInt) {
                for (i = 0; i < p_current->attr.arrayDim; i++) {
                    fprintf(pFile, "\tbipush %d\n", p_current->attr.upperIndex[i] - p_current->attr.lowerIndex[i] + 1);
                    str[i] = '[';
                }
                fprintf(pFile, "\tmultianewarray %sI %d\n", str, p_current->attr.arrayDim);
                fprintf(pFile, "\tputstatic %s/%s %sI\n", className, p_current->attr.name, str);
            } else {
                for (i = 0; i < p_current->attr.arrayDim; i++) {
                    fprintf(pFile, "\tbipush %d\n", p_current->attr.upperIndex[i] - p_current->attr.lowerIndex[i] + 1);
                    str[i] = '[';
                }
                fprintf(pFile, "\tmultianewarray %sF %d\n", str, p_current->attr.arrayDim);
                fprintf(pFile, "\tputstatic %s/%s %sF\n", className, p_current->attr.name, str);
            }
        }

        p_current = p_current->next;
    }
    fprintf(pFile, "\treturn\n");
    fprintf(pFile, ".end method\n\n");
    fprintf(pFile, "; standard initializer\n");
    fprintf(pFile, ".method public <init>()V\n");
    fprintf(pFile, "\taload_0\n");
    fprintf(pFile, "\tinvokenonvirtual java/lang/Object/<init>()V\n");
    fprintf(pFile, "\treturn\n");
    fprintf(pFile, ".end method\n\n");

}

void readInt(FILE *pFile, int label) {
    fprintf(pFile, "\tldc 0\n");
    fprintf(pFile, "\tistore 50\n");     
    fprintf(pFile, "L%d:\n", label);
    fprintf(pFile, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
    fprintf(pFile, "\tinvokevirtual java/io/InputStream/read()I\n");
    fprintf(pFile, "\tistore 51\n");
    fprintf(pFile, "\tiload 51\n");
    fprintf(pFile, "\tldc 10\n");
    fprintf(pFile, "\tisub\n");
    fprintf(pFile, "\tifeq L%d\n", label+1);
    fprintf(pFile, "\tiload 51\n");
    fprintf(pFile, "\tldc 32\n");
    fprintf(pFile, "\tisub\n");
    fprintf(pFile, "\tifeq L%d\n", label+1);
    fprintf(pFile, "\tiload 51\n");
    fprintf(pFile, "\tldc 48\n");
    fprintf(pFile, "\tisub\n");
    fprintf(pFile, "\tldc 10\n");
    fprintf(pFile, "\tiload 50\n");
    fprintf(pFile, "\timul\n");
    fprintf(pFile, "\tiadd\n");
    fprintf(pFile, "\tistore 50\n");
    fprintf(pFile, "\tgoto L%d\n", label);
    fprintf(pFile, "L%d:\n", label+1);
    fprintf(pFile, "\tiload 50\n");
    current_label += 2;
}

void printNumber(FILE *pFile, struct Node *paras, stlink st, int insideFunc, int isReal, int changeLine) {
    link p_temp = NULL;
    int i;
    char array_dim[16];
    struct Node *node;
    char c;
    char s[3];
    if (isReal) c = 'f';
    else c = 'i';
    if (changeLine) { s[0] = 'l'; s[1] = 'n'; s[2] = '\0';}
    else {s[0] = '\0';}

    if (paras->id) {
        p_temp = checkEntry(paras->id, st);
        if (insideFunc && p_temp) {
            if (p_temp->attr.arrayDim == 0) {
                fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
			    fprintf(pFile, "\t%cload %d\n", c, p_temp->index);
			    fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
			    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
            } else {
                fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
                fprintf(pFile, "\taload %d\n", p_temp->index);
                node = paras->leftmostChild;
                for (i = 0; i < p_temp->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                fprintf(pFile, "\t%caload\n", c);
                fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
			    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
            }
        } else {
            p_temp = searchEntry(paras->id, st_opened);
            if (p_temp->attr.arrayDim == 0) {
                fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
                if (p_temp->attr.funcType == TypeFunc) fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, paras->id, toupper(c));
			    else fprintf(pFile, "\tgetstatic %s/%s %c\n", className, paras->id, toupper(c));
			    fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
			    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
            } else {
                fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, paras->id, array_dim, toupper(c));
                node = paras->leftmostChild;
                for (i = 0; i < p_temp->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                fprintf(pFile, "\t%caload\n", c);
                fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
			    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
            }
        } 
    } else if (isArithOpers(paras)) {
        fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
        if (isReal) processArithmeticOp(pFile, paras, st, insideFunc, 1, 0, 0);
        else processArithmeticOp(pFile, paras, st, insideFunc, 0, 0, 0);
		fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
		fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
    } else if (paras->nodeType == NODE_CALL) {
        fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
        functionCall(pFile, paras->leftmostChild, 1, st, insideFunc, 0);  
        fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
		fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
    } else {
        fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
        if (isReal) fprintf(pFile, "\tldc %f\n", paras->number);
		else fprintf(pFile, "\tldc %d\n", (int)paras->number);
		fprintf(pFile, "\tinvokestatic java/lang/String/valueOf(%c)Ljava/lang/String;\n", toupper(c));
		fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
    }
}

void printString(FILE *pFile, struct Node *paras, stlink st, int insideFunc, int changeLine) {
    link p_temp = NULL;
    char s[3];
    if (changeLine) { s[0] = 'l'; s[1] = 'n'; s[2] = '\0';}
    else {s[0] = '\0';}
    if (paras->id) {
        p_temp = checkEntry(paras->id, st);
        if (insideFunc && p_temp) {
            fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
	    fprintf(pFile, "\taload %d\n", p_temp->index);
	    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
        } else {
            fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
	    fprintf(pFile, "\tgetstatic %s/%s Ljava/lang/String;\n", className, paras->id);
	    fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
        } 
    } else {
        fprintf(pFile, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
	fprintf(pFile, "\tldc %s\n", paras->stringconst);
	fprintf(pFile, "\tinvokevirtual java/io/PrintStream/print%s(Ljava/lang/String;)V\n", s);
    }
}

void functionCall(FILE *pFile, struct Node *proc, int isFunc, stlink st, int insideFunc, int stackTop) {
    struct Node *temp = proc;
    link p_temp = searchEntry(proc->id, st_opened);
    char paras[128];
    char ret[32];
    char array_dim[16];
    struct Node *node;
    int i, j = 0, k;
    for (i = 0; i < p_temp->attr.parasNum; i++) {
        for (k = 0; k < p_temp->attr.paras[i].arrayDim; k++) {
            paras[j++] = '[';
        }
        if (p_temp->attr.paras[i].type == TypeInt) paras[j++] = 'I';
        else if (p_temp->attr.paras[i].type == TypeReal) paras[j++] = 'F';
        else if (p_temp->attr.paras[i].type == TypeString) { strcpy(&paras[j], "Ljava/lang/String;"); j+=18; }
    }
    paras[j] = '\0';
    i = 0;
    while (temp) {
        if (i == 0) {
            if (temp->id) {
                p_temp = searchEntry(temp->id, st_opened);
                if (p_temp->attr.type == TypeInt)
                    strcpy(ret, "I");
                else if (p_temp->attr.type == TypeReal)
                    strcpy(ret, "F");
                else if (p_temp->attr.type == TypeString)
                    strcpy(ret, "Ljava/lang/String;");
            }
        } else {
            if (temp->id) {
                p_temp = checkEntry(temp->id, st);
                if (p_temp && insideFunc) {
                    if (p_temp->attr.type == TypeInt)
	                    fprintf(pFile, "\tiload %d\n", p_temp->index);
	                else if (p_temp->attr.type == TypeReal)
	                    fprintf(pFile, "\tfload %d\n", p_temp->index);
                } else {
		            p_temp = searchEntry(temp->id, st_opened);
		            if (p_temp->attr.arrayDim == 0) {
		                if (p_temp->attr.type == TypeInt)
		                    fprintf(pFile, "\tgetstatic %s/%s I\n", className, temp->id);
		                else if (p_temp->attr.type == TypeReal)
		                    fprintf(pFile, "\tgetstatic %s/%s F\n", className, temp->id);
		                else if (p_temp->attr.type == TypeString)
		                    fprintf(pFile, "\tgetstatic %s/%s Ljava/lang/String;\n", className, temp->id);
		            } else {
		                if (p_temp->attr.type == TypeInt) {
                            memset(array_dim, 0, sizeof(array_dim));
						    for (k = 0; k < p_temp->attr.arrayDim; k++) strcat(array_dim, "[");
						    fprintf(pFile, "\tgetstatic %s/%s %sI\n", className, temp->id, array_dim);
						    node = temp->leftmostChild;
                            if (node != NULL) {
								for (k = 0; k < p_temp->attr.arrayDim; k++) {
								    processArithmeticOp(pFile, node, st, insideFunc, 0, stackTop, stackTop);
								    fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[k]);
								    fprintf(pFile, "\tisub\n");
								    if (k != p_temp->attr.arrayDim - 1)
								        fprintf(pFile, "\taaload\n");
								    node = node->rightSibling;
								}
								fprintf(pFile, "\tiaload\n");
                            } 
		                } else if (p_temp->attr.type == TypeReal) {
		                    fprintf(pFile, "\tgetstatic %s/%s [F\n", className, temp->id);
                            processArithmeticOp(pFile, temp->leftmostChild, st, insideFunc, 0, stackTop, stackTop);
						    fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[0]);
						    fprintf(pFile, "\tisub\n");
						    fprintf(pFile, "\tfaload\n");
		                } else if (p_temp->attr.type == TypeString) {
		                    fprintf(pFile, "\tgetstatic %s/%s [Ljava/lang/String;\n", className, temp->id);
                        }
		            }
                }
            } else if (isArithOpers(temp)) {
                p_temp = searchEntry(proc->id, st_opened);
                if (p_temp->attr.paras[i-1].type == TypeInt) processArithmeticOp(pFile, temp, st, insideFunc, 0, stackTop, stackTop);
                else if (p_temp->attr.paras[i-1].type == TypeReal) processArithmeticOp(pFile, temp, st, insideFunc, 1, stackTop, stackTop);
            } else {
                if (temp->nodeType == NODE_INT) fprintf(pFile, "\tldc %d\n", (int)temp->number);
                else if (temp->nodeType == NODE_REAL) fprintf(pFile, "\tldc %f\n", temp->number);
                else if (temp->nodeType == NODE_STRING) fprintf(pFile, "\tldc %s\n", temp->stringconst);
                else if (temp->nodeType == NODE_CALL) {
                    processArithmeticOp(pFile, temp, st, insideFunc, 1, stackTop, stackTop);
                } 
            }
        }
        temp = temp->rightSibling;
        i++;
    }
    if (isFunc) fprintf(pFile, "\tinvokestatic %s/%s(%s)%s\n", className, proc->id, paras, ret);
    else fprintf(pFile, "\tinvokestatic %s/%s(%s)V\n", className, proc->id, paras);
}

int isArithOpers(struct Node *node) {
    if (node->nodeType == NODE_OP_PLUS ||
        node->nodeType == NODE_OP_MINUS ||
        node->nodeType == NODE_OP_MUL ||
        node->nodeType == NODE_OP_DIV) return 1;
    else return 0; 
}

struct Node* operationStack[128];
//int operationStackTop;

void processArithmeticOp(FILE *pFile, struct Node *root, stlink st, int insideFunc, int isReal, int operationStackTop, int operationStackBottom) {
    if (root == NULL) return;
    //operationStackTop = 0;
    struct Node* nodeStack[128];
    link p_temp1 = NULL;
    int stackTop = 0;
    int dim = 0;
    char c;
    if (isReal) c = 'f';
    else c = 'i';
    nodeStack[stackTop++] = root;

    while (stackTop != 0) {
        dim = 0;
        struct Node* node = nodeStack[--stackTop];
        operationStack[operationStackTop++] = node;
        if (node->id) {
            if (insideFunc) {
                p_temp1 = checkEntry(node->id, st);
                if (p_temp1 == NULL) p_temp1 = searchEntry(node->id, st_opened);
            } else {
                p_temp1 = searchEntry(node->id, st_opened);
            }
            dim = p_temp1->attr.arrayDim; 
        }
        if (node->nodeType != NODE_CALL && dim == 0 && node->leftmostChild) {
            nodeStack[stackTop++] = node->leftmostChild;
            nodeStack[stackTop++] = node->leftmostChild->rightSibling;
        }
    }    
                    
    operationStackTop--;
    while (operationStackTop >= operationStackBottom) {
        struct Node* node = operationStack[operationStackTop];
        if (isArithOpers(node)) {
            if (node->nodeType == NODE_OP_PLUS)
                fprintf(pFile, "\t%cadd\n", c);
            else if (node->nodeType == NODE_OP_MINUS)
                fprintf(pFile, "\t%csub\n", c);
            else if (node->nodeType == NODE_OP_MUL)
                fprintf(pFile, "\t%cmul\n", c);
            else 
                fprintf(pFile, "\t%cdiv\n", c);
        } else if (node->id) {
            if (insideFunc) {
                p_temp1 = checkEntry(node->id, st);
                if (p_temp1) {
                    if (p_temp1->attr.arrayDim == 0) {
                        fprintf(pFile, "\t%cload %d\n", c, p_temp1->index);
                    } else {
                        int i;
		                struct Node* mytemp;
		                fprintf(pFile, "\taload %d\n", p_temp1->index);
		                mytemp = node->leftmostChild;
		                for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                    processArithmeticOp(pFile, mytemp, st, insideFunc, 0, operationStackTop, operationStackTop);
		                    //fprintf(pFile, "\tldc %d\n", (int)mytemp->number);
		                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                    fprintf(pFile, "\tisub\n");
		                    if (i != p_temp1->attr.arrayDim - 1)
		                        fprintf(pFile, "\taaload\n");
		                    mytemp = mytemp->rightSibling;
		                }
		                fprintf(pFile, "\t%caload\n", c);    
                    }
                }
                else {
                    p_temp1 = searchEntry(node->id, st_opened);
                    if (p_temp1->attr.arrayDim == 0) {
		                if (p_temp1->attr.funcType == TypeFunc) {
		                    if (p_temp1->attr.type == TypeInt)
		                        fprintf(pFile, "\tinvokestatic %s/%s()I\n", className, node->id);
		                    else if (p_temp1->attr.type == TypeReal)
		                        fprintf(pFile, "\tinvokestatic %s/%s()F\n", className, node->id);
		                } else if (p_temp1->attr.funcType == TypeProc) {
		                    fprintf(pFile, "\tinvokestatic %s/%s()V\n", className, node->id);
		                } else {
		                    fprintf(pFile, "\tgetstatic %s/%s %c\n", className, node->id, toupper(c));
		                }
		            } else {
		                int i;
		                char array_dim[16];
		                struct Node* mytemp;
		                memset(array_dim, 0, sizeof(array_dim));
		                for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, node->id, array_dim, toupper(c));
		                mytemp = node->leftmostChild;
		                for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                    processArithmeticOp(pFile, mytemp, st, insideFunc, 0, operationStackTop, operationStackTop);
		                    //fprintf(pFile, "\tldc %d\n", (int)mytemp->number);
		                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                    fprintf(pFile, "\tisub\n");
		                    if (i != p_temp1->attr.arrayDim - 1)
		                        fprintf(pFile, "\taaload\n");
		                    mytemp = mytemp->rightSibling;
		                }
		                fprintf(pFile, "\t%caload\n", c);     
		            }
                }
            } else {
                p_temp1 = searchEntry(node->id, st_opened);
                if (p_temp1->attr.arrayDim == 0) {
                    if (p_temp1->attr.funcType == TypeFunc) {
                        if (p_temp1->attr.type == TypeInt)
                            fprintf(pFile, "\tinvokestatic %s/%s()I\n", className, node->id);
                        else if (p_temp1->attr.type == TypeReal)
                            fprintf(pFile, "\tinvokestatic %s/%s()F\n", className, node->id);
                    } else if (p_temp1->attr.funcType == TypeProc) {
                        fprintf(pFile, "\tinvokestatic %s/%s()V\n", className, node->id);
                    } else {
                        fprintf(pFile, "\tgetstatic %s/%s %c\n", className, node->id, toupper(c));
                    }
                } else {
                    int i;
                    char array_dim[16];
                    struct Node* mytemp;
                    memset(array_dim, 0, sizeof(array_dim));
                    for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
                    fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, node->id, array_dim, toupper(c));
                    mytemp = node->leftmostChild;
                    for (i = 0; i < p_temp1->attr.arrayDim; i++) {
                        processArithmeticOp(pFile, mytemp, st, insideFunc, 0, operationStackTop, operationStackTop);
                        //fprintf(pFile, "\tldc %d\n", (int)mytemp->number);
                        fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
                        fprintf(pFile, "\tisub\n");
                        if (i != p_temp1->attr.arrayDim - 1)
                            fprintf(pFile, "\taaload\n");
                        mytemp = mytemp->rightSibling;
                    }
                    fprintf(pFile, "\t%caload\n", c);     
                }
            }
        } else if (node->nodeType == NODE_CALL) {
            functionCall(pFile, node->leftmostChild, 1, st, insideFunc, operationStackTop);
        } else {
            if (isReal) fprintf(pFile, "\tldc %f\n", node->number);
            else fprintf(pFile, "\tldc %d\n", (int)node->number);
        }
        operationStackTop--;
    }
}

void assignNumber(FILE *pFile, struct Node *left, struct Node *right, stlink st, int insideFunc, int isReal) {
    link p_temp1 = NULL;
    link p_temp2 = NULL;
    int i;
    char array_dim[16];
    char c;
    struct Node *node;
    if (isReal) c = 'f';
    else c = 'i';

    if (insideFunc) {
        p_temp1 = checkEntry(left->id, st);               
        if (right->id) { /* RHS is a variable */
            p_temp2 = checkEntry(right->id, st);
            if (p_temp1 && p_temp2) { /* LHS and RHS are both local variables */
                if (p_temp1->attr.arrayDim == 0 && p_temp2->attr.arrayDim == 0) {
                    fprintf(pFile, "\t%cload %d\n", c, p_temp2->index);
                    fprintf(pFile, "\t%cstore %d\n", c, p_temp1->index);
                } else if (p_temp1->attr.arrayDim == 0) {
                    memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp2->index);
                    node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\t%caload\n", c);
		            fprintf(pFile, "\t%cstore %d\n", c, p_temp1->index);
                } else if (p_temp2->attr.arrayDim == 0) {
                    memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp1->index);
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    fprintf(pFile, "\t%cload %d\n", c, p_temp2->index);
		            fprintf(pFile, "\t%castore\n", c);
                } else {
                    memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp1->index);
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp2->index);
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\t%caload\n", c);
                    fprintf(pFile, "\t%castore\n", c);
                }
            } else if (p_temp1) { /* RHS is not local variable */
                p_temp2 = searchEntry(right->id, st_opened);
                if (p_temp1->attr.arrayDim == 0 && p_temp2->attr.arrayDim == 0) {
                    if (p_temp2->attr.funcType != TypeVar) {
                        if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                        else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                    } else fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
                    fprintf(pFile, "\t%cstore %d\n", c, p_temp1->index);
                } else if (p_temp2->attr.arrayDim == 0) {
		            fprintf(pFile, "\taload %d\n", p_temp1->index);
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            if (p_temp2->attr.funcType != TypeVar) {
                        if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                        else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                    }
		            else fprintf(pFile, "\t%cload %d\n", c, p_temp2->index);
		            fprintf(pFile, "\t%castore\n", c);
                } else if (p_temp1->attr.arrayDim == 0) {
		            memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    fprintf(pFile, "\t%caload\n", c);
		            fprintf(pFile, "\t%cstore %d\n", c, p_temp1->index);
                } else {
                    memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp1->index);
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\t%caload\n", c);
                    fprintf(pFile, "\t%castore\n", c);
                }
            } else if (p_temp2) { /* LHS is not local variable */
                p_temp1 = searchEntry(left->id, st_opened);
                if (p_temp1->attr.arrayDim == 0 && p_temp2->attr.arrayDim == 0) {
                    fprintf(pFile, "\t%cload %d\n", c, p_temp2->index);
                    if (p_temp1->attr.funcType != TypeFunc) fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
                } else if (p_temp1->attr.arrayDim == 0) {
		            fprintf(pFile, "\taload %d\n", p_temp2->index);
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\t%caload\n", c);
                    if (p_temp1->attr.funcType != TypeFunc) fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
		        } else if (p_temp2->attr.arrayDim == 0) {
		            memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    fprintf(pFile, "\t%cload %d\n", c, p_temp2->index);
		            fprintf(pFile, "\t%castore\n", c);
                } else {
                	memset(array_dim, 0, sizeof(array_dim));
                    for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            memset(array_dim, 0, sizeof(array_dim));
		            fprintf(pFile, "\taload %d\n", p_temp2->index);
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\t%caload\n", c);
                    fprintf(pFile, "\t%castore\n", c);
                }
            } else {  /* LHS and RHS are not local variable */
                p_temp1 = searchEntry(left->id, st_opened);
                p_temp2 = searchEntry(right->id, st_opened);
                if (p_temp1->attr.arrayDim == 0 && p_temp2->attr.arrayDim == 0) {
                    if (p_temp2->attr.funcType != TypeVar) {
                        if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                        else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                    }
		            else fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
		            if (p_temp1->attr.funcType != TypeFunc) fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
                } else if (p_temp1->attr.arrayDim == 0) {
                    memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    fprintf(pFile, "\t%caload\n", c);
                    if (p_temp1->attr.funcType != TypeFunc) fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
                } else if (p_temp2->attr.arrayDim == 0) {
                    memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    if (p_temp2->attr.funcType != TypeVar) {
                        if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                        else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                    }
		            else fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
                    fprintf(pFile, "\t%castore\n", c);
                } else {
                    memset(array_dim, 0, sizeof(array_dim));
                    for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
		            node = right->leftmostChild;
		            for (i = 0; i < p_temp2->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp2->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
		            fprintf(pFile, "\%caload\n", c);
                    fprintf(pFile, "\t%castore\n", c);
                }
            }
        } else {  /* RHS is a literal */
            if (p_temp1) { /* LHS is local variable */
                if (p_temp1->attr.arrayDim == 0) {
                    processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
                    fprintf(pFile, "\t%cstore %d\n", c, p_temp1->index);
                } else {
                    fprintf(pFile, "\taload %d\n", p_temp1->index);
                    processArithmeticOp(pFile, left->leftmostChild, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[0]);
                    fprintf(pFile, "\tisub\n");
                    processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
                    fprintf(pFile, "\t%castore\n", c);  
                }
            } else {   /* LHS is global variable */
                p_temp1 = searchEntry(left->id, st_opened);
                if (p_temp1->attr.arrayDim == 0) {
                    processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
                    if (p_temp1->attr.funcType != TypeFunc) fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
                } else {
                    memset(array_dim, 0, sizeof(array_dim));
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
		            fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
		            node = left->leftmostChild;
		            for (i = 0; i < p_temp1->attr.arrayDim; i++) {
		                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
		                fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
		                fprintf(pFile, "\tisub\n");
		                if (i != p_temp1->attr.arrayDim - 1)
		                    fprintf(pFile, "\taaload\n");
		                node = node->rightSibling;
		            }
                    processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
		            fprintf(pFile, "\t%castore\n", c); 
                }               
            }
        }
        p_temp1 = searchEntry(left->id, st_opened);
        if (p_temp1 && p_temp1->attr.funcType == TypeFunc) fprintf(pFile, "\tgoto L%d\n", return_label); 
    } else {
        if (right->id) { /* LHS and RHS are global variables */
            p_temp1 = searchEntry(left->id, st_opened);
            p_temp2 = searchEntry(right->id, st_opened);
            if (p_temp1->attr.arrayDim == 0 && p_temp2->attr.arrayDim == 0) {
                if (p_temp2->attr.funcType != TypeVar) {
                    if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                    else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                } else {
                    fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
                }
                fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
            } else if (p_temp1->attr.arrayDim == 0) {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
                node = right->leftmostChild;
                for (i = 0; i < p_temp2->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp2->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                fprintf(pFile, "\t%caload\n", c);
                fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
            } else if (p_temp2->attr.arrayDim == 0) {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
                node = left->leftmostChild;
                for (i = 0; i < p_temp1->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp1->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                if (p_temp2->attr.funcType != TypeVar) {
                    if (strcmp(right->id, "readInt") == 0) readInt(pFile, current_label);
                    else fprintf(pFile, "\tinvokestatic %s/%s()%c\n", className, right->id, toupper(c));
                } else {
                    fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
                }
                fprintf(pFile, "\t%castore\n", c);
            } else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
                node = left->leftmostChild;
                for (i = 0; i < p_temp1->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp1->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp2->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
                node = right->leftmostChild;
                for (i = 0; i < p_temp2->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp2->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp2->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                fprintf(pFile, "\t%caload\n", c);
                fprintf(pFile, "\t%castore\n", c);
            } 
        } else {   /* RHS is a literal */
            p_temp1 = searchEntry(left->id, st_opened);
            if (p_temp1->attr.arrayDim == 0) {
                processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
                fprintf(pFile, "\tputstatic %s/%s %c\n", className, left->id, toupper(c));
            } else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp1->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, left->id, array_dim, toupper(c));
                node = left->leftmostChild;
                for (i = 0; i < p_temp1->attr.arrayDim; i++) {
                    processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
                    fprintf(pFile, "\tldc %d\n", p_temp1->attr.lowerIndex[i]);
                    fprintf(pFile, "\tisub\n");
                    if (i != p_temp1->attr.arrayDim - 1)
                        fprintf(pFile, "\taaload\n");
                    node = node->rightSibling;
                }
                processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
                fprintf(pFile, "\t%castore\n", c);
            }
        }
    }
}

void assignString(FILE *pFile, struct Node *left, struct Node *right, stlink st, int insideFunc) {
    link p_temp1 = NULL;
    link p_temp2 = NULL;
    if (insideFunc) {
        p_temp1 = checkEntry(left->id, st);               
        if (right->id) {
            p_temp2 = checkEntry(right->id, st);
            if (p_temp1 && p_temp2) {
                fprintf(pFile, "\taload %d\n", p_temp2->index);
                fprintf(pFile, "\tastore %d\n", p_temp1->index);
            } else if (p_temp1) {
                fprintf(pFile, "\tgetstatic %s/%s Ljava/lang/String;\n", className, right->id);
                fprintf(pFile, "\tastore %d\n", p_temp1->index);
            } else if (p_temp2) {
                fprintf(pFile, "\taload %d\n", p_temp2->index);
                fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, left->id);
            } else {
                fprintf(pFile, "\tgetstatic %s/%s Ljava/lang/String;\n", className, right->id);
                fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, left->id);
            }
        } else {
            if (p_temp1) {
                fprintf(pFile, "\tldc %s\n", right->stringconst);
                fprintf(pFile, "\tastore %d\n", p_temp1->index);
            } else {
                fprintf(pFile, "\tldc %s\n", right->stringconst);
                fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, left->id);
            }
        }
    } else {
        if (right->id) {
            fprintf(pFile, "\tgetstatic %s/%s Ljava/lang/String;\n", className, right->id);
            fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, left->id);
        } else {
            fprintf(pFile, "\tldc %s\n", right->stringconst);
            fprintf(pFile, "\tputstatic %s/%s Ljava/lang/String;\n", className, left->id);
        }
    }
}

void processLogicOp(FILE *pFile, struct Node *left, struct Node *right, int gt, stlink st, int insideFunc, int label) {
    char c;
    int isReal = 0;
    struct Node *temp = left;
    link p_temp = NULL;
    int i;
    char array_dim[16];
    struct Node *node;

    if (left->nodeType == NODE_NOT) {
        temp = temp->leftmostChild;
    }
    if (temp->id && right->id) {
        p_temp = checkEntry(temp->id, st);
        if (p_temp && insideFunc) {
            if (p_temp->attr.type == TypeReal) isReal = 1;
            if (isReal) c = 'f';
            else c = 'i';
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\t%cload %d\n", c, p_temp->index);
            else {
                fprintf(pFile, "\taload %d\n", p_temp->index);
	            node = temp->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        } else {
            p_temp = searchEntry(temp->id, st_opened);
            if (p_temp->attr.type == TypeReal) isReal = 1;
            if (isReal) c = 'f';
            else c = 'i';
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\tgetstatic %s/%s %c\n", className, temp->id, toupper(c));
            else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, temp->id, array_dim, toupper(c));
	            node = temp->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        }
        p_temp = checkEntry(right->id, st);
        if (p_temp && insideFunc) {
            if (p_temp->attr.type == TypeReal) isReal = 1;
            if (isReal) c = 'f';
            else c = 'i';
            if (p_temp->attr.arrayDim == 0) 
                fprintf(pFile, "\t%cload %d\n", c, p_temp->index);
            else {
                fprintf(pFile, "\taload %d\n", p_temp->index);
	            node = right->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        } else {
            p_temp = searchEntry(right->id, st_opened);
            if (p_temp->attr.type == TypeReal) isReal = 1;
            if (isReal) c = 'f';
            else c = 'i';
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
            else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
	            node = right->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        }
    } else if (temp->id) {
        if (right->nodeType == NODE_REAL) isReal = 1;
        if (isReal) c = 'f';
        else c = 'i';
        p_temp = checkEntry(temp->id, st);
        if (p_temp && insideFunc) {
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\t%cload %d\n", c, p_temp->index);
            else {
                fprintf(pFile, "\taload %d\n", p_temp->index);
	            node = temp->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        } else {
            p_temp = searchEntry(temp->id, st_opened);
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\tgetstatic %s/%s %c\n", className, temp->id, toupper(c));
            else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, temp->id, array_dim, toupper(c));
	            node = temp->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        }
        if (isArithOpers(right)) {
            processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
        } else {
            if (isReal)
                fprintf(pFile, "\tldc %f\n", right->number);
            else
                fprintf(pFile, "\tldc %d\n", (int)right->number);
        }
    } else if (right->id) {
        if (temp->nodeType == NODE_REAL) isReal = 1;
        if (isReal) c = 'f';
        else c = 'i';
        if (isArithOpers(temp)) {
            processArithmeticOp(pFile, temp, st, insideFunc, isReal, 0, 0);
        } else {
            if (isReal)
                fprintf(pFile, "\tldc %f\n", temp->number);
            else
                fprintf(pFile, "\tldc %d\n", (int)temp->number);
        }
        p_temp = checkEntry(right->id, st);
        if (p_temp && insideFunc) {
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\t%cload %d\n", c, p_temp->index);
            else {
                fprintf(pFile, "\taload %d\n", p_temp->index);
	            node = right->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        } else {
            p_temp = searchEntry(right->id, st_opened);
            if (p_temp->attr.arrayDim == 0)
                fprintf(pFile, "\tgetstatic %s/%s %c\n", className, right->id, toupper(c));
            else {
                memset(array_dim, 0, sizeof(array_dim));
                for (i = 0; i < p_temp->attr.arrayDim; i++) strcat(array_dim, "[");
                fprintf(pFile, "\tgetstatic %s/%s %s%c\n", className, right->id, array_dim, toupper(c));
	            node = right->leftmostChild;
	            for (i = 0; i < p_temp->attr.arrayDim; i++) {
	                processArithmeticOp(pFile, node, st, insideFunc, 0, 0, 0);
	                fprintf(pFile, "\tldc %d\n", p_temp->attr.lowerIndex[i]);
	                fprintf(pFile, "\tisub\n");
	                if (i != p_temp->attr.arrayDim - 1)
	                    fprintf(pFile, "\taaload\n");
	                node = node->rightSibling;
	            }
	            fprintf(pFile, "\t%caload\n", c);
            }
        }
    } else {
        if (right->nodeType == NODE_REAL) isReal = 1;
        if (isReal) c = 'f';
        else c = 'i';
        if (isArithOpers(temp) && isArithOpers(right)) {
            processArithmeticOp(pFile, temp, st, insideFunc, isReal, 0, 0);
            processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
        } else if (isArithOpers(temp)) {
            processArithmeticOp(pFile, temp, st, insideFunc, isReal, 0, 0);
            if (isReal) {
                fprintf(pFile, "\tldc %f\n", right->number);
            } else {
                fprintf(pFile, "\tldc %d\n", (int)right->number);
            }
        } else if (isArithOpers(right)) {
            if (isReal)
                fprintf(pFile, "\tldc %f\n", temp->number);
            else
                fprintf(pFile, "\tldc %d\n", (int)temp->number);
            processArithmeticOp(pFile, right, st, insideFunc, isReal, 0, 0);
        } else {
            if (isReal) {
                fprintf(pFile, "\tldc %f\n", temp->number);
                fprintf(pFile, "\tldc %f\n", right->number);
            } else {
                fprintf(pFile, "\tldc %d\n", (int)temp->number);
                fprintf(pFile, "\tldc %d\n", (int)right->number);
            }
        }
    }
    if (isReal) {
        fprintf(pFile, "\tfcmpg\n");
        fprintf(pFile, "\tldc 0\n");
    } 
    switch (gt) {
        case NODE_OP_LT:
            if (left->nodeType == NODE_NOT) 
                fprintf(pFile, "\tif_icmpge L%d\n", label);
            else  
                fprintf(pFile, "\tif_icmplt L%d\n", label);
            break;
        case NODE_OP_GT:
            if (left->nodeType == NODE_NOT)
                fprintf(pFile, "\tif_icmple L%d\n", label);
            else
                fprintf(pFile, "\tif_icmpgt L%d\n", label);
            break;
        case NODE_OP_LE:
            if (left->nodeType == NODE_NOT)
                fprintf(pFile, "\tif_icmpgt L%d\n", label);
            else
                fprintf(pFile, "\tif_icmple L%d\n", label);
            break;
        case NODE_OP_GE:
            if (left->nodeType == NODE_NOT)
                fprintf(pFile, "\tif_icmplt L%d\n", label);
            else
                fprintf(pFile, "\tif_icmpge L%d\n", label);
            break;
        case NODE_OP_EQ:
            if (left->nodeType == NODE_NOT)
                fprintf(pFile, "\tif_icmpne L%d\n", label);
            else
                fprintf(pFile, "\tif_icmpeq L%d\n", label);
            break;
        case NODE_OP_NE:
            if (left->nodeType == NODE_NOT)
                fprintf(pFile, "\tif_icmpeq L%d\n", label);
            else
                fprintf(pFile, "\tif_icmpne L%d\n", label);
            break;
    }
}

void processAssign(FILE *pFile, struct Node *node, stlink st, int insideFunc)
{
    link p_temp = searchEntry(node->leftmostChild->id, st);
	if (p_temp->attr.type == TypeInt) {
		assignNumber(pFile, node->leftmostChild, node->leftmostChild->rightSibling, st, insideFunc, 0);
	} 
	else if (p_temp->attr.type == TypeReal) {
		assignNumber(pFile, node->leftmostChild, node->leftmostChild->rightSibling, st, insideFunc, 1);
	}
	else if (p_temp->attr.type == TypeString) {
		assignString(pFile, node->leftmostChild, node->leftmostChild->rightSibling, st, insideFunc);
	}
}

void processCall(FILE *pFile, struct Node *node, stlink st, int insideFunc, int isFunc) {
	if (strcmp(node->leftmostChild->id, "printInt") == 0) {
		printNumber(pFile, node->leftmostChild->rightSibling, st, insideFunc, 0, 0);
	}
	else if (strcmp(node->leftmostChild->id, "printReal") == 0) {
		printNumber(pFile, node->leftmostChild->rightSibling, st, insideFunc, 1, 0);
	}
	else if (strcmp(node->leftmostChild->id, "printString") == 0) {
		printString(pFile, node->leftmostChild->rightSibling, st, insideFunc, 0);
	}
    else if (strcmp(node->leftmostChild->id, "printlnInt") == 0) {
		printNumber(pFile, node->leftmostChild->rightSibling, st, insideFunc, 0, 1);
	}
	else if (strcmp(node->leftmostChild->id, "printlnReal") == 0) {
		printNumber(pFile, node->leftmostChild->rightSibling, st, insideFunc, 1, 1);
	}
	else if (strcmp(node->leftmostChild->id, "printlnString") == 0) {
		printString(pFile, node->leftmostChild->rightSibling, st, insideFunc, 1);
	}
    else {
        functionCall(pFile, node->leftmostChild, isFunc, st, insideFunc, 0);   
    }
}

void processIf(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label) {
    current_label += 2;
	ifCondition(pFile, node->leftmostChild, st, insideFunc, label);
	ifElseBody(pFile, node->leftmostChild->rightSibling->rightSibling, st, insideFunc, label);
	ifThenBody(pFile, node->leftmostChild->rightSibling, 0, st, insideFunc, label, 0);
}

void processWhile(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label, int isfor) {
    current_label += 3;
	fprintf(pFile, "L%d:\n", label);
	ifCondition(pFile, node->leftmostChild, st, insideFunc, label + 1);
	fprintf(pFile, "\tgoto L%d\n", label + 2);
	ifThenBody(pFile, node->leftmostChild->rightSibling, 1, st, insideFunc, label + 1, isfor);
}

void processRepeat(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label) {
    current_label += 2;
    ifThenBody(pFile, node->leftmostChild->rightSibling, 2, st, insideFunc, label, 0);
    ifCondition(pFile, node->leftmostChild, st, insideFunc, label + 1);
    fprintf(pFile, "\tgoto L%d\n", label);
    fprintf(pFile, "L%d:\n", label + 1); 
}

void processFor(FILE *pFile, struct Node *node, stlink st, int insideFunc, int label) {
    link p_temp = NULL;
    int downto = 0;
    if ((int)node->leftmostChild->leftmostChild->number <= (int)node->leftmostChild->leftmostChild->rightSibling->number) 
        downto = 1;
    else 
        downto = 2;
    struct Node *temp_le;
    struct Node *temp_while = MakeNode(NODE_WHILE);
    if (downto == 1) {
        temp_le = MakeNode(NODE_OP_LE);
    } else {
        temp_le = MakeNode(NODE_OP_GE);
    }
    struct Node *temp_body = node->leftmostChild->rightSibling;
    fprintf(pFile, "\tldc %d\n", (int)node->leftmostChild->leftmostChild->number);
    p_temp = checkEntry(node->leftmostChild->id, st);
    if (insideFunc && p_temp)
        fprintf(pFile, "\tistore %d\n", p_temp->index);
    else
    	fprintf(pFile, "\tputstatic %s/%s I\n", className, node->leftmostChild->id);
    temp_le->leftmostChild = node->leftmostChild;
    temp_le->leftmostChild->rightSibling = node->leftmostChild->leftmostChild->rightSibling;
    temp_le->leftmostChild->leftmostChild = NULL;
    temp_while->leftmostChild = temp_le;
    temp_while->leftmostChild->rightSibling = temp_body;
    processWhile(pFile, temp_while, st, insideFunc,label, downto);
    //PrintTree(temp_while, 0);
}

void processFunc(FILE *pFile, struct Node *node, int st_index, int isFunc) { 
    struct Node *temp = node->leftmostChild;
    struct Node *temp1;
    stlink st = st_head;
    while (st->index != st_index) {
        st = st->next;
    }
    link p_temp = searchEntry(temp->id, st_opened);
    char paras[128];
    char ret = 'i';
    int i, j = 0, k;
    if (p_temp->attr.type == TypeInt) ret = 'i';
    else if (p_temp->attr.type == TypeReal) ret = 'f';
    
    for (i = 0; i < p_temp->attr.parasNum; i++) {
        for (k = 0; k < p_temp->attr.paras[i].arrayDim; k++) {
            paras[j++] = '[';
        }
        if (p_temp->attr.paras[i].type == TypeInt) paras[j++] = 'I';
        else if (p_temp->attr.paras[i].type == TypeReal) paras[j++] = 'F';
        else if (p_temp->attr.paras[i].type == TypeString) { strcpy(&paras[j], "Ljava/lang/String;"); j+=18; }
    }
    paras[j] = '\0';
        if (isFunc) {
            fprintf(pFile, ".method public static %s(%s)%c\n", temp->id, paras, toupper(ret));
        } else {
            fprintf(pFile, ".method public static %s(%s)V\n", temp->id, paras);
        }
	fprintf(pFile, "\t.limit locals 100\n");
	fprintf(pFile, "\t.limit stack 100\n");

    while (temp->nodeType != NODE_BLOCK && temp->rightSibling != NULL) {
        temp = temp->rightSibling;
        if (temp->nodeType == NODE_VAR_DECL) {
            temp1 = temp->leftmostChild;
            while (temp1->id) {
                p_temp = checkEntry(temp1->id, st);
                int i;
                char str[16];
                if (p_temp->attr.type == TypeInt) {
                    if (p_temp->attr.arrayDim == 0) {
                        fprintf(pFile, "\tldc %d\n", 0);
				        fprintf(pFile, "\tistore %d\n", p_temp->index); 
                    } else {
                        for (i = 0; i < p_temp->attr.arrayDim; i++) {
                            fprintf(pFile, "\tbipush %d\n", p_temp->attr.upperIndex[i] - p_temp->attr.lowerIndex[i] + 1);
                            str[i] = '[';
                        }
                        str[i] = '\0';
                        fprintf(pFile, "\tmultianewarray %sI %d\n", str, p_temp->attr.arrayDim);
                        fprintf(pFile, "\tastore %d\n", p_temp->index);
                    }
                } else if (p_temp->attr.type == TypeReal) {
                    if (p_temp->attr.arrayDim == 0) {
                        fprintf(pFile, "\tldc %f\n", 0.0);
				        fprintf(pFile, "\tfstore %d\n", p_temp->index);
                    } else {
                        for (i = 0; i < p_temp->attr.arrayDim; i++) {
                            fprintf(pFile, "\tbipush %d\n", p_temp->attr.upperIndex[i] - p_temp->attr.lowerIndex[i] + 1);
                            str[i] = '[';
                        }
                        str[i] = '\0';
                        fprintf(pFile, "\tmultianewarray %sF %d\n", str, p_temp->attr.arrayDim);
                        fprintf(pFile, "\tastore %d\n", p_temp->index);
                    } 
                }
                temp1 = temp1->rightSibling;
            }
        }
    }
    return_label = current_label++;
    processBlock(pFile, temp->leftmostChild, st, 1);

    if (isFunc) {
        fprintf(pFile, "L%d:\n", return_label);
        fprintf(pFile, "\t%creturn\n", ret);
    }
    else fprintf(pFile, "\treturn\n");
    fprintf(pFile, ".end method\n\n");
}

void ifCondition(FILE *pFile, struct Node *cond, stlink st, int insideFunc, int label) {
    link p_temp = NULL;
    if (cond->id) {
        p_temp = checkEntry(cond->id, st);
        if (p_temp->attr.type == TypeInt) {
            if (insideFunc) {
                fprintf(pFile, "\tiload %d\n", p_temp->index);
                fprintf(pFile, "\tifne L%d\n", label);
            } else {
                fprintf(pFile, "\tgetstatic %s/%s I\n", className, cond->id);
                fprintf(pFile, "\tifne L%d\n", label);
            }
		} 
		else if (p_temp->attr.type == TypeReal) {
		        if (insideFunc) {
		            fprintf(pFile, "\tfload %d\n", p_temp->index);
		            fprintf(pFile, "\tldc 0.0\n");
		            fprintf(pFile, "\tfcmpg\n");
		            fprintf(pFile, "\tifne L%d\n", label);
		        } else {
		            fprintf(pFile, "\tgetstatic %s/%s F\n", className, cond->id);
		            fprintf(pFile, "\tldc 0.0\n");
		            fprintf(pFile, "\tfcmpg\n");
		            fprintf(pFile, "\tifne L%d\n", label);
		        }
		}
		else if (p_temp->attr.type == TypeString) {
			
		}
    } else if (cond->nodeType == NODE_CALL) {
        processCall(pFile, cond, st, insideFunc, 1);
        fprintf(pFile, "\tifne L%d\n", label);
    } else if (isArithOpers(cond)) {
        processArithmeticOp(pFile, cond, st, insideFunc, 0, 0, 0);
        fprintf(pFile, "\tifne L%d\n", label);
    } else if (cond->nodeType == NODE_OP_LT)  {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_LT, st, insideFunc, label);
    } else if (cond->nodeType == NODE_OP_GT) {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_GT, st, insideFunc, label);
    } else if (cond->nodeType == NODE_OP_LE) {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_LE, st, insideFunc, label);
    } else if (cond->nodeType == NODE_OP_GE) {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_GE, st, insideFunc, label);
    } else if (cond->nodeType == NODE_OP_EQ) {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_EQ, st, insideFunc, label);
    } else if (cond->nodeType == NODE_OP_NE) {
        processLogicOp(pFile, cond->leftmostChild, cond->leftmostChild->rightSibling, NODE_OP_NE, st, insideFunc, label);
    } else if (cond->nodeType == NODE_NOT) {
        cond = cond->leftmostChild;
        if (cond->id) {
		    p_temp = checkEntry(cond->id, st);
		    if (p_temp->attr.type == TypeInt) {
		        if (insideFunc) {
		            fprintf(pFile, "\tiload %d\n", p_temp->index);
		            fprintf(pFile, "\tifeq L%d\n", label);
		        } else {
		            fprintf(pFile, "\tgetstatic %s/%s I\n", className, cond->id);
		            fprintf(pFile, "\tifeq L%d\n", label);
		        }
			} 
			else if (p_temp->attr.type == TypeReal) {
				    if (insideFunc) {
				        fprintf(pFile, "\tfload %d\n", p_temp->index);
				        fprintf(pFile, "\tldc 0.0\n");
				        fprintf(pFile, "\tfcmpg\n");
				        fprintf(pFile, "\tifeq L%d\n", label);
				    } else {
				        fprintf(pFile, "\tgetstatic %s/%s F\n", className, cond->id);
				        fprintf(pFile, "\tldc 0.0\n");
				        fprintf(pFile, "\tfcmpg\n");
				        fprintf(pFile, "\tifeq L%d\n", label);
				    }
			}
			else if (p_temp->attr.type == TypeString) {
			
			}
		} else if (cond->nodeType == NODE_CALL) {
		    processCall(pFile, cond, st, insideFunc, 1);
		    fprintf(pFile, "\tifeq L%d\n", label);
		} else {
            processArithmeticOp(pFile, cond, st, insideFunc, 0, 0, 0);
            fprintf(pFile, "\tifeq L%d\n", label);
        }
    } else {
        processArithmeticOp(pFile, cond, st, insideFunc, 0, 0, 0);
        fprintf(pFile, "\tifne L%d\n", label);
    }
}

void ifThenBody(FILE *pFile, struct Node *thenNode, int isWhile, stlink st, int insideFunc, int label, int isfor) {
    link p_temp = NULL;
    fprintf(pFile, "L%d:\n", label);
    if (thenNode->nodeType == NODE_BLOCK) {
        processBlock(pFile, thenNode->leftmostChild,st, insideFunc);
    } else if (thenNode->nodeType == NODE_CALL) {
        processCall(pFile, thenNode,st, insideFunc, 0);
    } else if (thenNode->nodeType == NODE_ASSIGN) {
        processAssign(pFile, thenNode,st, insideFunc);
    } else if (thenNode->nodeType == NODE_IF) {
		processIf(pFile, thenNode,st, insideFunc, current_label);
	} else if (thenNode->nodeType == NODE_WHILE) {
		processWhile(pFile, thenNode,st, insideFunc, current_label, 0);
	} else if (thenNode->nodeType == NODE_REPEAT) {
        processRepeat(pFile, thenNode, st, insideFunc, current_label);
    } else if (thenNode->nodeType == NODE_FOR) {
        processFor(pFile, thenNode, st, insideFunc, current_label);
    }
    if (isfor == 1) {
        p_temp = checkEntry(thenNode->leftmostSibling->id, st);
        fprintf(pFile, "\ticonst_1\n");
        if (insideFunc && p_temp)
            fprintf(pFile, "\tiload %d\n", p_temp->index);
        else
            fprintf(pFile, "\tgetstatic %s/%s I\n", className, thenNode->leftmostSibling->id);
        fprintf(pFile, "\tiadd\n");
        if (insideFunc && p_temp)
            fprintf(pFile, "\tistore %d\n", p_temp->index);
        else
            fprintf(pFile, "\tputstatic %s/%s I\n", className, thenNode->leftmostSibling->id); 
    } else if (isfor == 2) {
        p_temp = checkEntry(thenNode->leftmostSibling->id, st);
        if (insideFunc && p_temp)
            fprintf(pFile, "\tiload %d\n", p_temp->index);
        else
            fprintf(pFile, "\tgetstatic %s/%s I\n", className, thenNode->leftmostSibling->id);
        fprintf(pFile, "\ticonst_1\n");
        fprintf(pFile, "\tisub\n");
        if (insideFunc && p_temp)
            fprintf(pFile, "\tistore %d\n", p_temp->index);
        else
            fprintf(pFile, "\tputstatic %s/%s I\n", className, thenNode->leftmostSibling->id); 
    }
    if (isWhile == 1) fprintf(pFile, "\tgoto L%d\n", label - 1);
    if (isWhile != 2) fprintf(pFile, "L%d:\n", label + 1);
    
}

void ifElseBody(FILE *pFile, struct Node *elseNode, stlink st, int insideFunc, int label) {
    if (elseNode->nodeType == NODE_BLOCK) {
        processBlock(pFile, elseNode->leftmostChild, st, insideFunc);
    } else if (elseNode->nodeType == NODE_CALL) {
        processCall(pFile, elseNode,st, insideFunc, 0);
    } else if (elseNode->nodeType == NODE_ASSIGN) {
        processAssign(pFile, elseNode,st, insideFunc);
    } else if (elseNode->nodeType == NODE_IF) {
		processIf(pFile, elseNode,st, insideFunc, current_label);
	} else if (elseNode->nodeType == NODE_WHILE) {
		processWhile(pFile, elseNode, st, insideFunc, current_label, 0);
	} else if (elseNode->nodeType == NODE_REPEAT) {
        processRepeat(pFile, elseNode, st, insideFunc, current_label);
    } else if (elseNode->nodeType == NODE_FOR) {
        processFor(pFile, elseNode, st, insideFunc, current_label);
    }
    fprintf(pFile, "\tgoto L%d\n", label + 1);
}

void processBlock(FILE *pFile, struct Node *startNode, stlink st, int insideFunc) {
    struct Node *temp = startNode;
    link p_temp = NULL;
    
    while (temp) {
        if (temp->nodeType == NODE_ASSIGN) {
            processAssign(pFile, temp, st, insideFunc);
        }
        else if (temp->nodeType == NODE_CALL) {
            processCall(pFile, temp, st, insideFunc, 0);
        }
        else if (temp->nodeType == NODE_BLOCK) {
            processBlock(pFile, temp->leftmostChild, st, insideFunc);
        }
        else if (temp->nodeType == NODE_IF) {
            if (temp->leftmostChild->nodeType == NODE_OP_AND) {
                struct Node *temp_if = MakeNode(NODE_IF);
                temp_if->leftmostChild = temp->leftmostChild->leftmostChild->rightSibling;
                temp_if->leftmostChild->rightSibling = temp->leftmostChild->rightSibling;
                temp->leftmostChild = temp->leftmostChild->leftmostChild;
                temp->leftmostChild->rightSibling = temp_if;
                temp->leftmostChild->rightSibling->rightSibling = temp_if->leftmostChild->rightSibling->rightSibling;
                processIf(pFile, temp, st, insideFunc, current_label);
                //PrintTree(temp, 0);
            } else if (temp->leftmostChild->nodeType == NODE_OP_OR) {
                struct Node *temp_if = MakeNode(NODE_IF);
                temp_if->leftmostChild = temp->leftmostChild->leftmostChild->rightSibling;
                temp_if->leftmostChild->rightSibling = temp->leftmostChild->rightSibling;
                temp->leftmostChild = temp->leftmostChild->leftmostChild;
                temp->leftmostChild->rightSibling = CopyNode(temp_if->leftmostChild->rightSibling);
                temp->leftmostChild->rightSibling->rightSibling = temp_if;
                processIf(pFile, temp, st, insideFunc, current_label);
                //PrintTree(temp, 0);
            } else {
                processIf(pFile, temp, st, insideFunc, current_label);
            }
        }
        else if (temp->nodeType == NODE_WHILE) {
            processWhile(pFile, temp, st, insideFunc, current_label, 0);
        } else if (temp->nodeType == NODE_REPEAT) {
            processRepeat(pFile, temp, st, insideFunc, current_label);
        } else if (temp->nodeType == NODE_FOR) {
            processFor(pFile, temp, st, insideFunc, current_label);
        }
        temp = temp->rightSibling;
    }
}

void codeGenerateMain(struct Node *node, char output[]) {
    struct Node *temp = node->leftmostChild;
    className = node->leftmostChild->id;
    FILE *pFile = fopen(output, "w");
    if (pFile == NULL) {
        printf("create j file fails\n");
        return;
    }
    fprintf(pFile, ".class public %s\n", node->leftmostChild->id);
    fprintf(pFile, ".super java/lang/Object\n\n");
    buildStaticField(pFile);
    initialStaticField(pFile);
    int func_index = 1;
    while (temp->nodeType != NODE_BLOCK && temp->rightSibling != NULL) {
        if (temp->nodeType == NODE_PROC) { processFunc(pFile, temp, func_index, 0); func_index++; }
        else if (temp->nodeType == NODE_FUNC) { processFunc(pFile, temp, func_index, 1); func_index++; }
        temp = temp->rightSibling;
    }
    fprintf(pFile, ".method public static main([Ljava/lang/String;)V\n");
    fprintf(pFile, "\t.limit locals 100\n");
    fprintf(pFile, "\t.limit stack 100\n");
    fprintf(pFile, "\tinvokestatic %s/vinit()V\n", className);
    processBlock(pFile, temp->leftmostChild, st_opened, 0);
    fprintf(pFile, "\treturn\n");
    fprintf(pFile, ".end method\n");
    fclose(pFile);
}
