#ifndef XREGEX_H
#define XREGEX_H

#define FORCE 1
#define RESTRICT 2
#define RECURSIVE 3

char *errorString;
typedef int LongChar;
void dealError(int exitStauts,const char *errorPosition, char *msg);
#define DEAL_ERRER(exitStauts, msg) dealError(exitStauts, __FUNCTION__, msg)
/*
返回utf-8码占据的字节数
如果出错返回-1
*/
int utf8ToInt(unsigned char *s, int *result);
/*
函数不会分配内存空间
确保传入的result参数指向有效的地址
*/
int intToUtf8(int number, char *result);

typedef struct IntVector {
    int *vector;
    int allocSize;
    int dataSize;
} IntVector;

IntVector *initIntVector(int size);
void appendIntVector(IntVector *iv, int data);
int getIntVectorDataSize(IntVector *iv);
int getIntVectorData(IntVector *iv, int position);
void setIntVectorData(IntVector *iv, int position, int data);
IntVector *copyIntVector(IntVector *iv);
/*只删除数据 不释放内存*/
void deleteIntVectorData(IntVector *iv);
void deleteIntVectorLastData(IntVector *iv);
void freeIntVector(IntVector *iv);

IntVector * transExpress(char *express);
IntVector * getEscapeCharacterExpress(IntVector *express);
enum regexNodeAttr {
    SERIES,   /*串联*/
    PARALLEL, /*并联*/
    OPTIONAL, /*可选*/
};

enum LexicalResultType {
    CHARACTER_STRING,
    LEFT_PARENTHESIS,   /* ( */
    RIGHT_PARENTHESIS,  /* ) */
    LEFT_BRACKET,       /* [ */
    RIGHT_BRACKET,      /* ] */
    ASTERISK,           /* * */
    VERTICAL_BAR,       /* | */
    EMPTY_STRING,
    END
};

typedef struct LexicalResult{
    IntVector *string;
    enum LexicalResultType type;
} LexicalResult;

LexicalResult * lexicalAnalyse(IntVector * express);


enum Symbol{
    N_S,N_A,N_B,N_C,N_D,N_E,N_F,
    T,START
};


typedef struct ASTNode{
    struct ASTNode *left;
    struct ASTNode*right;
    enum Symbol symbol;
    IntVector * string;
} ASTNode;
ASTNode *parse(char express[]);

#endif