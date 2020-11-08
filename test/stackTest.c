#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "allTest.h"
#include "stack.h"

void assignInt(void *a, void *b) {
    *(int *)a = *(int *)b;
}
void testInitStack(CuTest *tc) {
    Stack *stack = NULL;
    stack = INIT_STACK(int, 0, assignInt);
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

CuSuite *
CuGetStackSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testInitStack);
    SUITE_ADD_TEST(suite, testPushStack);
    SUITE_ADD_TEST(suite, testPopStack);
    SUITE_ADD_TEST(suite, testGetTopStack);
    return suite;
}
