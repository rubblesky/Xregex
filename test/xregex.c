#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "allTest.h"

#include "xregex.h"

void testRegex(CuTest *tc){
    char *re = "a((bc)*|d*)e";
    IntVector *iv = transExpress(re);
    IntVector* iv2 = getEscapeCharacterExpress(iv);
    freeIntVector(iv);
    LexicalResult *lr = lexicalAnalyse(iv2);
    freeIntVector(iv2);
    getRegexTree(lr);

}
CuSuite *
CuGetRegexSuite(void) {
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, testRegex);

    return suite;
}