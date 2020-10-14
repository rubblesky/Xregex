#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "xregex.h"

void testUtf8ToInt(CuTest *tc) {
    char *s;
    s = "😂";
    int result, size;
    size = utf8ToInt(s, &result);
    CuAssertIntEquals(tc, 4, size);
    CuAssertIntEquals(tc, 0x1f602, result);

    s = "怠";
    size = utf8ToInt(s, &result);
    CuAssertIntEquals(tc, 3, size);
    CuAssertIntEquals(tc, 0x6020, result);

    s = "ˆ";
    size = utf8ToInt(s, &result);
    CuAssertIntEquals(tc, 2, size);
    CuAssertIntEquals(tc, 0x02c6, result);

    s = "A";
    size = utf8ToInt(s, &result);
    CuAssertIntEquals(tc, 1, size);
    CuAssertIntEquals(tc, 'A', result);


    
    int i[5] = {-1,0};
    size = utf8ToInt((unsigned char *)&i, &result);
    CuAssertIntEquals(tc, 0, size);

}

void testIntToUtf8(CuTest *tc) {
    int number, size;
    char s[5];

    number = 'A';
    size = intToUtf8(number, s);
    CuAssertStrEquals(tc, "A", s);
    CuAssertIntEquals(tc, 1, size);

    number = 0x02c6;
    size = intToUtf8(number, s);
    CuAssertStrEquals(tc, "ˆ", s);
    CuAssertIntEquals(tc, 2, size);

    number = 0x6020;
    size = intToUtf8(number, s);
    CuAssertStrEquals(tc, "怠", s);
    CuAssertIntEquals(tc, 3, size);

    number = 0x1f602;
    size = intToUtf8(number, s);
    CuAssertStrEquals(tc, "😂", s);
    CuAssertIntEquals(tc, 4, size);

    number = -1;
    size = intToUtf8(number, s);
    CuAssertIntEquals(tc, 0, size);
}

void testInitXregexTree(CuTest *tc){
    XregexTree *xt = initXregexTree();
    CuAssertTrue(tc, xt->root->childSize == 0);
    CuAssertTrue(tc, xt->root->childNum == 0);
    CuAssertTrue(tc, xt->root->child == NULL);
    CuAssertTrue(tc, xt->root->content == NULL);
    freeXregexTree(xt);
}

void testNewXregexNode(CuTest *tc){
    XregexNode *xn = newXregexNode();
    CuAssertTrue(tc, xn->childSize == 0);
    CuAssertTrue(tc, xn->childNum == 0);
    deleteXregexNode(xn,FORCE);
}

void testAddChildXregexNode(CuTest *tc){
    XregexNode *xn1 = newXregexNode();
    XregexNode *xn2 = newXregexNode();
    addChildXregexNode(xn1, xn2);
    CuAssertTrue(tc, xn1->childNum == 1);
    CuAssertPtrEquals(tc, xn2, xn1->child[0]);
    deleteXregexNode(xn1, RECURSIVE);
}

void testDeleteXregexNode(CuTest *tc){
    XregexNode *xn1 = newXregexNode();
    int result = deleteXregexNode(xn1, FORCE);
    CuAssertIntEquals(tc, 0, result);

    xn1 = newXregexNode();
    XregexNode *xn2 = newXregexNode();
    addChildXregexNode(xn1, xn2);
    result = deleteXregexNode(xn1, RESTRICT);
    CuAssertPtrNotNull(tc,xn1);
    CuAssertIntEquals(tc, -1, result);

    result = deleteXregexNode(xn1, RECURSIVE);
    CuAssertIntEquals(tc, 0, result);

}

CuSuite *CuGetSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testUtf8ToInt);
    SUITE_ADD_TEST(suite, testIntToUtf8);
    SUITE_ADD_TEST(suite, testInitXregexTree);
    SUITE_ADD_TEST(suite, testNewXregexNode);
    SUITE_ADD_TEST(suite, testAddChildXregexNode);
    SUITE_ADD_TEST(suite, testDeleteXregexNode);

    return suite;
}
