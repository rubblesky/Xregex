#ifndef XREGEX_H
#define XREGEX_H

#define FORCE 1
#define RESTRICT 2
#define RECURSIVE 3

void dealError(int exitStauts,const char *errorPosition, char *msg);
#define DEAL_ERRER(exitStauts, msg) dealError(exitStauts, __FUNCTION__, msg)
/*
返回utf-8码占据的字节数
如果出错返回0
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
void freeIntVector(IntVector *iv);

typedef struct RepeatOption {
    int isRepeative;
    int munTimes;
    int maxTimes;
} RepeatOption;

RepeatOption *initRepeatOption();

enum regexNodeAttr {
    SERIES,   /*串联*/
    PARALLEL, /*并联*/
    OPTIONAL, /*可选*/
};
typedef struct XregexNode {
    struct XregexNode **child;
    int childNum;
    int childSize;
    enum regexNodeAttr attr;
    RepeatOption *repeat;
    IntVector *content;
} XregexNode;

typedef struct XregexTree {
    XregexNode *root;

} XregexTree;

XregexTree *initXregexTree();
XregexNode *newXregexNode();
void addChildXregexNode(XregexNode *parent, XregexNode *child);
/*删除单个节点 
mode = FORCE 不检查节点是否有子节点
mode = RESTRICT 节点有子节点时不执行删除操作并返回 -1
 */
int deleteXregexNode(XregexNode *xn, int mode);
void freeXregexTree(XregexTree *xt);

XregexTree *getXregextTree(char *regexExpress);
#endif