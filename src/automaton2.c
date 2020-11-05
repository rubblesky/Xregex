#include "automaton2.h"

#include <stdlib.h>

#include "vector.h"
#define DEFAULE_MATCH_CONTENT_SIZE 2
typedef int LongChar;

enum MatchMode {
    /*range最大值和最小值不能相同*/
    SINGLE_CHAR,        /*单个字符*/
    RANGE,              /*范围*/
    EMPTY_STRING,       /*空串*/
    ANONYMOUS_CAPTURE,  /*匿名捕获*/
    /*贪婪匹配  命名捕获    回溯引用*/
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
void MatchContentAssign(LongChar *character, LongChar *fromCharacter) {
    *character = *fromCharacter;
}

void freeMatchContentAssign(LongChar *match) {
    return;
}
/*使用LongChar数组作为参数是否能简化程序？*/
int setMatch(Match *match, enum MatchMode matchMode, LongChar *matchContent) {
    match->matchMode = matchMode;
    if (match->matchContent != NULL) {
        freeVector(match->matchContent);
    } else {
        ;
    }
    if (matchContent == NULL) {
        return 0;
    } else {
        /*是否有更好的办法*/
        int i;
        for (int i = 0; matchContent[i] != 0; i++)
            ;
        match->matchContent = INIT_VECTOR(i + 1, LongChar, MatchContentAssign, freeMatchContentAssign);
        if (match->matchContent == NULL) {
            return -1;
        } else {
            /*此处为通过LongChar数组为Vector赋值 还未完成*/
        }
    }
}

Edge *initEdge(enum MatchMode matchMode, LongChar *matchContent, int point) {
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

int addStatus(AutoMaton *am, int isEnd, int isStart) {
    Status s = {NULL, isEnd};
    int r = appendVector(am->status, &s);
    if (isStart && r >= 0) {
        am->start = getReferenceVector(am->status, r);
    }
    return r;
}
int addEdge(AutoMaton *am, int start, int end, enum MatchMode matchMode, LongChar MatchContent, int point) {
    Status *s = getReferenceVector(am->status, start);
    if (s->outEdge == NULL) {
        s->outEdge = initEdge(matchMode, MatchContent, point);
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
        e->nextEdge = initEdge(matchMode, MatchContent, point);
        if (e->nextEdge == NULL) {
            return -1;
        } else {
            return 0;
        }
    }
}
/*未完成*/
AutoMaton *NFA2DFA(AutoMaton *NFA) {
    AutoMaton *DFA = initAutoMaton(getVectorSize(NFA->status));
    Edge *e = NFA->start->outEdge;
    if (e == NULL) {
        addStatus(DFA, 1, 1);
        return DFA;
    }
    while (e->nextEdge != NULL) {
        if (e->match.matchMode)
            e = e->nextEdge;
    }
}