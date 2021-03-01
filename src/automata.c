//
// Created by lenovo on 2021/2/26.
//
#include <stdlib.h>
#include "xregex.h"
#define CHARACTER_NUMBER 65535


NFA * initNFA(){
    NFA *a = malloc(sizeof(struct NFA));
#define FIRST_ALLOC_SIZE 100
    a->allocSize = FIRST_ALLOC_SIZE;
    a->usedSize = 0;
    a->nodes = malloc(sizeof(struct NFANode) * FIRST_ALLOC_SIZE);
    return a;
}

int newNFANodeIndex(struct NFA *nfa){
    if(nfa->usedSize==nfa->allocSize){
        nfa->allocSize *= 2;
        struct NFANode* n = malloc(sizeof (struct NFANode) * nfa->allocSize);
        for(NFANode * p = nfa->nodes, * p2 = n;p != nfa->nodes + nfa->usedSize;p++,p2++){
            p2->isEnd = p->isEnd;
            p2->out = p->out;
        }
    } else{
        ;
    }

    return (nfa->usedSize)++;
}

void initNFANode(NFANode *node){
    node->out = NULL;
    node->isEnd = 0;
}

void addNFAEdge(NFANode*node,int point,int character){
    NFAEdge *newEdge = malloc(sizeof(struct NFAEdge));
    newEdge->next = node->out;
    node->out = newEdge;
    newEdge->character = character;
    newEdge->point = point;
}