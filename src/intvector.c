#include "xregex.h"
#include <stdlib.h>
IntVector *initIntVector(int size) {
    IntVector *iv = malloc(sizeof(IntVector));
    iv->allocSize = size;
    iv->dataSize = 0;
    iv->vector = malloc(sizeof(int) * size);
    if (iv->vector == NULL) {
        DEAL_ERRER(-1, "malloc failed\n");
    }
    return iv;
}
void appendIntVector(IntVector *iv, int data) {
    if (iv->dataSize == iv->allocSize) {
        iv->allocSize *= 2;
        int *tmp = realloc(iv->vector, sizeof(int) * iv->allocSize);
        if (tmp == NULL) {
            DEAL_ERRER(-1, "realloc failed\n");
        } else {
            iv->vector = tmp;
        }
    } else {
        ;
    }
    iv->vector[iv->dataSize] = data;
    iv->dataSize++;
}

int getIntVectorDataSize(IntVector *iv) {
    return iv->dataSize;
}
/*从python那抄来一点语法糖*/
int getIntVectorData(IntVector *iv, int position) {
    if (position >= 0 && position < iv->dataSize) {
        return iv->vector[position];
    }else if(position < 0 && position <= iv->dataSize){
        return iv->vector[iv->dataSize + position];
    } else {
        DEAL_ERRER(-2, "vector out of size\n");
    }
}
void setIntVectorData(IntVector *iv, int position, int data) {
    if (position > 0 && position < iv->dataSize) {
        iv->vector[position] = data;
    } else if (position < 0 && position <= iv->dataSize) {
        iv->vector[iv->dataSize + position] = data;
    } else {
        DEAL_ERRER(-2, "vector out of size\n");
    }
}

IntVector *copyIntVector(IntVector *iv) {
    /*复制构造函数*/
    IntVector *ivCopy = initIntVector(iv->allocSize);
    ivCopy->dataSize = iv->dataSize;
    memcpy(ivCopy->vector, iv->vector, iv->dataSize * sizeof(int));
    return ivCopy;
}
/*只删除数据 不释放内存*/
void deleteIntVectorData(IntVector *iv) {
    iv->dataSize = 0;
}

void deleteIntVecterLastData(IntVector *iv){
    iv->dataSize--;
}

void freeIntVector(IntVector *iv) {
    if (iv->allocSize > 0) {
        free(iv->vector);
        free(iv);
    } else {
        free(iv);
    }
}