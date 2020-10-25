#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <stdbool.h>

#include "vector.h"
typedef struct Status Status;
typedef struct Edge {
    Status *start;
    Status *end;
    Vector *matchContent;
    
} Edge;

struct Status {
    Vector **outEdge;
    Vector **inEdge;
};

typedef struct AutoMaton {
    Vector *status;
    Vector *edge;
} AutoMaton;

#endif
