#include "vector.h"

#include <stdlib.h>
#include <string.h>
Vector *initVector(int VectorSize, int typeSize, assignFunction assign) {
    Vector *v = malloc(sizeof(Vector));
    if (v == NULL) {
        return NULL;
    } else {
        v->allocSize = VectorSize;
        v->typeSize = typeSize;
        v->dataSize = 0;
        v->assign = assign;
        v->vector = malloc(typeSize * VectorSize);
        if (v->vector == NULL) {
            free(v);
            return NULL;
        } else {
            return v;
        }
    }
}

void freeVector(Vector *v) {
    if (v == NULL)
        return;
    else if (v->allocSize != 0) {
        free(v->vector);
    }
    free(v);
}

int appendVector(Vector *v, void *element) {
    if (v->dataSize == v->allocSize) {
        Vector *tmp = realloc(v->vector, v->allocSize * 2 * v->typeSize);
        if (tmp == NULL) {
            return -1;
        } else {
            v->allocSize *= 2;
        }
    }

    (*v->assign)(&(((unsigned char *)(v->vector))[(v->dataSize) * (v->typeSize)]), element);
    v->dataSize++;
    return v->dataSize - 1;
}

int setValueVector(Vector *v, int pos, void *value) {
    if (pos > 0 && pos < v->dataSize) {
        (*v->assign)(&(((unsigned char *)(v->vector))[pos * (v->typeSize)]), value);
        return 0;
    } else {
        return -1;
    }
}

int getValueVector(Vector *v, int pos, void *value) {
    if (pos > 0 && pos < v->dataSize) {
        (*v->assign)(&(((unsigned char *)(v->vector))[pos * (v->typeSize)]), value);
        return 0;
    } else {
        return -1;
    }
}

int getVectorSize(Vector *v) {
    return v->dataSize;
}

Vector *copyVector(Vector *v) {
    Vector *tmp = initVector(v->allocSize, v->typeSize, v->assign);
    if (tmp == NULL) {
        return NULL;
    } else {
        tmp->dataSize = v->dataSize;
        memcpy(tmp->vector, v->vector, v->typeSize * v->dataSize);
        return tmp;
    }
}
/*直接返回地址*/
void *getVector(Vector *v) {
    return v->vector;
}
void *getReferenceVector(Vector *v, int pos) {
    return &(((unsigned char *)(v->vector))[pos * (v->typeSize)]);
}

int deleteLastElement(Vector *v) {
    if (v->dataSize > 0) {
        v->dataSize--;
        return 0;
    } else {
        return -1;
    }
}

int exchangePositionVector(Vector *v, int pos1, int pos2) {
    if (pos1 < 0 || pos1 >= v->dataSize || pos2 < 0 || pos2 > v->dataSize) {
        return -1;
    } else {
        void *tmp = malloc(v->typeSize);
        memcpy(tmp, &(v->vector[pos1]), v->typeSize);
        memcpy(&(v->vector[pos1]), &(v->vector[pos2]), v->typeSize);
        memcpy(&(v->vector[pos2]), tmp, v->typeSize);
        /*
        (*(v->assign))(tmp, &(v->vector[pos1]));
        (*(v->assign))(&(v->vector[pos1]), &(v->vector[pos2]));
        (*(v->assign))(&(v->vector[pos2]),tmp);
        */
        free(tmp);
        return 0;
    }
}

int moveToLastVector(Vector *v, int pos) {
    return exchangePositionVector(v, pos, v->dataSize - 1);
}

int isInVector(Vector *v, void *element) {
    /*
    当element不在v中时可能出现未定义错误
    所以该函数判断未必准确
    */
    unsigned int distance = (unsigned char *)element - (unsigned char *)(v->vector);
    if (distance % v->typeSize == 0 && distance / v->typeSize >= 0 && distance / v->typeSize < v->dataSize) {
        return 1;
    } else {
        return 0;
    }
}
/*输入参数错误的结果为未定义*/
int getPostionVector(Vector *v, void *element) {
    return ((unsigned char *)element - (unsigned char *)(v->vector)) / v->typeSize;
}

int moveToLastAndDeleteVector(Vector *v, void *element) {
    int pos = getPostionVector(v, element);
    if (moveToLastVector(v, pos) < 0) {
        return -1;
    } else {
        ;
    }
    if (deleteLastElement(v) < 0) {
        return -1;
    } else {
        return 0;
    }
}
