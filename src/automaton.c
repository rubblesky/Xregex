#include "automaton.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULT_MATCHCONTENT_SIZE 5
#define DEFAULT_EDGE_NUM 3
#define DEFAULT_STATUS_NUM 20
//这里所有的copyVector函数都要加上错误检查

Edge *initEdge(Status *start, Status *end, Vector *matchContent, enum matchMode matchMode) {
    Edge *e = malloc(sizeof(Edge));
    if (e == NULL) {
        return NULL;
    } else {
        e->start = start;
        e->end = end;
        e->matchMode = matchMode;
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

void statusPointerAssign(Edge *edge, Edge *fromEdge) {
    edge = fromEdge;
}

Status *initStatus() {
    Status *s = malloc(sizeof(Status));
    if (s == NULL) {
        return NULL;
    } else {
        s->inEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign);
        s->outEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign);
        if (s->inEdge == NULL || s->outEdge == NULL) {
            freeVector(s->inEdge);
            freeVector(s->outEdge);
            free(s);
            return NULL;
        } else {
            return s;
        }
    }
}

void freeStatus(Status *s) {
    freeVector(s->inEdge);
    freeVector(s->outEdge);
    free(s);
}
void EdgeAssign(Edge *edge, Edge *fromEdge) {
    edge->start = fromEdge->start;
    edge->end = fromEdge->end;
    edge->matchContent = copyVector(fromEdge->matchContent);
}
void StatusAssign(Status *status, Status *fromStatus) {
    if (fromStatus != NULL) {
        status->inEdge = copyVector(fromStatus->inEdge);
        status->outEdge = copyVector(fromStatus->outEdge);
    }else{
        status = initStatus();
    }
}

AutoMaton *initAutoMaton() {
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if (am == NULL) {
        return NULL;
    } else {
        am->edge = INIT_VECTOR(DEFAULT_EDGE_NUM * DEFAULT_STATUS_NUM, Edge, EdgeAssign);
        am->status = INIT_VECTOR(DEFAULT_STATUS_NUM, Status, StatusAssign);
        if (am->edge == NULL && am->status == NULL) {
            freeVector(am->edge);
            freeVector(am->status);
            free(am);
            return NULL;
        } else {
            return am;
        }
    }
}

void freeAutoMaton(AutoMaton *am) {
    freeVector(am->edge);
    freeVector(am->status);
    free(am);
}


int appendStatus(AutoMaton *am, Status *s) {
    return appendVector(am->status, s);
}

int appendEdge(AutoMaton *am, Edge *e) {
    Status *start = getReferenceVector(am->status, e->start - (Status *)(getVector(am->status)));
    appendVector(start->outEdge, e);
    Status *end = getReferenceVector(am->status, e->end - (Status *)(getVector(am->status)));
    appendVector(end->inEdge, e);
    return appendVector(am->edge, e);
}

int appendEdgeByIndex(AutoMaton *am,int start,int end,Vector *matchContent,enum matchMode matchMode){
    Status *startStatus = getReferenceVector(am->status, start);
    Status *endStatus = getReferenceVector(am->status, end);
    Edge *e = initEdge(startStatus, endStatus, matchContent, matchMode);
    appendVector(startStatus->outEdge,e);
    appendVector(endStatus->inEdge, e);
    int r = appendVector(am->edge, e);
    freeEdge(e);
    return r;
}
/*建议不用进行异常处理*/

int deleteEdge(AutoMaton *am,Edge *e){
    if(!isInVector(am->edge,e)){
        return -1;
    }else{
        /*边的start和end相同时可能会返回异常值 不必理会*/
        moveToLastAndDeleteVector(e->end->inEdge, e);
        moveToLastAndDeleteVector(e->start->outEdge, e);
        moveToLastAndDeleteVector(am->edge, e);
        freeEdge(e);
        return 0;
    }
}

