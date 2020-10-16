add_executable(test)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test)
set(TEST_LIST "AllTests.c" "CuTest.c" "test.c")

enable_testing()
add_test(cutest test)

target_sources(test
PRIVATE
    ${TEST_LIST}
)
target_link_libraries(test
PRIVATE
    Xregex
)

target_include_directories(test
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/src
)