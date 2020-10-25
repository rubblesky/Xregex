#include "automaton.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULT_MATCHCONTENT_SIZE 5
#define DEFAULT_EDGE_NUM 3
#define DEFAULT_STATUS_NUM 20
//这里所有的copyVector函数都要加上错误检查

Edge *initEdge(Status *start, Status *end, Vector *matchContent) {
    Edge *e = malloc(sizeof(Edge));
    if (e == NULL) {
        return NULL;
    } else {
        e->start = start;
        e->end = end;
        e->matchContent = initVector(DEFAULT_MATCHCONTENT_SIZE, matchContent->typeSize, matchContent->assign);
        if (e->matchContent == NULL) {
            free(e);
            return NULL;
        } else {
            return e;
        }
    }
}

void freeEdge(Edge *e) {
    freeVector(e->matchContent);
    free(e);
}

void statusAssign(Edge *edge, Edge *fromEdge) {
    edge = fromEdge;
}

Status *initStatus() {
    Status *s = malloc(sizeof(Status));
    if (s == NULL) {
        return NULL;
    } else {
        s->inEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusAssign);
        s->outEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusAssign);
        if(s->inEdge == NULL || s->outEdge ==NULL){
            freeVector(s->inEdge);
            freeVector(s->outEdge);
            free(s);
            return NULL;
        }else{
            return s;
        }
    }
}

void freeStatus(Status *s){
    freeVector(s->inEdge);
    freeVector(s->outEdge);
    free(s);
}
void EdgeAssign(Edge *edge,Edge *fromEdge){
    edge->start = fromEdge->start;
    edge->end = fromEdge->end;
    edge->matchContent = copyVector(fromEdge->matchContent);
}
void StatusAssign(Status *status,Status * fromStatus){
    status->inEdge = copyVector(fromStatus->inEdge);
    status->outEdge = copyVector(fromStatus->outEdge);
}

AutoMaton *initAutoMaton(){
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if(am == NULL){
        return NULL;
    }else{
        am->edge = INIT_VECTOR(DEFAULT_EDGE_NUM * DEFAULT_STATUS_NUM, Edge, EdgeAssign);
        am->status = INIT_VECTOR(DEFAULT_STATUS_NUM, Status, StatusAssign);
        if(am->edge == NULL && am->status == NULL){
            freeVector(am->edge);
            freeVector(am->status);
            free(am);
            return NULL;
        }else{
            return am;
        }
    }
}

void freeAutoMaton(AutoMaton *am){
    freeVector(am->edge);
    freeVector(am->status);
    free(am);
}