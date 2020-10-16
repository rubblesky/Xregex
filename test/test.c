#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "lexical.c"
#include "stack.h"
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

    int i[5] = {-1, 0};
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

void testInitXregexTree(CuTest *tc) {
    XregexTree *xt = initXregexTree();
    CuAssertTrue(tc, xt->root->childSize == 0);
    CuAssertTrue(tc, xt->root->childNum == 0);
    CuAssertTrue(tc, xt->root->child == NULL);
    CuAssertTrue(tc, xt->root->content == NULL);
    freeXregexTree(xt);
}

void testNewXregexNode(CuTest *tc) {
    XregexNode *xn = newXregexNode();
    CuAssertTrue(tc, xn->childSize == 0);
    CuAssertTrue(tc, xn->childNum == 0);
    deleteXregexNode(xn, FORCE);
}

void testAddChildXregexNode(CuTest *tc) {
    XregexNode *xn1 = newXregexNode();
    XregexNode *xn2 = newXregexNode();
    addChildXregexNode(xn1, xn2);
    CuAssertTrue(tc, xn1->childNum == 1);
    CuAssertPtrEquals(tc, xn2, xn1->child[0]);
    deleteXregexNode(xn1, RECURSIVE);
}

void testDeleteXregexNode(CuTest *tc) {
    XregexNode *xn1 = newXregexNode();
    int result = deleteXregexNode(xn1, FORCE);
    CuAssertIntEquals(tc, 0, result);

    xn1 = newXregexNode();
    XregexNode *xn2 = newXregexNode();
    addChildXregexNode(xn1, xn2);
    result = deleteXregexNode(xn1, RESTRICT);
    CuAssertPtrNotNull(tc, xn1);
    CuAssertIntEquals(tc, -1, result);

    result = deleteXregexNode(xn1, RECURSIVE);
    CuAssertIntEquals(tc, 0, result);
}

void assignInt(void *a, void *b) {
    *(int *)a = *(int *)b;
}
void testInitStack(CuTest *tc) {
    Stack *stack = NULL;
    stack = initStack(sizeof(int), 0, assignInt);
    CuAssertPtrNotNull(tc, stack);
    CuAssertIntEquals(tc, 0, stack->arrayUsedSize);
    CuAssertIntEquals(tc, DEFAULT_STACK_SIZE, stack->arrayAllocSize);
    CuAssertIntEquals(tc, sizeof(int), stack->valueTypeSize);
    freeStack(stack);
}
void testPushStack(CuTest *tc) {
    Stack *stack = NULL;
    stack = initStack(sizeof(int), 3, assignInt);
    int c = 5;
    stackPush(stack, &c);
    c = 6;
    stackPush(stack, &c);
    stackPush(stack, &c);
    CuAssertIntEquals(tc, 3, stack->arrayAllocSize);
    stackPush(stack, &c);
    CuAssertIntEquals(tc, 6, stack->arrayAllocSize);
    CuAssertIntEquals(tc, 4, stack->arrayUsedSize);
    freeStack(stack);
}
void testPopStack(CuTest *tc) {
    Stack *stack = NULL;
    stack = initStack(sizeof(int), 3, assignInt);
    int c = 5;
    stackPush(stack, &c);
    c = 6;
    stackPush(stack, &c);
    c = 7;
    stackPush(stack, &c);
    c = 8;
    stackPush(stack, &c);

    c = stackPop(stack);
    CuAssertIntEquals(tc, 0, c);
    c = stackPop(stack);
    c = stackPop(stack);
    c = stackPop(stack);
    CuAssertIntEquals(tc, 0, c);
    c = stackPop(stack);
    CuAssertIntEquals(tc, -1, c);

    freeStack(stack);
}

void testGetTopStack(CuTest *tc) {
    Stack *stack = NULL;
    stack = initStack(sizeof(int), 3, assignInt);
    int c = 5;
    stackPush(stack, &c);
    c = 6;
    stackPush(stack, &c);
    c = 7;
    stackPush(stack, &c);
    c = 8;
    stackPush(stack, &c);

    int *p;
    p = stackGetTop(stack);
    CuAssertIntEquals(tc, *p, 8);
    stackPop(stack);
    p = stackGetTop(stack);
    CuAssertIntEquals(tc, *p, 7);
    stackPop(stack);
    p = stackGetTop(stack);
    CuAssertIntEquals(tc, *p, 6);
    stackPop(stack);
    p = stackGetTop(stack);
    CuAssertIntEquals(tc, *p, 5);
    stackPop(stack);

    freeStack(stack);
}

void testInitIntVector(CuTest *tc){
    IntVector *iv = NULL;
    iv = initIntVector(5);
    CuAssertIntEquals(tc, iv->allocSize, 5);
    CuAssertPtrNotNull(tc, iv);
    CuAssertIntEquals(tc, iv->dataSize, 0);
    freeIntVector(iv);
}
void testAppendIntVector(CuTest *tc) {
    IntVector* iv = initIntVector(2);
    appendIntVector(iv, 1);
    CuAssertIntEquals(tc, iv->vector[0], 1);
    CuAssertIntEquals(tc, iv->dataSize, 1);
    appendIntVector(iv, 2);
    CuAssertIntEquals(tc, iv->vector[1], 2);
    appendIntVector(iv, 3);
    CuAssertIntEquals(tc, iv->vector[2], 3);
    CuAssertIntEquals(tc, iv->allocSize, 4);
    CuAssertIntEquals(tc, iv->dataSize, 3);
    freeIntVector(iv);
}
void testDeleteIntVector(CuTest *tc) {
    IntVector *iv = initIntVector(2);
    appendIntVector(iv, 1);
    appendIntVector(iv, 2);
    CuAssertIntEquals(tc, iv->dataSize, 2);
    deleteIntVectorData(iv);
    CuAssertIntEquals(tc, iv->dataSize, 0);
    freeIntVector(iv);
}
void testGetIntVectorData(CuTest *tc) {
    IntVector *iv = initIntVector(2);
    appendIntVector(iv, 1);
    appendIntVector(iv, 2);
    int i = getIntVectorData(iv, 0);
    CuAssertIntEquals(tc, i, 1);
    i = getIntVectorData(iv, 1);
    CuAssertIntEquals(tc, i, 2);
    freeIntVector(iv);
}

void testDealDefiniteRepeat(CuTest *tc) {
    char re[] = "{1,2}";
    IntVector *iv = getIntArrayFromUtf8(re);
    SymbolTable *st = initSymbolTable(5);
    dealDefiniteRepeat(iv, 0, st);
    CuAssertIntEquals(tc, 1, st[0].symbol->value->vector[0]);
    CuAssertIntEquals(tc, 2, st[0].symbol->value->vector[1]);
    freeIntVector(iv);
    freeSymbolTable(st);
}

CuSuite *
CuGetSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testUtf8ToInt);
    SUITE_ADD_TEST(suite, testIntToUtf8);
    SUITE_ADD_TEST(suite, testInitXregexTree);
    SUITE_ADD_TEST(suite, testNewXregexNode);
    SUITE_ADD_TEST(suite, testAddChildXregexNode);
    SUITE_ADD_TEST(suite, testDeleteXregexNode);

    SUITE_ADD_TEST(suite, testInitStack);
    SUITE_ADD_TEST(suite, testPushStack);
    SUITE_ADD_TEST(suite, testPopStack);
    SUITE_ADD_TEST(suite, testGetTopStack);

    SUITE_ADD_TEST(suite, testInitIntVector);
    SUITE_ADD_TEST(suite, testAppendIntVector);
    SUITE_ADD_TEST(suite, testDeleteIntVector);
    SUITE_ADD_TEST(suite, testGetIntVectorData);

    SUITE_ADD_TEST(suite, testDealDefiniteRepeat);
    return suite;
}
