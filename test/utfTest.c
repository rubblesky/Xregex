#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "allTest.h"

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
    CuAssertIntEquals(tc, -1, size);
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
    CuAssertIntEquals(tc, -1, size);
}

CuSuite *
CuGetUtfSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testUtf8ToInt);
    SUITE_ADD_TEST(suite, testIntToUtf8);


    return suite;
}
