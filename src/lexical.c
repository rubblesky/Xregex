#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "xregex.h"

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
    IntVector * value;
} Symbol;

typedef struct SymbolTable {
    Symbol *symbol;
    int tableSize;
    int allocSize;
} SymbolTable;

SymbolTable *initSymbolTable(int size) {
    SymbolTable *symbolTbale = malloc(sizeof(SymbolTable));
    symbolTbale->allocSize = size;
    symbolTbale->symbol = malloc(sizeof(Symbol) * size);
    //symbolTbale->symbol->value = initIntVector(5);
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
    for (int i = 0; i < st->tableSize;i++){
        freeIntVector(st->symbol[i].value);
    }
    if(st->allocSize != 0){
        free(st->symbol);
    }
    free(st);
}

IntVector *getIntArrayFromUtf8(char *regexExpress);
int dealDefiniteRepeat(IntVector *intArray, int pos, SymbolTable *st);
SymbolTable *lexicalAnalyze(char *regexExpress) {
    IntVector *intArray = getIntArrayFromUtf8(regexExpress);
    int intArraySize = getIntVectorDataSize(intArray);
    SymbolTable *st = initSymbolTable(intArraySize);

    int isEscaped = 0;
    int c;
    Symbol tmp;
    tmp.value = initIntVector(5);
    for (int i = 0; i < intArraySize; i++) {
        c = getIntVectorData(intArray, i);
        if (c == '\\' && isEscaped == 0) {
            isEscaped = 1;
            continue;
        }
        if (isEscaped == 1) {
            if (isInEscapedCharacters(c)) {
                tmp.type = CHARACTER;
                appendIntVector(tmp.value, c);
                appendSymbol(st, &tmp);
            } else if (isInMetaCharacters(c)) {
                deleteIntVectorData(tmp.value);
                Symbol tmp;
                tmp.type = CHARACTER;
                switch (c) {
                    case 'n':
                        appendIntVector(tmp.value, '\n');
                        break;
                    case 'r':
                        appendIntVector(tmp.value, '\r');
                        break;
                    case 't':
                        appendIntVector(tmp.value, '\t');
                        break;
                    default:
                        fprintf(stderr, "未处理的元字符\n");
                        break;
                }
                appendSymbol(st, &tmp);
            } else {
                /*不合法的转义*/
                /*可能还会在这加上匿名捕获的内容*/
                //dealLexicalError1();
            }
        } else {
            switch (c) {
                case '{':
                    i += dealDefiniteRepeat(intArray,i,st);
                    break;
                case '[':
                    //i += dealParallel(intArray,st);
                    break;
                case '*':
                    //tmp.type = REPEAT;
                    //tmp.value = -1;
                case '}':
                case ']':
                    /*不合法的匹配*/
                    //dealLexicalError3();
                default:
                    tmp.type = CHARACTER;
                    break;
            }
        }
        isEscaped = 0;
    }
    freeIntVector(tmp.value);
    return st;
}

IntVector *getIntArrayFromUtf8(char *regexExpress) {
    IntVector *intArray = initIntVector(strlen(regexExpress));
    int c, size;
    while (0 != (size = utf8ToInt(regexExpress, &c))) {
        regexExpress += size;
        appendIntVector(intArray, c);
    }
    return intArray;
}
int dealDefiniteRepeat(IntVector *intArray,int pos,SymbolTable* st){
    int i = pos;
    if (getIntVectorData(intArray, i) != '{') {
        return 0;
    } else {
        i++;
        int c;
        int isEnd = 0;
        Symbol tmp;
        tmp.value = initIntVector(5);
        int num = 0;
        int hasComma = 0;
        while ((c = getIntVectorData(intArray, i)) != '\0' && !isEnd) {
            i++;
            switch (c)
            {
            case '}':
                if(hasComma){
                    if(num == 0){
                        appendIntVector(tmp.value, -1);
                    }else{
                        appendIntVector(tmp.value, num);
                    }
                }else{
                    appendIntVector(tmp.value, getIntVectorData(tmp.value, 0));
                }
                isEnd = 1;
                break;
            case ',':
                appendIntVector(tmp.value, num);
                num = 0;
                hasComma = 1;
                break;
            case ' ':
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                num *= 10;
                num += c - '0';
                break;
            default:
                break;
            }
        }

        appendSymbol(st, &tmp);
        freeIntVector(tmp.value);
    }

    return i - pos;
}