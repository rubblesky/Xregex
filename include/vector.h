#ifndef VECTOR_H
#define VECTOR_H

typedef void (*assignFunction)(void *assignedValue, void *fromValue);

typedef struct Vector {
    void *vector;
    int typeSize;
    int dataSize;
    int allocSize;
    /*对于复杂结构 这应该是一个深度复制函数*/
    assignFunction assign;
} Vector;
#define INIT_VECTOR(size,type,assignFunction) initVector(size,sizeof(type),assignFunction)
Vector *initVector(int VectorSize, int typeSize, assignFunction assign);
void freeVector(Vector *v);
int appendVector(Vector *v, void *element);
int setValueVector(Vector *v, int pos, void *value);
int getValueVector(Vector *v, int pos, void *value);
int getVectorSize(Vector *v);
Vector *copyVector(Vector *v);
void *getVector(Vector *v);
void *getReferenceVector(Vector *v, int pos);
int deleteLastElement(Vector *v);
int exchangePositionVector(Vector *v, int pos1, int pos2);
int moveToLastVector(Vector *v, int pos);
int isInVector(Vector *v, void *element);
int getPostionVector(Vector *v, void *element);

int moveToLastAndDeleteVector(Vector *v, void *element);
#endif