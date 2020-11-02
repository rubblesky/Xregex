#include "automaton2.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULE_MATCH_CONTENT_SIZE 10
typedef int LongChar;
enum MatchMode {
    /*这里要改 不允许出现sequence 一律拆分成多条边
    range最大值和最小值不能相同*/
    SEQUENCE,
    RANGE,
    REGEX,
    FUNCTION,
};

typedef struct Match Match;
struct Match {
    enum MatchMode matchMode;
    Vector *matchContent;
};

typedef struct Edge Edge;
struct Edge {
    Edge *nextEdge;
    Match match;
    int point;
};

typedef struct Status {
    Edge *outEdge;
    int isEnd;
} Status;

typedef struct AutoMaton {
    Vector *status;
    Status *start;

} AutoMaton;

void StatusAssign(Status *status, Status *fromStatus) {
    status->outEdge = fromStatus->outEdge;
    status->isEnd = fromStatus->isEnd;
}
void freeStatusAssign(Status *s) {
    return;
}

AutoMaton *initAutoMaton(int statusNum) {
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if (am == NULL) {
        return NULL;
    } else {
        am->status = INIT_VECTOR(statusNum, Status, StatusAssign, freeStatusAssign);
        am->start = NULL;
        if (am->status == NULL) {
            free(am);
            return NULL;
        } else {
            return am;
        }
    }
}

AutoMaton *freeAutoMaton(AutoMaton *am) {
    int statusNum = getVectorSize(am->status);
    Status s;
    Edge *e = NULL;
    for (int i = 0; i < statusNum; i++) {
        getValueVector(am->status, i, &s);
        e = s.outEdge;
        while (e->nextEdge != NULL) {
            Edge *prevEdge = e;
            e = e->nextEdge;
            free(prevEdge);
        }
    }
    freeVector(am->status);
    free(am);
}
/*这里也不对呀  应该是match才对*/
void EdgeAssign(Edge *edge, Edge *fromEdge) {
    edge->nextEdge = fromEdge->nextEdge;
    edge->match.matchContent = copyVector(fromEdge->match.matchContent);
    edge->match.matchMode = fromEdge->match.matchMode;
    edge->point = fromEdge->point;
}

void freeEdgeAssign(Edge *e) {
    freeVector(e->match.matchContent);
}

int setMatch(Match *match, enum MatchMode matchMode, LongChar *matchContent) {
    match->matchMode = matchMode;
    if (match->matchContent != NULL) {
        freeVector(match->matchContent);
    } else {
        ;
    }
    /*这里也要改*/
    match->matchContent = INIT_VECTOR(DEFAULE_MATCH_CONTENT_SIZE, LongChar, EdgeAssign, freeEdgeAssign);
    return match->matchContent == NULL ? -1 : 0;
}

Edge *initEdge(enum MatchMode matchMode, LongChar *matchContent,int point) {
    Edge *e = malloc(sizeof(Edge));
    if (e == NULL) {
        return NULL;
    } else {
        e->nextEdge = NULL;
        if (setMatch(&e->match, matchMode, matchContent) < 0) {
            free(e);
            return NULL;
        } else {
            return e;
        }
    }
}

void freeEdge(Edge *e) {
    freeVector(e->match.matchContent);
    free(e);
}

int addStatus(AutoMaton *am) {
    Status s = {NULL};
    return appendVector(am->status, &s);
}
int addEdge(AutoMaton *am, int start, int end, enum MatchMode matchMode, LongChar MatchContent,int point) {
    Status *s = getReferenceVector(am->status, start);
    if (s->outEdge == NULL) {
        s->outEdge = initEdge(matchMode, MatchContent,point);
        if (s->outEdge == NULL) {
            return -1;
        } else {
            return 0;
        }

    } else {
        Edge *e = s->outEdge;
        while (e->nextEdge != NULL) {
            e = e->nextEdge;
        }
        e->nextEdge = initEdge(matchMode, MatchContent,point);
        if (e->nextEdge == NULL) {
            return -1;
        } else {
            return 0;
        }
    }
}

int removeEdge() {
}