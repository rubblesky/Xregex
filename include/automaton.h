#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <stdbool.h>

#include "vector.h"
typedef struct Status Status;
enum matchMode {
    SEQUENCE,
    RANGE,
    REGEX,
    
};
typedef struct Edge {
    Status *start;
    Status * end;
    Vector * matchContent;
    enum matchMode matchMode;

} Edge;

struct Status {
    Vector **outEdge;
    Vector **inEdge;
};

typedef struct AutoMaton {
    Vector *status;
    Vector *edge;
} AutoMaton;

Edge *initEdge(Status *start, Status *end, Vector *matchContent, enum matchMode matchMode);
void freeEdge(Edge *e);
Status *initStatus();
void freeStatus(Status *s);
AutoMaton *initAutoMaton();
void freeAutoMaton(AutoMaton *am);
#define APPEND_NEW_STATUS(autoMaton) appendStatus(autoMaton, NULL)
int appendStatus(AutoMaton *am, Status *s);
int appendEdge(AutoMaton *am, Edge *e);
int appendEdgeByIndex(AutoMaton *am, int start, int end, Vector *matchContent, enum matchMode matchMode);
int deleteEdge(AutoMaton *am, Edge *e);
#endif
