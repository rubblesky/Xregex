#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "CuTest.h"
#include "allTest.h"

#include "xregex.h"
void showAST(ASTNode *node);
void  showNFA(NFA * automata);
void testRegex(CuTest *tc) {
    char *re = "a|b";
    struct ASTNode* ASTRoot = parse(re);
    showAST(ASTRoot);
    NFA * NFA1 = getNFA(ASTRoot);
    showNFA(NFA1);
}

CuSuite *
CuGetRegexSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testRegex);

    return suite;
}


void showAST(ASTNode *node){
    if(node == NULL){return;}
    else {
        showAST(node->left);
        showAST(node->right);
        printf("%d  ", node->symbol);
    }
}

void  showNFA(NFA * automata){
    printf("\n");
    for(int i = 0;i < automata->usedSize;i++){
        printf("%d : ",i);
        for(NFAEdge *p = automata->nodes[i].out;p != NULL;p = p->next){
            printf("%d %c\t",p->point,p->character > 0?p->character:'e');
        }
        printf("\n");
    }
    printf("\n");
}

/*
char* printSymbol(struct RegexTreeNode *rtn) {
    if(rtn == NULL){
        return NULL;
    }
    char *s;
    switch (rtn->symbol) {
        case START:
            s = "start";
            break;
        case N_S:
            s = "S";
            break;
        case N_A:
            s = "A";
            break;
        case N_B:
            s = "B";
            break;
        case N_C:
            s = "C";
            break;
        case N_D:
            s = "D";
            break;
        case N_E:
            s = "E";
            break;
        case N_F:
            s = "F";
            break;
        case T:
            s = "T";
            break;

    }
    return s;
}

void showRegexTree(RegexTreeNode *rtn) {
    RegexTreeNode *queue[1000];

    int i = 0, j = 0,end;
    queue[i++] = rtn;
    end = i;
    for(;j<i;j++){
        RegexTreeNode *p = queue[j];
        if(j == end){
            printf("\n%s:%s\t\t  ",printSymbol(queue[j]),printSymbol(queue[j]->parent));
            end = i;
        }else{
            printf("%s:%s\t\t  ",printSymbol(queue[j]),printSymbol(queue[j]->parent));
        }
        if(queue[j]->firstChild != NULL){
            queue[i++] = queue[j]->firstChild;
            p = queue[j]->firstChild;
            while (p->nextSibling != NULL){
                queue[i++] = p->nextSibling;
                p = p->nextSibling;
            }
        }

    }
 }
*/
