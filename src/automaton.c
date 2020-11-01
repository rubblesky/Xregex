#include "automaton.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULT_MATCHCONTENT_SIZE 5
#define DEFAULT_EDGE_NUM 3
#define DEFAULT_STATUS_NUM 20
//这里所有的copyVector函数都要加上错误检查

void LongCharAssign(LongChar *l1, LongChar *l2) {
    *l1 = *l2;
}
static void nothingToFree(void *x) {
    return;
}
Match *initMatch(enum MatchMode matchMode, LongChar *matchContent, int size) {
    Match *m = malloc(sizeof(Match));
    if (m == NULL) {
        return NULL;
    } else {
        m->matchMode = matchMode;
        m->matchContent = INIT_VECTOR(size + 1, LongChar, LongCharAssign, nothingToFree);
        for (int i = 0; i < size; i++) {
            appendVector(m->matchContent, matchContent + i);
        }
        return m;
    }
}
static Match *copyMatch(Match *m) {
    return initMatch(m->matchMode, m->matchContent->vector, m->matchContent->dataSize);
}

void setMatch(Match *m, enum MatchMode matchMode, LongChar *matchContent, int size) {
    clearVecter(m->matchContent);
    for (int i = 0; i < size; i++) {
        appendVector(m->matchContent, matchContent + i);
    }
    m->matchMode = matchMode;
}

void freeMatch(Match *m) {
    freeVector(m->matchContent);
    free(m);
}

void setMatchMode(Match *m, enum MatchMode matchMode) {
    m->matchMode = matchMode;
}

int setMatchContent(Match *m, Vector *matchContent) {
    freeVector(m->matchContent);
    m->matchContent = copyVector(matchContent);
    if (m->matchContent == NULL) {
        return -1;
    } else {
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
        e->match = copyMatch(match);
        //e->match->matchMode = match->matchMode;
        //e->match->matchContent = copyVector(match->matchContent);
        //e->match->matchContent = initVector(DEFAULT_MATCHCONTENT_SIZE, match->matchContent->typeSize, match->matchContent->assign);
        if (e->match->matchContent == NULL) {
            free(e);
            return NULL;
        } else {
            return e;
        }
    }
}

void freeEdge(Edge *e) {
    freeMatch(e->match);
    free(e);
}

void statusPointerAssign(Edge **edge, Edge **fromEdge) {
    *edge = *fromEdge;
}

Status *initStatus(int isFinalStatus) {
    Status *s = malloc(sizeof(Status));
    s->isFinalStatus = isFinalStatus;
    if (s == NULL) {
        return NULL;
    } else {
        s->inEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign, nothingToFree);
        s->outEdge = INIT_VECTOR(DEFAULT_EDGE_NUM, Edge *, statusPointerAssign, nothingToFree);
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
void setFinalStatus(Status *s, int isFinalStatus) {
    s->isFinalStatus = isFinalStatus;
}

static void EdgeAssign(Edge *edge, Edge *fromEdge) {
    edge->start = fromEdge->start;
    edge->end = fromEdge->end;
    edge->match = copyMatch(fromEdge->match);
}
static void freeEdgeAssign(Edge *e) {
    freeMatch(e->match);
    return;
}

static void StatusAssign(Status *status, Status *fromStatus) {
    status->inEdge = copyVector(fromStatus->inEdge);
    status->outEdge = copyVector(fromStatus->outEdge);
    status->isFinalStatus = fromStatus->isFinalStatus;
}
static void freeStatusAssign(Status *s) {
    freeVector(s->inEdge);
    freeVector(s->outEdge);
    return;
}

AutoMaton *initAutoMaton() {
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if (am == NULL) {
        return NULL;
    } else {
        am->edge = INIT_VECTOR(DEFAULT_EDGE_NUM * DEFAULT_STATUS_NUM, Edge, EdgeAssign, freeEdgeAssign);
        am->status = INIT_VECTOR(DEFAULT_STATUS_NUM, Status, StatusAssign, freeStatusAssign);
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
    Status *start = getReferenceVector(am->status, getPostionVector(am->status, e->start));
    appendVector(start->outEdge, e);
    Status *end = getReferenceVector(am->status, getPostionVector(am->status, e->end)
                                     /* e->end - (Status *)(getVectorHeader(am->status))*/);
    appendVector(end->inEdge, e);
    return appendVector(am->edge, e);
}

int appendEdgeByIndex(AutoMaton *am, int start, int end, Match *match) {
    Status *startStatus = getReferenceVector(am->status, start);
    Status *endStatus = getReferenceVector(am->status, end);
    Edge *e = initEdge(startStatus, endStatus, match);

    int r = appendVector(am->edge, e);
    Edge *tmp = getReferenceVector(am->edge, r);
    appendVector(startStatus->outEdge, &tmp);
    appendVector(endStatus->inEdge, &tmp);
    freeEdge(e);
    return r;
}
/*建议不用进行异常处理*/

int deleteEdge(AutoMaton *am, Edge *e) {
    if (!isInVector(am->edge, e)) {
        return -1;
    } else {
        /*边的start和end相同时可能会返回异常值 不必理会*/
        moveToLastAndDeleteVector(e->end->inEdge, e);
        moveToLastAndDeleteVector(e->start->outEdge, e);
        moveToLastAndDeleteVector(am->edge, e);
        freeEdge(e);
        return 0;
    }
}
static int isMatch(Match *m, int c) {
    switch (m->matchMode) {
        case SEQUENCE: {
            int mc;
            for (int i = 0; getValueVector(m->matchContent, i, &mc) != -1 && mc != c; i++) {
                ;
            }
            if (mc == c) {
                return 1;
            } else {
                return 0;
            }
        } break;
        case RANGE: {
            int start, end;
            getValueVector(m->matchContent, 0, &start);
            getValueVector(m->matchContent, 1, &end);
            for (int i = start; i <= end; i++) {
                if (c == i) {
                    return 1;
                }
            }
            return 0;
        } break;
        case FUNCTION:

        default:
            return -1;
            break;
    }
}

int runAutoMation(AutoMaton *am, int start, int *s) {
    Status *nowStatus = getReferenceVector(am->status, start);
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        int j = 0;
        Edge *e = NULL;
        while (getValueVector(nowStatus->outEdge, j, &e) == 0 && !isMatch(e->match, s[i])) {
            j++;
        }
        if (j == getVectorSize(nowStatus->outEdge)) {
            return -1;
        } else {
            nowStatus = e->end;
        }
    }
    if (nowStatus->isFinalStatus) {
        return i;
    } else {
        return -1;
    }
}
