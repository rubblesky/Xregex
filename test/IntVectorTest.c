#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "allTest.h"
#include "xregex.h"
void testInitIntVector(CuTest *tc) {
    IntVector *iv = NULL;
    iv = initIntVector(5);
    CuAssertIntEquals(tc, iv->allocSize, 5);
    CuAssertPtrNotNull(tc, iv);
    CuAssertIntEquals(tc, iv->dataSize, 0);
    freeIntVector(iv);
}
void testAppendIntVector(CuTest *tc) {
    IntVector *iv = initIntVector(2);
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

CuSuite *
CuGetIntVectorSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testInitIntVector);
    SUITE_ADD_TEST(suite, testAppendIntVector);
    SUITE_ADD_TEST(suite, testDeleteIntVector);
    SUITE_ADD_TEST(suite, testGetIntVectorData);
    return suite;
}
