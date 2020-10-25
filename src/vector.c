#include <stdlib.h>
#include <string.h>
#include "vector.h"
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

void freeVector(Vector *v){
    if (v == NULL) return;
    else if (v->allocSize != 0) {
        free(v->vector);
    }
    free(v);
}

int appendVector(Vector *v ,void *element){ 
    if(v->dataSize == v->allocSize){
        Vector *tmp = realloc(v->vector, v->allocSize * 2 * v->typeSize);
        if(tmp == NULL){
            return -1;
        }else{
            v->allocSize *= 2;
        }
    }

    (*v->assign)(&(v->vector[v->dataSize]), element);
    v->dataSize++;
    return v->dataSize - 1;
}

int setValueVector(Vector *v, int pos,void * value){
    if(pos > 0 && pos < v->dataSize){
        (*v->assign)(&(v->vector[pos]), value);
        return 0;
    } else {
        return -1;
    }
}

int getValueVector(Vector *v, int pos,void * value){
    if (pos > 0 && pos < v->dataSize) {
        (*v->assign)(&(v->vector[pos]), value);
        return 0;
    } else {
        return -1;
    }
}

Vector * copyVector(Vector *v){
    Vector *tmp = initVector(v->allocSize, v->typeSize, v->assign);
    if (tmp == NULL) {
        return NULL;
    } else {
        tmp->dataSize = v->dataSize;
        memcpy(tmp->vector, v->vector, v->typeSize * v->dataSize);
        return tmp;
    }
}
