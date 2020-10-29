#include "automaton.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULT_MATCHCONTENT_SIZE 5
#define DEFAULT_EDGE_NUM 3
#define DEFAULT_STATUS_NUM 20
//这里所有的copyVector函数都要加上错误检查

void LongCharAssign(LongChar * l1,LongChar *l2){
    *l1 = *l2;
}
static void nothingToFree(void *x){
    return;
}
Match *initMatch(enum MatchMode matchMode ,LongChar * matchContent,int size) {
    /*m->matchContent未赋值*/
    Match *m = malloc(sizeof(Match));
    if(m == NULL){
        return NULL;
    }else{
        m->matchMode = matchMode;
        m->matchContent = INIT_VECTOR(size + 1, LongChar,LongCharAssign, nothingToFree);
        for (int i = 0; i < size;i++){
            appendVector(m->matchContent, matchContent + i);
        }
        return m;
    }
}

void setMatch(Match *m, enum MatchMode matchMode, LongChar *matchContent, int size){
    clearVecter(m->matchContent);
    for (int i = 0; i < size;i++){
        appendVector(m->matchContent, matchContent + i);
    }
    m->matchMode = matchMode;
}

void freeMatch(Match *m) {
    freeVector(m->matchContent);
    free(m);
}

void setMatchMode(Match *m,enum MatchMode matchMode){
    m->matchMode = matchMode;
}

int setMatchContent(Match *m,Vector *matchContent){
    freeVector(m->matchContent);
    m->matchContent = copyVector(matchContent);
    if(m->matchContent == NULL){
        return -1;
    }else{
        return 0;
    }
}

Edge *initEdge(Status *start, Status *end, Match *match) {
    Edge *e = malloc(sizeof(Edge));
    if (e == NULL) {
        return NULL;
    } else {
        e->start = start;
        e->end = end;
        e->match->matchMode = match->matchMode;
        e->match->matchContent = copyVector(match->matchContent);
        //e->match->matchContent = initVector(DEFAULT_MATCHCONTENT_SIZE, match->matchContent->typeSize, match->matchContent->assign);
        if (e->match->matchContent == NULL) {
            free(e);
            return NULL;
        }
        else {
            return e;
        }
    }
}

void freeEdge(Edge *e) {
    freeVector(e->match->matchContent);
    free(e);
}

void statusPointerAssign(Edge *edge, Edge *fromEdge) {
    edge = fromEdge;
}

Status *initStatus(int isFinalStatus) {
    Status *s = malloc(sizeof(Status));
    s->isFinalStatus = isFinalStatus;
    if (s == NULL) {
        return NULL;
    } else {
        s->inEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign,nothingToFree);
        s->outEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign,nothingToFree);
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
void setFinalStatus(Status *s, int isFinalStatus){
    s->isFinalStatus = isFinalStatus;
}

static void EdgeAssign(Edge *edge, Edge *fromEdge) {
    edge->start = fromEdge->start;
    edge->end = fromEdge->end;
    edge->match->matchMode = fromEdge->match->matchMode;
    edge->match->matchContent = copyVector(fromEdge->match->matchContent);
}
static void freeEdgeAssign(Edge * e){
    freeVector(e->match->matchContent);
    return;
}

static void StatusAssign(Status *status, Status *fromStatus) {
    status->inEdge = copyVector(fromStatus->inEdge);
    status->outEdge = copyVector(fromStatus->outEdge);
    status->isFinalStatus = fromStatus->isFinalStatus;
}
static void freeStatusAssign(Status *s){
    freeVector(s->inEdge);
    freeVector(s->outEdge);
    return;
}

AutoMaton *initAutoMaton() {
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if (am == NULL) {
        return NULL;
    } else {
        am->edge = INIT_VECTOR(DEFAULT_EDGE_NUM * DEFAULT_STATUS_NUM, Edge, EdgeAssign,freeEdgeAssign);
        am->status = INIT_VECTOR(DEFAULT_STATUS_NUM, Status, StatusAssign,freeStatusAssign);
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
    Status *start = getReferenceVector(am->status, getPostionVector(am->status,e->start));
    appendVector(start->outEdge, e);
    Status *end = getReferenceVector(am->status,getPostionVector(am->status,e->end)
    /* e->end - (Status *)(getVectorHeader(am->status))*/);
    appendVector(end->inEdge, e);
    return appendVector(am->edge, e);
}

int appendEdgeByIndex(AutoMaton *am,int start,int end,Match *match){
    Status *startStatus = getReferenceVector(am->status, start);
    Status *endStatus = getReferenceVector(am->status, end);
    Edge *e = initEdge(startStatus, endStatus, match);
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

