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
    if (restInput->type == LEFT_PARENTHESIS || restInput->type == CHARACTER_STRING) {
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
    if (restInput->type == LEFT_PARENTHESIS || restInput->type == CHARACTER_STRING) {
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
               restInput->type == CHARACTER_STRING || restInput->type == END) {
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
        printf("D->|A \n");
        restInput += 1;
        restInput += F_A(addNode(rt, N_A), restInput);
    } else if (restInput->type == LEFT_PARENTHESIS || restInput->type == RIGHT_PARENTHESIS ||
               restInput->type == CHARACTER_STRING || restInput->type == END) {
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

/* No S have nextSibling*/
RegexTreeNode *eliminateS(RegexTreeNode *rtn) {
    RegexTreeNode *l = rtn->firstChild;
    RegexTreeNode *r = rtn->nextSibling;
    if (l != NULL) {
        if (l->symbol == N_S) {
            if (l->firstChild != NULL) {      //S->AS
                eliminateS(rtn->firstChild);
            } else {      //S->empty
                rtn->firstChild = NULL;
                free(l);
            }
        } else {
            eliminateS(l);
        }
    }
    if (r != NULL) {
        if (r->symbol == N_S) {
            if (r->firstChild != NULL) {      //S->AS
                eliminateS(rtn->nextSibling);
            } else {      //S->empty
                rtn->nextSibling = NULL;
                free(r);
            }
        } else {
            eliminateS(r);
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
            /* D-> |A*/
            RegexTreeNode *p = rtn->parent;
            RegexTreeNode *l;
            for (l = p->firstChild; l->nextSibling != NULL && l->nextSibling->symbol != N_D; l = l->nextSibling) {
                ;
            }
            l->nextSibling = rtn->nextSibling;  //NULL
            p->nextSibling = rtn->firstChild;
            p->parent->firstChild = rtn;    //at the beginning , D's parent is A, and this A is always the first child of S
            p->parent = rtn;
            rtn->firstChild = p;
            moveD(p->nextSibling);
            /*
            if (p->nextSibling->firstChild != NULL) {
                moveD(p->nextSibling->firstChild);
            }
            if (p->nextSibling->nextSibling != NULL) {
                moveD(p->nextSibling->nextSibling);
            }
*/
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
        if (rtn->nextSibling != NULL) {
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

            if(l->nextSibling!=NULL){
                rtn = l->nextSibling;
            }else{
                return NULL;
            }
        } else {
            RegexTreeNode *l = rtn->parent->firstChild;
            l->nextSibling = rtn->nextSibling;
            free(rtn);
            if(l->nextSibling!=NULL){
                rtn = l->nextSibling;
            }else{
                return NULL;
            }
        }
    }

    if (rtn->firstChild != NULL) {
        moveC(rtn->firstChild);
    }
    if(rtn->nextSibling != NULL){
        moveC(rtn->nextSibling);
    }
    return rtn;
}
