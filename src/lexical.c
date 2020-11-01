
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xregex.h"
#define LEXICAL_ERROR_DISPLAY 10

const int metaCharacters[] = {'.', '*', '(', ')', '[', ']', '{', '}', '|', '\\', 0};
const int escapedCharacters[] = {'n', 'r', 't'};
int isInMetaCharacters(int c) {
    /* 先用循环实现 之后看需要是否要改成hash*/
    int i;
    for (i = 0; metaCharacters[i] != 0; i++) {
        if (metaCharacters[i] == c) {
            break;
        }
    }
    if (metaCharacters[i] == 0) {
        return 0;
    } else {
        return 1;
    }
}
int isInEscapedCharacters(int c) {
    /* 先用循环实现 之后看需要是否要改成hash*/
    int i;
    for (i = 0; escapedCharacters[i] != 0; i++) {
        if (escapedCharacters[i] == c) {
            break;
        }
    }
    if (escapedCharacters[i] == 0) {
        return 0;
    } else {
        return 1;
    }
}

enum Type {

    NUMBER,
    CHARACTER,
    REPEAT,
    VERTICAL_BAR, /* | */
    SET,          /* [] */
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    QUESTION_MARK, /* ? */
    ANONYMOUS_CAPTURE,
    /*
    NUMBER,
    CHARACTER,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACK,
    ASTERISK,      /*星号*
    QUESTION_MARK, /* ? *
    POINT,         /* . *
    VERTICAL_BAR   /* | *
    */
};

typedef struct Symbol {
    enum Type type;
    IntVector *value;
} Symbol;

typedef struct SymbolTable {
    Symbol *symbol;
    int tableSize;
    int allocSize;
} SymbolTable;

SymbolTable *initSymbolTable(int size) {
    SymbolTable *symbolTbale = malloc(sizeof(SymbolTable));
    symbolTbale->allocSize = size;
    symbolTbale->tableSize = 0;
    symbolTbale->symbol = malloc(sizeof(Symbol) * size);
}

void appendSymbol(SymbolTable *st, Symbol *symbol) {
    /*这个函数有问题 value的数据类型*/
    if (st->tableSize >= st->allocSize) {
        st->allocSize *= 2;
        Symbol *tmp = realloc(st->symbol, st->allocSize * sizeof(Symbol));
        if (tmp != NULL) {
            st->symbol = tmp;
        } else {
            DEAL_ERRER(-1, "realloc failed\n");
        }
    } else {
        ;
    }
    st->symbol[st->tableSize].type = symbol->type;
    st->symbol[st->tableSize].value = copyIntVector(symbol->value);
    st->tableSize++;
}

void freeSymbolTable(SymbolTable *st) {
    for (int i = 0; i < st->tableSize; i++) {
        freeIntVector(st->symbol[i].value);
    }
    if (st->allocSize != 0) {
        free(st->symbol);
    }
    free(st);
}

IntVector *getIntArrayFromUtf8(char *regexExpress);
int getEscapedCharacterValue(IntVector *intArray, int pos);
int dealEscapedCharacter(IntVector *intArray, int pos, SymbolTable *st);
int dealDefiniteRepeat(IntVector *intArray, int pos, SymbolTable *st);
int dealParallel(IntVector *intArray, int pos, SymbolTable *st);
void lexicalError(IntVector *intArray, int pos, char *msg);
SymbolTable *lexicalAnalyze(char *regexExpress) {
    IntVector *intArray = getIntArrayFromUtf8(regexExpress);
    int intArraySize = getIntVectorDataSize(intArray);
    SymbolTable *st = initSymbolTable(intArraySize);

    int c;
    Symbol tmp;
    tmp.value = initIntVector(5);
    for (int i = 0; i < intArraySize; i++) {
        c = getIntVectorData(intArray, i);
        switch (c) {
            case '\\':
                i += dealEscapedCharacter(intArray, i, st);
                break;
            case '{':
                i += dealDefiniteRepeat(intArray, i, st);
                break;
            case '[':
                i += dealParallel(intArray, i, st);
                break;
            case '*':
                tmp.type = REPEAT;
                deleteIntVectorData(tmp.value);
                appendIntVector(tmp.value, -1);
                appendSymbol(st, &tmp);
            case '}':
            case ']':
                lexicalError(intArray, i, "不合法的匹配：");
            default:
                tmp.type = CHARACTER;
                deleteIntVectorData(tmp.value);
                appendIntVector(tmp.value, c);
                appendSymbol(st, &tmp);
                break;
        }
    }
    freeIntVector(tmp.value);
    return st;
}

void lexicalError(IntVector *intArray, int pos, char *msg) {
    int errorPos = pos;
    putchar('\n');
    printf(msg);
    putchar('\n');
    if (pos > LEXICAL_ERROR_DISPLAY / 2) {
        pos -= LEXICAL_ERROR_DISPLAY / 2;
    } else {
        pos = 0;
    }
    char c[9];
    int vectorSize = getIntVectorDataSize(intArray);
    for (int i = 0; i < LEXICAL_ERROR_DISPLAY && i < vectorSize; i++) {
        c[intToUtf8(getIntVectorData(intArray, pos + i), c)] = '\0';
        if (errorPos != pos + i) {
            //但愿不是未定义行为
            printf(c);
        } else {
            printf("\033[31m");
            printf(c);
            printf("\033[0m");
        }
    }
    putchar('\n');
    exit(-1);
}

int dealEscapedCharacter(IntVector *intArray, int pos, SymbolTable *st) {
    int i = pos;
    if (getIntVectorData(intArray, i) != '\\') {
        return 0;
    } else {
        i++;
        Symbol tmp;
        tmp.value = initIntVector(5);
        int c = getIntVectorData(intArray, i);
        int num = 0;
        if (c <= '9' && c >= '0') {
            while (c <= '9' && c >= '0') {
                num *= 10;
                num += c - '0';
                i++;
                c = getIntVectorData(intArray, i);
            }
            deleteIntVectorData(tmp.value);
            appendIntVector(tmp.value, c);
            tmp.type = ANONYMOUS_CAPTURE;
            appendSymbol(st, &tmp);
        } else if ((c = getEscapedCharacterValue(intArray, i)) != -1) {
            i += 2;
            deleteIntVectorData(tmp.value);
            appendIntVector(tmp.value, c);
            tmp.type = CHARACTER;
            appendSymbol(st, &tmp);
        } else {
            lexicalError(intArray, pos, "无法识别的转义字符：");
        }
    }
    return i - pos;
}

int getEscapedCharacterValue(IntVector *intArray, int pos) {
    int c = getIntVectorData(intArray, pos);
    if (isInEscapedCharacters(c)) {
        switch (c) {
            case 'r':
                return '\r';
            case 'n':
                return '\n';
            case 't':
                return '\t';
            default:
                break;
        }
    } else if (isInMetaCharacters(c)) {
        return c;
    } else {
        return -1;
    }
}

IntVector *getIntArrayFromUtf8(char *regexExpress) {
    IntVector *intArray = initIntVector(strlen(regexExpress));
    int c, size;
    while (0 != (size = utf8ToInt(regexExpress, &c))) {
        if (c == 0) {
            appendIntVector(intArray, 0);
            break;
        }
        regexExpress += size;
        appendIntVector(intArray, c);
    }
    return intArray;
}
/*
static int setLongCharArrayByChar(LongChar *ls, int lsSize, unsigned char *s) {
    int i;
    for (i = 0; s[i] != '\0' && i < lsSize; i++) {
        ls[i] = s[i];
    }
    if (i == lsSize) {
        return -1;
    } else {
        return i;
    }
}
static void addEdge(AutoMaton *am,int start ,int end,enum MatchMode matchMode,char *s){
    LongChar matchContent[50];
    int size = setLongCharArrayByChar(matchContent, 50, s);
    Match *m = initMatch(SEQUENCE, matchContent, 5);
    setMatch(m, matchMode, matchContent, size);
    appendEdgeByIndex(am, start, end, m);
    free(m);
}
*/
int dealDefiniteRepeat(IntVector *intArray, int pos, SymbolTable *st) {
    /*
    AutoMaton *am = initAutoMaton();
    Status *s = initStatus(0);
    for (int i = 0; i < 5; i++) {
        appendStatus(am,s);
    }
    setFinalStatus(s, 1);
    appendStatus(am, s);
    freeStatus(s);

    addEdge(am, 0, 1, SEQUENCE, "{");
    addEdge(am, 1, 1, SEQUENCE, "\n\r\t ");
    addEdge(am, 1, 2, RANGE, "09");
    addEdge(am, 1, 3, SEQUENCE, ",");
    addEdge(am, 2, 2, SEQUENCE, "\n\r\t ");
    addEdge(am, 2, 3, SEQUENCE, ",");
    addEdge(am, 3, 3, SEQUENCE, "\n\t\r ");
    addEdge(am, 3, 4, RANGE, "09");
    addEdge(am, 3, 5, SEQUENCE, "}");
    addEdge(am, 4, 4, SEQUENCE, "\n\r\t ");
    addEdge(am, 4, 5, SEQUENCE, "}");
    addEdge(am, 2, 5, SEQUENCE, "}");
    int r = runAutoMation(am, 0, intArray->vector + pos);
    freeAutoMaton(am);
    return r;
*/
}

int dealParallel(IntVector *intArray, int pos, SymbolTable *st) {
    int i = pos;
    if (getIntVectorData(intArray, i) != '[') {
        return 0;
    } else {
        i++;
        Symbol tmp;
        tmp.value = initIntVector(5);
        int c = getIntVectorData(intArray, i);
        while (c != ']' && c != '\0') {
            switch (c) {
                case '\\':
                    appendIntVector(tmp.value, getEscapedCharacterValue(intArray, i));
                    i++;  //后面还有一个i++
                    break;

                default:
                    appendIntVector(tmp.value, c);
                    break;
            }
            i++;
            c = getIntVectorData(intArray, i);
        }
        if (c == '\0') {
            lexicalError(intArray, i, "未匹配的 \'[\' :");
        } else {
            appendSymbol(st, &tmp);
        }
    }
    return i - pos;
}