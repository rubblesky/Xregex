#include "xregex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_STRING -128
#define ANONYMOUS_CAPTURE_LEFT -129  /*(:*/
#define ANONYMOUS_CAPTURE_RIGHT -130 /*:)*/
#define NAMED_CAPTURE_LEFT -131      /*(=*/
#define NAMED_CAPTURE_RIGHT -132     /*=)*/

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

IntVector *preprocess(char *regexExpress);
IntVector *getRegexVector(char *regexExpress);
IntVector *dealMetacharacter(IntVector *regexVector);
int unfoldRepeat(IntVector *regexVector);
int analyzeRegexExpress(char *regexExpress, XregexNode *xn) {
    IntVector *regex = preprocess(regexExpress);
}

IntVector *preprocess(char *regexExpress) {
    IntVector *rv = getRegexVector(regexExpress);
    rv = dealMetacharacter(rv);
    unfoldRepeat(rv);
}

IntVector *getRegexVector(char *regexExpress) {
    IntVector *regexVector = initIntVector(strlen(regexExpress));
    int i;
    while (0 != utf8ToInt(regexExpress++, &i) && i != 0) {
        if (i == '\\') {
            if (0 == utf8ToInt(&i, regexExpress++) || 0 == i) {
                break;
            } else {
                /*预处理器不负责纠错 由此出现奇怪的bug一律看作特性或UB*/
                /* 
                \n之类的转义字符不用考虑 
                但是如果需要处理\b \w 这样原来 ASCII未定义的字符就要小心了 没准对C语言来说是UB
                */
                appendIntVector(regexVector, i);
            }
        } else {
            switch (i) {
                    /*先把它们设置为负值 处理元字符时还会调整*/
                case '(':
                case ')':
                case '[':
                case ']':
                case '*':
                case ':':
                case '=':
                case '^':
                case '-':
                    appendIntVector(regexVector, -i);
                    break;
                default:
                    appendIntVector(regexVector, i);
                    break;
            }
        }
    }
    return regexVector;
}

void setNegative(IntVector *iv, int pos) {
    /*这里其实应该加上错误处理的*/
    int data = getIntVectorData(iv, pos);
    if (data > 0) {
        setIntVectorData(iv, pos, -data);
    } else {
        return;
    }
}

void setPositive(IntVector *iv, int pos) {
    int data = getIntVectorData(iv, pos);
    if (data < 0) {
        setIntVectorData(iv, pos, -data);
    } else {
        return;
    }
}

IntVector *dealMetacharacter(IntVector *regexVector) {
    int data;
    int isInBracket = 0;
    for (int i = 0; data = getIntVectorData(regexVector, i); i++) {
        if (data = -'[') {
            isInBracket = 1;
        } else if (data = -']') {
            isInBracket = 0;
        } else {
            ;
        }

        if (isInBracket) {
            switch (data) {
                case -'(':
                case -')':
                case -'*':
                case -':':
                case -'=':
                    setPositive(regexVector, i);
            }
        } else {
            switch (data) {
                case -'-':
                case -'^':
                    setPositive(regexVector, i);
                    break;
                default:
                    break;
            }
        }
    }
}

static int getRepeatTimes(IntVector *iv, int start, int *max, int *min) {
    /*max = 0 代表正无穷*/
    int data;
    if ((data = getIntVectorData(iv, start)) != -'{') {
        DEAL_ERRER(-1, "unkown repeat start postion");
    } else {
        int times[2] = {0, 0};
        int *tp = &times;
        int isCut = 0, isCount = 0;
        for (int i = start + 1; (data = getIntVectorData(iv, i)) != -'}'; i++) {
            if (data = ',') {
                if (tp == times) {
                    tp++;
                    isCount = 0;
                    isCut = 0;
                } else {
                    /*error*/
                }
            } else if (data >= '0' && data <= '9' && !isCut) {
                *tp *= 10;
                *tp += data - '0';
                isCount = 1;
            } else if (isCount && (data == ' ' || data == '\t')) {
                isCut = 1;
            } else {
                /*error*/
            }
        }
        *min = times[0];
        *max = times[1];
        if (*max < *min && *max != 0) {
            /*error*/
        } else {
            return 0;
        }
    }
}
static int getMatch(IntVector *iv, int end, const int left, const int right) {
    int data;
    int isNotMatch = 0;
    int i;
    for (i = end - 1; i >= 0 && !isNotMatch && i != end - 1; i--) {
        data = getIntVectorData(iv, i);
        if (data == right) {
            isNotMatch++;
        } else if (data == left) {
            isNotMatch--;
        }
    }
    return end - i - 1;
}
static int getRepeatScope(IntVector *iv, int end) {
    int i = end - 1;
    if (i < 0) {
        return -1;
        /*error*/
    } else {
        int data = getIntVectorData(iv, i);
        switch (data) {
            case -')':
                return getMatch(iv, end, -'(', -')');
            case -']':
                return getMatch(iv, end, -'[', -']');
            case ANONYMOUS_CAPTURE_RIGHT:
                return getMatch(iv, end, ANONYMOUS_CAPTURE_LEFT, ANONYMOUS_CAPTURE_RIGHT);
            case NAMED_CAPTURE_RIGHT:
                return getMatch(iv, end, NAMED_CAPTURE_LEFT, NAMED_CAPTURE_RIGHT);
            default:
                if (data > 0) {
                    return 1;
                } else {
                    /*error*/
                    return -1;
                }
                break;
        }
    }
}
/*
消除不与括号相连的 : 和 = 
同时把与括号相连的按照语义添加到newregexVector中
*/
static int addFunction(IntVector *oldRegexVector, IntVector *newRegexVector, int pos) {
    int data = getIntVectorData(oldRegexVector, pos);
    int i = pos;
    if ('(' == -data) {
        if (i + 1 < getIntVectorDataSize(oldRegexVector)) {
            data = getIntVectorData(oldRegexVector, i + 1);
            if (':' == -data) {
                i++;
                appendIntVector(newRegexVector, ANONYMOUS_CAPTURE_LEFT);
            } else if ('=' == -data) {
                i++;
                appendIntVector(newRegexVector, NAMED_CAPTURE_LEFT);
            } else {
                appendIntVector(newRegexVector, -'(');
            }
        } else {
            /*error*/
        }
    } else if (':' == -data) {
        if (i + 1 < getIntVectorDataSize(oldRegexVector)) {
            data = getIntVectorData(oldRegexVector, i - 1);
            if (')' == -data) {
                i++;
                appendIntVector(newRegexVector, ANONYMOUS_CAPTURE_RIGHT);
            } else {
                /*添加正数*/
                appendIntVector(newRegexVector, ':');
            }
        }
    } else if ('=' == -data) {
        if (i + 1 < getIntVectorDataSize(oldRegexVector)) {
            data = getIntVectorData(oldRegexVector, i - 1);
            if (')' == -data) {
                i++;
                appendIntVector(newRegexVector, NAMED_CAPTURE_RIGHT);
            } else {
                /*添加正数*/
                appendIntVector(newRegexVector, '=');
            }
        }
    } else {
        return -1;
    }
    return i;
}

static void repeat(IntVector *iv,IntVector *content,int times){
    int data = getIntVectorData(content, 0);
    if (data == ANONYMOUS_CAPTURE_LEFT || data == NAMED_CAPTURE_LEFT ){
        appendIntVector(iv, -'(');
        for (int r = 0; r < times; r++) {
            for (int i = 1; i < getIntVectorDataSize(content) - 1; i++) {
                appendIntVector(iv, getIntVectorData(content, i));
            }
        }
        appendIntVector(iv, -')');
    }else{
        for (int r = 0; r < times; r++) {
            for (int i = 0; i < getIntVectorDataSize(content); i++) {
                appendIntVector(iv, getIntVectorData(content, i));
            }
        }
    }
}


static void addRetition(IntVector *newRegexVector, IntVector *content, int max, int min) {
    if (max == 0) {
    } else {
        for (int i = 0; i < getIntVectorDataSize(content); i++) {
            deleteIntVectorLastData(newRegexVector);
        }
        appendIntVector(newRegexVector, -'(');

        for (int t = max; t > min;t--){
            for (int i = 0; i < getIntVectorDataSize(content); i++) {
                appendIntVector(newRegexVector, getIntVectorData(content, i));
            }
            repeat(newRegexVector, content, max - 1);
            appendIntVector(newRegexVector, -'|');
        }
        /*min times*/
        if(min == 0){
            int data = getIntVectorData(content, 0);
            if(data == ANONYMOUS_CAPTURE_LEFT){
                appendIntVector(newRegexVector, ANONYMOUS_CAPTURE_LEFT);
                appendIntVector(newRegexVector, EMPTY_STRING);
                appendIntVector(newRegexVector, ANONYMOUS_CAPTURE_RIGHT);
            }else if(data == NAMED_CAPTURE_LEFT){
                appendIntVector(newRegexVector, NAMED_CAPTURE_LEFT);
                appendIntVector(newRegexVector, EMPTY_STRING);
                appendIntVector(newRegexVector, NAMED_CAPTURE_RIGHT);
            }else{
                appendIntVector(newRegexVector, EMPTY_STRING);
            }
        }
        appendIntVector(newRegexVector, -')');
    }
}

int unfoldRepeat(IntVector *regexVector) {
    int data;
    IntVector *newRegexVector = initIntVector(getIntVectorDataSize(regexVector) * 2);
    for (int i = 0; i < getIntVectorDataSize(regexVector); i++) {
        data = getIntVectorData(regexVector, i);
        if ('{' == -data) {
            int min, max;
            /*记得加上错误处理*/
            getRepeatTimes(regexVector, i, &max, &min);
            int scope = getRepeatScope(regexVector, i);
            if (scope > 0) {
                int start = i - scope;
                IntVector *content = initIntVector(scope);
                for (int t = start; t < i; t++) {
                    appendIntVector(content, getIntVectorData(regexVector, t));
                }
                addRetition(newRegexVector, content, max, min);
                freeIntVector(content);
            } else {
                /*error*/
            }
        } else if ('(' == -data || ':' == -data || '=' == -data) {
            i = addFunction(regexVector, newRegexVector, i);
        } else {
            appendIntVector(newRegexVector, data);
        }
    }
}