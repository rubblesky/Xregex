#ifndef VECTOR_H
#define VECTOR_H

typedef void (*assignFunction)(void *assignedValue, void *fromValue,...);

typedef struct Vector {
    void *vector;
    int typeSize;
    int dataSize;
    int allocSize;

    assignFunction assign;
} Vector;
#define INIT_VECTOR(size,type,assignFunction) initVector(size,sizeof(type),assignFunction)
Vector *initVector(int VectorSize, int typeSize, assignFunction assign);
void freeVector(Vector *v);
int appendVector(Vector *v, void *element);
int setValueVector(Vector *v, int pos, void *value);
int getValueVector(Vector *v, int pos, void *value);
Vector *copyVector(Vector *v);

#endif