#include "xregex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
            return 0;
        case 2:
            *result += (int)(*s - 0xc0);
            break;
        case 3:
            *result += (int)(*s - 0xe0);
            break;
        case 4:
            *result += (int)(*s - 0xf0);
            break;
        default:
            return 0;
    }

    for (int i = byte_num - 1; i > 0; i--) {
        s++;
        *result *= 0x40;
        if (0xc0 != (*s) & FIRST_TWO_BITS) {
            return 0;
        } else {
            *result += (*s - 0x80);
        }
    }
    return byte_num;
}

int intToUtf8(int number, char *result) {
    if (number < 0) {
        return 0;
    } else if (number < 0x80) {
        *result = number;
        return 1;
    } else if (number < 0x800) {
        *result = 0xc0 + (number >> 6);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 2;
    } else if (number < 0x10000) {
        *result = 0xe0 + (number >> 12);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 3;
    } else if (number <= 0xf48fbfbf) {
        *result = 0xf0 + (number >> 18);
        result++;
        *result = FIRST_BIT + ((number >> 12) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 4;
    } else {
        return 0;
    }
}

IntVector *initIntVector(int size) {
    IntVector *iv = malloc(sizeof(IntVector));
    iv->allocSize = size;
    iv->dataSize = 0;
    iv->vector = malloc(sizeof(int) * size);
    if (iv->vector == NULL) {
        DEAL_ERRER(-1, "malloc failed\n");
    }
    return iv;
}
void appendIntVector(IntVector *iv, int data) {
    if (iv->dataSize == iv->dataSize) {
        iv->allocSize *= 2;
        int *tmp = realloc(iv->vector, sizeof(int) * iv->allocSize);
        if (tmp == NULL) {
            DEAL_ERRER(-1, "realloc failed\n");
        } else {
            iv->vector = tmp;
        }
    } else {
        ;
    }
    iv->vector[iv->dataSize] = data;
    iv->dataSize++;
}

int getIntVectorDataSize(IntVector *iv) {
    return iv->dataSize;
}

int getIntVectorData(IntVector *iv, int position) {
    if (position > 0 && position < iv->dataSize) {
        return iv->vector[position];
    } else {
        DEAL_ERRER(-2, "vector out of size\n");
    }
}
void setIntVectorData(IntVector *iv, int position, int data) {
    if (position > 0 && position < iv->dataSize) {
        iv->vector[position] = data;
    } else {
        DEAL_ERRER(-2, "vector out of size\n");
    }
}
void freeIntVector(IntVector *iv) {
    if (iv->allocSize > 0) {
        free(iv->vector);
        free(iv);
    } else {
        free(iv);
    }
}

RepeatOption *initRepeatOption() {
    RepeatOption *ro = malloc(sizeof(RepeatOption));
    ro->isRepeative = 0;
    ro->maxTimes = 0;
    ro->munTimes = 0;
    return ro;
}

void deleteXregexChildTree(XregexNode *xn);

XregexTree *initXregexTree() {
    XregexTree *xt = malloc(sizeof(XregexTree));
    xt->root = newXregexNode();
    return xt;
}

XregexNode *newXregexNode() {
    XregexNode *xn = malloc(sizeof(XregexNode));
    xn->child = NULL;
    xn->childNum = 0;
    xn->childSize = 0;
    xn->content = NULL;
    xn->attr = SERIES;
    xn->repeat = initRepeatOption();
    return xn;
}

void addChildXregexNode(XregexNode *parent, XregexNode *child) {
    if (parent->childSize == 0) {
        parent->child = malloc(FIRST_ALLOC_CHILD_SIZE * sizeof(XregexNode *));
        parent->childSize = FIRST_ALLOC_CHILD_SIZE;
    } else if (parent->childSize == parent->childNum) {
        parent->childSize *= 2;
        XregexNode **tmp = realloc(parent->child, parent->childSize * sizeof(XregexNode *) * 2);
        if (tmp == NULL) {
            perror(__FUNCTION__);
        } else {
            parent->child = tmp;
        }
    } else {
        ;
    }

    parent->child[parent->childNum] = child;
    parent->childNum++;
}
int deleteXregexNode(XregexNode *xn, int mode) {
    assert(mode >= 1 && mode <= 3);
    if (mode == FORCE) {
        if (xn->childSize > 0) {
            free(xn->child);
        } else {
            ;
        }
        free(xn->repeat);
        free(xn);
        return 0;
    } else if (mode == RESTRICT) {
        if (xn->childNum > 0) {
            return -1;
        } else if (xn->childSize > 0) {
            free(xn->child);
            ;
        } else {
            ;
        }
        free(xn->repeat);
        free(xn);
        return 0;
    } else if (mode == RECURSIVE) {
        deleteXregexChildTree(xn);

        return 0;
    } else {
        return -2;
    }
}
void freeXregexTree(XregexTree *xt) {
    deleteXregexChildTree(xt->root);
    free(xt);
}

void deleteXregexChildTree(XregexNode *xn) {
    for (int i = 0; i < xn->childNum; i++) {
        deleteXregexChildTree(xn->child[i]);
    }
    deleteXregexNode(xn, FORCE);
}

int analyzeRegexExpress(char *regexExpress, XregexNode *xn);

XregexTree *getXregextTree(char *regexExpress) {
    XregexTree *xt = initXregexTree();
    XregexNode *xn = xt->root;
    analyzeRegexExpress(regexExpress, xn);
    return xt;
}
int analyzeRegexExpress(char *regexExpress, XregexNode *xn) {
    int c;
    int size = utf8ToInt(regexExpress, &c);
    //int isEscaped = 0;
    IntVector *content = initIntVector(FIRST_ALLOC_CONENT_SIZE);
    if (c = '\\') {
        regexExpress += size;
        size = utf8ToInt(regexExpress, &c);
        appendIntVector(xn->content, c);
    } else {
        switch (c) {
            case '(':
                
                break;
            default:
                break;
        }
    }

    /*
    while (c != 0) {
        if (c == '\\') {
            isEscaped = 1;
            regexExpress += size;
            size = utf8ToInt(regexExpress, &c);
            continue;
        } else {
            if (isEscaped == 1) {
                isEscaped = 0;
                switch (c) {
                    case '(':
                        break;
                    case ')':
                        break;
                    case '[':
                        break;
                    case ']':
                        break;
                    case '*':
                        break;
                    case '|':
                        break;
                    default:
                        appendIntVector(content, c);
                        break;
                }
            }
            else{
                appendIntVector(content, c);
            }
        }

        regexExpress += size;
        size = utf8ToInt(regexExpress, &c);
    }
    */
}
XregexNode *priority(char *regexExpress) {
}

int matchParentheses(char *regexExpress);
/*
enum regexNodeAttr childNodeAttr(char *regexExpress) {
    int c;
    int size = utf8ToInt(regexExpress, &c);
    if (c == '(') {
        regexExpress += matchBrockets(regexExpress,'(' ,')');
    } else if (c == '[') {
        regexExpress += matchBrockets(regexExpress, '[', ']');
    } else if(c == '\\'){
        regexExpress += size;
        regexExpress += utf8ToInt(regexExpress,&c);
    }else{
        regexExpress += size;
    }

    do{
        size = utf8ToInt(regexExpress, &c);
        regexExpress += size;
        if(c == '*'){
            
        }
    }
}


int matchBrockets(char *regexExpress,char leftBrocket, char rightBrocket){

    int c;
    int size = utf8ToInt(regexExpress, &c);
    int sizeSummation = size;
    int isEscaped = 0;
    if (c != leftBrocket) {
        return -1;
    } else {
        int isNotMatched = 1;
        while (c != 0 && isNotMatched) {
            size = utf8ToInt(regexExpress, &c);
            if (c == '\\' && !isEscaped) {
                isEscaped = 1;
                regexExpress += size;
                sizeSummation += size;
                continue;
            }

            if (c == leftBrocket && !isEscaped) {
                isNotMatched++;
            } else if (c == rightBrocket && !isEscaped) {
                isNotMatched--;
            } else {
                ;
            }

            if (isEscaped) {
                isEscaped = !isEscaped;
            }

            regexExpress += size;
            sizeSummation += size;
        }
    }

    return sizeSummation;
}
*/
typedef struct XregexRule{
    /*终结符是正数 非终结符是负数*/
    int ruleLeftSide;
    int **ruleRightSide;
    int ruleNum;
    int allocSize;
} XregexRule;
