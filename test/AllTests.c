#include <stdio.h>

#include "CuTest.h"
#include "allTest.h"

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    //CuSuiteAddSuite(suite, CuGetUtfSuite());
    CuSuiteAddSuite(suite, CuGetRegexSuite());



    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void) {
    RunAllTests();
}

