#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <stdbool.h>

#include "vector.h"
typedef struct Status Status;
typedef struct Edge {
    Status start;
    Status end;
    Vector *matchContent;

} Edge;

struct Status {
    int statusIdentifier;
    //Vector *inEdge;
    Vector *outEdge;
};

typedef struct AutoMaton {
    Vector *status;
} AutoMaton;


#endif
