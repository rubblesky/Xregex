#include "xregex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_STRING -128

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

int unfoldRepeat(char *regexExpress, IntVector *iv);
int analyzeRegexExpress(char *regexExpress, XregexNode *xn) {
    IntVector* regex = preprocess(regexExpress);
}

IntVector *preprocess(char* regexExpress){
    IntVector *iv = initIntVector(strlen(regexExpress));
    int i;
    while (0 != utf8ToInt(regexExpress++,&i) && i != 0){
        if(i == '\\'){
            if(0 == utf8ToInt(&i,regexExpress++) || 0 == i){
                break;
            }else{
                /*预处理器不负责纠错 由此出现奇怪的bug一律看作特性或UB*/
                /* 
                \n之类的转义字符不用考虑 
                但是如果需要处理\b \w 这样原来 ASCII未定义的字符就要小心了 没准对C语言来说是UB
                */
                appendIntVector(iv, i);
            }
        }else if(i == '{'){
            unfoldRepeat(regexExpress - 1, iv);
        }else {
            switch (i)
            {
            case '(':
            case ')':
            case '[':
            case ']':
            case '*':
                appendIntVector(iv, -i);
                break;
            default:
                appendIntVector(iv, i);
                break;
            }
        }
        
    }
}

int unfoldRepeat(char* regexExpress,IntVector *iv){
    int i = getIntVectorData(iv, -1);
    int start = -2, end = -2;
    if (')' == i) {
        int tmp;
        while('(' != getIntVectorData(iv,start)){
            start--;
        }
        start += 1;
    } else if (']' == i) {
        while ('[' != getIntVectorData(iv, start)) {
            start--;
        }
        start += 1;
    } else {
        ;
    }
    int times[2];
    int *tp = &times;
    for (char *r = regexExpress; *r != '}' && r != '\0'; r++) {
        int n = 0;
        if (*r == ',') {
            if (tp == times) {
                *tp++ = n;
            } else {
                /*error*/
            }
        } else if (*r >= '0' && *r <= '9') {
            n *= 10;
            n += *r - '0';
        } else if (*r == ' ' || *r == '\t') {
            /*换行符和回车符都视为错误吧 空格不会中断数字*/
            continue;
        } else {
            /*error*/
        }
    }
    /*接下来进行展开 等到语法设计出来之后再写*/
}