#ifndef VECTOR_H
#define VECTOR_H

typedef void (*assignFunction)(void *assignedValue, void *fromValue);
typedef void (*freeFunction)(void *element);
typedef struct Vector {
    void *vector;
    int typeSize;
    int dataSize;
    int allocSize;
    /*对于复杂结构 这应该是一个深度复制函数*/
    assignFunction assign;
    /*用于释放assign使用的内存*/
    freeFunction freeAssign;
} Vector;
#define INIT_VECTOR(size, type, assignFunction, freeAssignFunction) initVector(size, sizeof(type), assignFunction, freeAssignFunction)
Vector *initVector(int VectorSize, int typeSize, assignFunction assign, freeFunction freeAssign);
void freeVector(Vector *v);
void clearVecter(Vector *v);
int appendVector(Vector *v, void *element);
int setValueVector(Vector *v, int pos, void *value);
int getValueVector(Vector *v, int pos, void *value);
int getVectorSize(Vector *v);
Vector *copyVector(Vector *v);
void *getReferenceVector(Vector *v, int pos);
int deleteLastElement(Vector *v);
int exchangePositionVector(Vector *v, int pos1, int pos2);
int moveToLastVector(Vector *v, int pos);
int isInVector(Vector *v, void *element);
int getPostionVector(Vector *v, void *element);

int moveToLastAndDeleteVector(Vector *v, void *element);
#endif