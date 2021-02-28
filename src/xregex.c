#include "xregex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FIRST_BIT (0x80)       //1000 0000b
#define FIRST_TWO_BITS (0xc0)  //1100 0000b
#define LAST_SIX_BITS (0x3f)   //0011 1111b

#define FIRST_ALLOC_CHILD_SIZE (10)
#define FIRST_ALLOC_CONENT_SIZE (30)

typedef struct RegexTreeNode{
    struct RegexTreeNode* parent;
    struct RegexTreeNode* firstChild;
    struct RegexTreeNode* nextSibling;
    enum Symbol symbol;
    IntVector * string;

} RegexTreeNode;




void dealError(int exitStauts, const char *errorPosition, char *msg) {
    fprintf(stderr, "%s : %s", errorPosition, msg);
    exit(exitStauts);
}

int utf8ToInt(unsigned char *s, int *result);

int intToUtf8(int number, char *result);

int utf8ToInt(unsigned char *s, int *result) {
    int byte_num = 0;
    int tmp = *s;
    *result = 0;
    while (0x80 == (tmp & FIRST_BIT)) {
        byte_num++;
        tmp <<= 1;
    }
    switch (byte_num) {
        case 0:
            *result = *s;
            return 1;
        case 1:
            return -1;  //return  error
        case 2:
            *result += (int) (*s - 0xc0);
            break;
        case 3:
            *result += (int) (*s - 0xe0);
            break;
        case 4:
            *result += (int) (*s - 0xf0);
            break;
        default:
            return -1; //return  error
    }

    for (int i = byte_num - 1; i > 0; i--) {
        s++;
        *result *= 0x40;
        if (0x80 != (*s) & FIRST_TWO_BITS) {
            return -1;//return  error
        } else {
            *result += (*s - 0x80);
        }
    }
    return byte_num;
}

int intToUtf8(int number, char *result) {
    if (number < 0) {
        return -1;  //return error
    } else if (number < 0x80) {
        *result = number;
        *(result + 1) = 0;
        return 1;
    } else if (number < 0x800) {
        *result = 0xc0 + (number >> 6);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        *(result + 1) = 0;
        return 2;
    } else if (number < 0x10000) {
        *result = 0xe0 + (number >> 12);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        *(result + 1) = 0;
        return 3;
    } else if (number <= 0xf48fbfbf) {
        *result = 0xf0 + (number >> 18);
        result++;
        *result = FIRST_BIT + ((number >> 12) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        *(result + 1) = 0;
        return 4;
    } else {
        return -1;  //return error
    }
}


/*convert express string into IntVector*/
IntVector *transExpress(char *express) {
    IntVector *iv = initIntVector(strlen(express));
    int i;
    while (utf8ToInt(express++, &i) != -1 && i != 0) {
        appendIntVector(iv, i);
    }
    return iv;
}

IntVector *getEscapeCharacterExpress(IntVector *express) {
    int dataLength = getIntVectorDataSize(express);
    IntVector *iv = initIntVector(dataLength);
    for (int i = 0; i < dataLength; i++) {
        int c = getIntVectorData(express, i);
        if ((int) '\\' == c) {
            appendIntVector(iv, getIntVectorData(express, ++i));
        } else if (c == (int) '[' || c == (int) ']' || c == (int) '(' || c == (int) ')' || c == (int) '*' ||
                   c == (int) '|') {
            appendIntVector(iv, -c);
        } else {
            appendIntVector(iv, c);
        }
    }
    return iv;
}

int F_S(RegexTreeNode *rt, LexicalResult *restInput);

int F_A(RegexTreeNode *rt, LexicalResult *restInput);

int F_B(RegexTreeNode *rt, LexicalResult *restInput);

int F_C(RegexTreeNode *rt, LexicalResult *restInput);

int F_D(RegexTreeNode *rt, LexicalResult *restInput);

int F_E(RegexTreeNode *rt, LexicalResult *restInput);

int F_F(RegexTreeNode *rt, LexicalResult *restInput);

RegexTreeNode *getRegexTree(LexicalResult *lr) {
    RegexTreeNode *root = malloc(sizeof(struct RegexTreeNode));
    root->symbol = N_S;
    root->parent = NULL;
    root->firstChild = NULL;
    root->nextSibling = NULL;
    F_S(root, lr);
    return root;
}

static RegexTreeNode *addNode(RegexTreeNode *parent, enum Symbol symbol) {
    RegexTreeNode *l = parent->firstChild;
    RegexTreeNode *n = malloc(sizeof(struct RegexTreeNode));
    n->firstChild = NULL;
    n->nextSibling = NULL;
    n->string = NULL;
    if (l == NULL) {
        parent->firstChild = n;
        n->parent = parent;
        n->symbol = symbol;
    } else {
        while (l->nextSibling != NULL) {
            l = l->nextSibling;
        }
        n->parent = parent;
        l->nextSibling = n;
        n->symbol = symbol;
    }
    return n;
}


/* return -1 when error */
int F_S(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == LEFT_PARENTHESIS || restInput->type == CHARACTER_STRING || restInput->type == LEFT_BRACKET) {
        printf("S->AS\n");
        restInput += F_A(addNode(rt, N_A), restInput);
        restInput += F_S(addNode(rt, N_S), restInput);
    } else if (restInput->type == END || restInput->type == RIGHT_PARENTHESIS) {
        printf("S->empty\n");
        /*EMPTY STRING*/
    } else {
        errorString = "error occur when parsing non-terminal character S";
        DEAL_ERRER(-1, errorString);
        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_A(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == LEFT_PARENTHESIS || restInput->type == CHARACTER_STRING || restInput->type == LEFT_BRACKET) {
        printf("A->BCD\n");
        restInput += F_B(addNode(rt, N_B), restInput);
        restInput += F_C(addNode(rt, N_C), restInput);
        restInput += F_D(addNode(rt, N_D), restInput);
    } else {
        errorString = "error occur when parsing non-terminal character A";
        DEAL_ERRER(-1, errorString);
        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_B(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == LEFT_PARENTHESIS) {
        printf("B->E\n");
        restInput += F_E(addNode(rt, N_E), restInput);
    } else if (restInput->type == CHARACTER_STRING) {
        printf("B->T\n");
        addNode(rt, T)->string = restInput->string;
        restInput += 1;
    } else if (restInput->type == LEFT_BRACKET) {
        printf("B->F\n");
        restInput += F_F(addNode(rt, N_F), restInput);
    } else {
        errorString = "error occur when parsing non-terminal character B";
        DEAL_ERRER(-1, errorString);

        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_C(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == ASTERISK) {
        printf("C->*\n");
        addNode(rt, T);
        restInput += 1;
    } else if (restInput->type == VERTICAL_BAR || restInput->type == LEFT_PARENTHESIS ||
               restInput->type == RIGHT_PARENTHESIS ||
               restInput->type == CHARACTER_STRING || restInput->type == LEFT_BRACKET || restInput->type == END) {
        printf("C->empty\n");
        /*EMPTY STRING*/
    } else {
        errorString = "error occur when parsing non-terminal character C";
        DEAL_ERRER(-1, errorString);
        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_D(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == VERTICAL_BAR) {
        printf("D->|S \n");
        restInput += 1;
        restInput += F_S(addNode(rt, N_S), restInput);
    } else if (restInput->type == LEFT_PARENTHESIS || restInput->type == RIGHT_PARENTHESIS ||
               restInput->type == CHARACTER_STRING || restInput->type == LEFT_BRACKET || restInput->type == END) {
        printf("D->empty\n");
        /*EMPTY STRING*/
    } else {
        errorString = "error occur when parsing non-terminal character D";
        DEAL_ERRER(-1, errorString);
        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_E(RegexTreeNode *rt, LexicalResult *restInput) {
    LexicalResult *initialRestInput = restInput;
    if (restInput->type == LEFT_PARENTHESIS) {
        printf("E->(S)\n");
        restInput += 1;
        restInput += F_S(addNode(rt, N_S), restInput);
        restInput += 1;
    } else {
        errorString = "error occur when parsing non-terminal character E";
        DEAL_ERRER(-1, errorString);
        return -1;
        /*maybe the routine should stop here*/
    }
    return restInput - initialRestInput;
}

int F_F(RegexTreeNode *rt, LexicalResult *restInput) {
    if (restInput->type == LEFT_BRACKET) {
        printf("F -> [T]\n");
        addNode(rt, T);
        return 3;
    } else {
        errorString = "error occur when parsing non-terminal character F";
        DEAL_ERRER(-1, errorString);
        return -1;
    }
}


/* No S have nextSibling*/
RegexTreeNode *eliminateSEmptyString(RegexTreeNode *rtn) {
    RegexTreeNode *l = rtn->firstChild;
    RegexTreeNode *r = rtn->nextSibling;
    if (l != NULL) {
        if (l->symbol == N_S) {
            if (l->firstChild != NULL) {      //S->AS
                eliminateSEmptyString(rtn->firstChild);
            } else {      //S->empty
                rtn->firstChild = NULL;
                free(l);
            }
        } else {
            eliminateSEmptyString(l);
        }
    }
    if (r != NULL) {
        if (r->symbol == N_S) {
            if (r->firstChild != NULL) {      //S->AS
                eliminateSEmptyString(rtn->nextSibling);
            } else {      //S->empty
                rtn->nextSibling = NULL;
                free(r);
            }
        } else {
            eliminateSEmptyString(r);
        }

    }
    return rtn;
}


RegexTreeNode *eliminateB(RegexTreeNode *rtn) {
    if (rtn->symbol == N_B) {
        if (rtn->firstChild != NULL) {
            RegexTreeNode *f = rtn->firstChild;
            if (f->symbol == N_E || f->symbol == T) {
                f->parent = rtn->parent;
                f->nextSibling = rtn->nextSibling;  //f.nextSibling is NULL at the beginning ,or it has an error
                rtn->parent->firstChild = f;    //B is always the first child of its parent (A)
                free(rtn);
                rtn = f;
            } else {
                /* error : B's child is neither T nor E*/
            }
        } else {
            /*error : B has no child*/
        };
    } else { ;
    }
    if (rtn->firstChild != NULL) {
        eliminateB(rtn->firstChild);
    }
    if (rtn->nextSibling != NULL) {
        eliminateB(rtn->nextSibling);
    }
    return rtn;
}


RegexTreeNode *moveD(RegexTreeNode *rtn) {
    if (rtn->symbol == N_D) {
        RegexTreeNode *f = rtn->firstChild;
        if (f != NULL) {
            /* D-> |S*/
            RegexTreeNode *p = rtn->parent;
            rtn->parent = p->parent;
            RegexTreeNode *l;
            for (l = p->firstChild; l->nextSibling != NULL && l->nextSibling->symbol != N_D; l = l->nextSibling) { ;
            }
            l->nextSibling = rtn->nextSibling;  //NULL
            p->nextSibling = rtn->firstChild;
            p->parent->firstChild = rtn;    //at the beginning , D's parent is A, and this A is always the first child of S
            p->parent = rtn;
            rtn->firstChild = p;
            moveD(p->nextSibling);
        } else {
            /* D -> empty*/
            RegexTreeNode *p = rtn->parent;
            RegexTreeNode *l;
            for (l = p->firstChild; l->nextSibling != NULL &&
                                    l->nextSibling->symbol != N_D; l = l->nextSibling) { ;/* get previous sibling*/
            }
            l->nextSibling = NULL;
            free(rtn);
        }
    } else {
        if (rtn->firstChild != NULL) {
            moveD(rtn->firstChild);
        }
        if (rtn->nextSibling != NULL && rtn->parent->symbol != N_D) {
            /* do not let the adjust affect the recursion */
            moveD(rtn->nextSibling);
        }
    }
    return rtn;
}

RegexTreeNode *moveC(RegexTreeNode *rtn) {
    if (rtn->symbol == N_C) {
        if (rtn->firstChild != NULL) {
            RegexTreeNode *l = rtn->parent->firstChild;    //tht previous sibling of C
            l->nextSibling = rtn->nextSibling;
            l->parent = rtn;
            rtn->parent->firstChild = rtn;
            free(rtn->firstChild);
            rtn->firstChild = l;

            if (l->nextSibling != NULL) {
                rtn = l->nextSibling;
            } else {
                return NULL;
            }
        } else {
            RegexTreeNode *l = rtn->parent->firstChild;
            l->nextSibling = rtn->nextSibling;
            free(rtn);
            if (l->nextSibling != NULL) {
                rtn = l->nextSibling;
            } else {
                return NULL;
            }
        }
    }

    if (rtn->firstChild != NULL) {
        moveC(rtn->firstChild);
    }
    if (rtn->nextSibling != NULL) {
        moveC(rtn->nextSibling);
    }
    return rtn;
}

static RegexTreeNode *eliminateA(RegexTreeNode *rtn);

static RegexTreeNode *eliminateS(RegexTreeNode *rtn);

RegexTreeNode *eliminateRedundancy(RegexTreeNode *rtn) {
    // A->C
    // A->T
    // A->E
    eliminateA(rtn);
    // S->D
    // S->T
    eliminateS(rtn);
    return rtn;

}

RegexTreeNode *eliminateA(RegexTreeNode *rtn) {
    RegexTreeNode *l = rtn->firstChild;
    RegexTreeNode *r = rtn->nextSibling;
    if (l != NULL) {
        if (l->symbol == N_A) {
            if (l->firstChild->symbol == T || l->firstChild->symbol == N_C || l->firstChild->symbol == N_E) {
                l->firstChild->parent = l->parent;
                l->firstChild->nextSibling = l->nextSibling;
                rtn->firstChild = l->firstChild;
                free(l);
            } else { ;
            }
        } else { ;
        }
        eliminateA(rtn->firstChild);
    }

    if (r != NULL) {
        if (r->symbol == N_A) {
            if (r->firstChild->symbol == T || r->firstChild->symbol == N_C || r->firstChild->symbol == N_E) {
                r->firstChild->parent = r->parent;
                r->firstChild->nextSibling = r->nextSibling;
                rtn->nextSibling = r->firstChild;
                free(r);
            } else { ;
            }
        } else { ;
        }
        eliminateA(rtn->nextSibling);
    }
    return rtn;
}

RegexTreeNode *eliminateS(RegexTreeNode *rtn) {
    RegexTreeNode *l = rtn->firstChild;
    RegexTreeNode *r = rtn->nextSibling;
    if (l != NULL) {
        if (l->symbol == N_S) {
            if ((l->firstChild->symbol == T && l->firstChild->nextSibling == NULL) || l->firstChild->symbol == N_D) {
                l->firstChild->parent = l->parent;
                l->firstChild->nextSibling = l->nextSibling;
                rtn->firstChild = l->firstChild;
                free(l);
            } else { ;
            }
        } else { ;
        }
        eliminateS(rtn->firstChild);
    }

    if (r != NULL) {
        if (r->symbol == N_S) {
            if ((r->firstChild->symbol == T && r->firstChild->nextSibling == NULL) || r->firstChild->symbol == N_D) {
                r->firstChild->parent = r->parent;
                r->firstChild->nextSibling = r->nextSibling;
                rtn->nextSibling = r->firstChild;
                free(r);
            } else { ;
            }
        } else { ;
        }
        eliminateS(rtn->nextSibling);
    }
    return rtn;

}


RegexTreeNode *addStart(RegexTreeNode *root) {
    if (root->symbol == N_S && root->firstChild->nextSibling == NULL) {
        root->symbol = START;
        return root;
    } else {
        RegexTreeNode *newRoot = malloc(sizeof(RegexTreeNode));
        newRoot->symbol = START;
        newRoot->parent = NULL;
        newRoot->firstChild = root;
        root->parent = newRoot;
        newRoot->string = NULL;
        return newRoot;
    }
}

RegexTreeNode *adjustPriority(RegexTreeNode *rtn) {
    if (rtn == NULL) {
        return NULL;
    } else if (rtn->symbol == N_D) {
        RegexTreeNode *p = rtn->parent;
        if (p->symbol == N_S) {
            rtn->parent = p->parent;
            if (p->parent->firstChild == p) {
                p->parent->firstChild = rtn;
            } else {
                p->parent->firstChild->nextSibling = rtn;
            }
            p->parent = rtn;
            p->firstChild->nextSibling = rtn->firstChild;
            rtn->firstChild->parent = p;

            p->nextSibling = rtn->firstChild->nextSibling;
            rtn->firstChild->nextSibling = NULL;
            rtn->firstChild = p;
            adjustPriority(rtn);
        } else {
            adjustPriority(rtn->firstChild);
            adjustPriority(rtn->nextSibling);
        }
    } else {
        adjustPriority(rtn->firstChild);
        adjustPriority(rtn->nextSibling);
    }
    return rtn;
}


RegexTreeNode *dealRegexTree(RegexTreeNode *root) {
    root = eliminateSEmptyString(root);
    root = eliminateB(root);
    root = moveD(root);
    root = moveC(root);
    root = eliminateRedundancy(root);
    root = addStart(root);
    root = adjustPriority(root);
    return root;
}


ASTNode *getBinaryTree(RegexTreeNode *rnt, ASTNode *brnt) {
    if (rnt == NULL) {
        return NULL;
    } else {
        brnt = malloc(sizeof(struct ASTNode));
        brnt->symbol = rnt->symbol;
        brnt->string = rnt->string;
        brnt->left = getBinaryTree(rnt->firstChild, brnt->left);
        brnt->right = (rnt->firstChild != NULL) ? getBinaryTree(rnt->firstChild->nextSibling, brnt->right):NULL;

        return brnt;
    }
}

ASTNode *getAST(LexicalResult *lr) {
    RegexTreeNode *root = getRegexTree(lr);
    root = dealRegexTree(root);
    ASTNode * ASTRoot;
    ASTRoot = getBinaryTree(root,ASTRoot);
    return ASTRoot;
}

ASTNode *parse(char express[]){
    IntVector *iv = transExpress(express);
    IntVector *iv2 = getEscapeCharacterExpress(iv);
    freeIntVector(iv);
    LexicalResult *lr = lexicalAnalyse(iv2);
    freeIntVector(iv2);
    struct ASTNode* ASTRoot = getAST(lr);
    free(lr);
    return ASTRoot;
}