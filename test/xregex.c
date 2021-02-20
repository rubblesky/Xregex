#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "CuTest.h"
#include "allTest.h"

#include "xregex.h"

void showRegexTree(RegexTreeNode *rtn);

void testRegex(CuTest *tc) {
    char *re = "a((bc)*|d*)e";
    IntVector *iv = transExpress(re);
    IntVector *iv2 = getEscapeCharacterExpress(iv);
    freeIntVector(iv);
    LexicalResult *lr = lexicalAnalyse(iv2);
    freeIntVector(iv2);
    RegexTreeNode * root = getRegexTree(lr);
    root = eliminateS(root);
    root = eliminateB(root);
    moveD(root);
    //moveC(root);
    showRegexTree(root);

}

CuSuite *
CuGetRegexSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testRegex);

    return suite;
}

char* printSymbol(struct RegexTreeNode *rtn) {
    char *s;
    switch (rtn->symbol) {
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
        case T:
            s = "T";
            break;
        case END:
            s = "#";
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
            printf("\n%s  ",printSymbol(queue[j]));
            end = i;
        }else{
            printf("%s  ",printSymbol(queue[j]));
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