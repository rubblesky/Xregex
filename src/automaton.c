#include "vector.h"
#include "automaton.h"

#include <stdlib.h>
void statusAssign(Status *assignedStatus, Status *fromStatus) {
    //error
    assignedStatus->outEdge = fromStatus->outEdge;
    assignedStatus->statusIdentifier = assignedStatus->statusIdentifier;
}

AutoMaton *initAutoMaton(int statusNum) {
    AutoMaton *am = malloc(sizeof(AutoMaton));
    if(am == NULL){
        return NULL;
    }else{
        am->status = INIT_VECTOR(statusNum, Status, statusAssign);
        if(am->status == NULL){
            return -1;
        }else{
            return 0;
        }
    }
}