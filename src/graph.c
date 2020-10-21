#include <stdlib.h>

struct GraphEdge;
typedef struct GraphNode {
    void *data;
    GraphEdge **edges;
    int edgeSize;
    int allocSize;
} GraphNode;

typedef struct GraphEdge{
    GraphNode *pointedNode;
    void *data;
} GraphEdge;

typedef void (*assignFunction)(void *assignedValue, void *fromValue);

typedef struct Graph {
    GraphNode **node;
    int allocSize;
    int nodeSize;
    int typeSize;
    assignFunction nodeAssign;
    assignFunction edgeAssign;
} Graph;

Graph *initGraph(int size,int typeSize,assignFunction nodeAssign,assignFunction edgeAssign) {
    Graph *gp = malloc(sizeof(Graph));
    if (gp == NULL) {
        return NULL;
    } else {
        gp->nodeAssign = nodeAssign;
        gp->nodeAssign = edgeAssign;
        gp->typeSize = typeSize;
        gp->node = malloc(size * sizeof(GraphNode));
        if (gp->node == NULL) {
            gp->allocSize = 0;
            gp->nodeSize = 0;
            return NULL;
        }else{
            gp->allocSize = size;
            gp->nodeSize = 0;
            return gp;
        }
    }
}

void freeGraph(Graph *g){
    if(g->allocSize == 0){
        free(g);
    }else{
        for (int i = 0; i < g->nodeSize;i++){
            if(g->node[i]->allocSize != NULL){
                for (int j = 0; j < g->node[i]->edgeSize;j++){
                    free(g->node[i]->edges[j]);
                }
                free(g->node[i]->edges);
            }
        }
        free(g->node);
        free(g);
    }
}


int appendGraphNode(Graph *g,void * value){
    if(g->nodeSize == g->allocSize){
        g->allocSize *= 2;
        GraphNode *tmp = realloc(g->node,g->nodeSize * g->allocSize);
        if(tmp != NULL){
            g->node = tmp;
        }else{
            return -1;
        }
    }else{
        g->nodeAssign(g->node[g->nodeSize]->data, value);
        g->node[g->nodeSize]->edges = NULL;
        g->node[g->nodeSize]->allocSize = 0;
        g->node[g->nodeSize]->edgeSize = 0;
        g->nodeSize++;
        return g->nodeSize - 1;
    }
}

int appendGraphEdge(Graph *g,int NodeID1,int NodeID2){
    
}
