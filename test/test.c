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
}

CuSuite *CuGetSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testUtf8ToInt);
    return suite;
}
