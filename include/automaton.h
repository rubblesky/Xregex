#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <stdbool.h>

#include "vector.h"
typedef struct Status Status;
enum MatchMode {
    SEQUENCE,
    RANGE,
    REGEX,
    FUNCTION,

};
typedef struct Match{
    Vector *matchContent;
    enum MatchMode matchMode;
} Match;
typedef struct Edge {
    Status *start;
    Status * end;
    Match *match;

} Edge;

struct Status {
    /*type : Edge**/
    Vector *outEdge;
    Vector *inEdge;
    int isFinalStatus;
};

typedef struct AutoMaton {
    Vector *status;
    Vector *edge;
} AutoMaton;
typedef int LongChar;
Match *initMatch(enum MatchMode matchMode, LongChar *matchContent, int size);
void setMatch(Match *m, enum MatchMode matchMode, LongChar *matchContent, int size);
void freeMatch(Match *m);
//void setMatchMode(Match *m, enum MatchMode matchMode);
//int setMatchContent(Match *m, Vector *matchContent);
Edge *initEdge(Status *start, Status *end, Match *match);
void freeEdge(Edge *e);
Status *initStatus(int isFinalStatus);
void freeStatus(Status *s);
void setFinalStatus(Status *s, int isFinalStatus);
AutoMaton *initAutoMaton();
void freeAutoMaton(AutoMaton *am);
#define APPEND_NEW_STATUS(autoMaton) appendStatus(autoMaton, NULL)
int appendStatus(AutoMaton *am, Status *s);
int appendEdge(AutoMaton *am, Edge *e);
int appendEdgeByIndex(AutoMaton *am, int start, int end, Match *match);
int deleteEdge(AutoMaton *am, Edge *e);
int runAutoMation(AutoMaton *am, int start, int *s);
#endif
