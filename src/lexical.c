#include <stdlib.h>
#include <string.h>

#include "xregex.h"

const int escapedCharacters[] = {'.', '*', '(', ')', '[', ']', '{', '}', '|', 0};

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
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACK,
    ASTERISK,      /*星号*/
    QUESTION_MARK, /* ? */
    POINT,         /* . */
    VERTICAL_BAR   /* | */
};

typedef struct Symbol {
    enum Type type;
    int value;
} Symbol;

typedef struct SymbolTable {
    Symbol *symbol;
    int tableSize;
    int allocSize;
} SymbolTable;

SymbolTable *initSymbolTable(int size) {
    SymbolTable *symbolTbale = malloc(sizeof(SymbolTable));
    symbolTbale->allocSize = size;
    symbolTbale->symbol = malloc(sizeof(SymbolTable) * size);
    symbolTbale->tableSize = 0;
}

void appendSymbol(SymbolTable *st, Symbol *symbol) {
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
    st->symbol[st->tableSize].value = symbol->value;
    st->tableSize++;
}

void freeSymbolTable(SymbolTable *st) {
    if (st->allocSize != 0) {
        free(st->symbol);
    }
    free(st);
}

//IntVector *getIntArrayFromUtf8(char *regexExpress);

SymbolTable *lexicalAnalyze(char *regexExpress) {
    int regexExpressSize = strlen(regexExpress);
    //IntVector *intArray = getIntArrayFromUtf8(regexExpress);
    int intArraySize = getIntVectorDataSize(regexExpressSize);
    SymbolTable *st = initSymbolTable(intArraySize);
    int isEscaped = 0;
    int c, size;
    while ((size = utf8ToInt(regexExpress, &c)) != 0) {
        if (c == '\\' && isEscaped == 0) {
            isEscaped = 1;
            regexExpress += size;
            continue;
        } else if (isEscaped == 1 && isInEscapedCharacters(c)) {
            Symbol tmp = {CHARACTER, c};
        } else if (isEscaped == 1 && !isInEscapedCharacters(c)) {
            dealLexicalError1();
        }else if (isEscaped == 0)

        isEscaped = 0;
    }

    if (*(regexExpress + size) != 0) {
        dealLexicalError2();
        /*还没有实现*/
    }
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