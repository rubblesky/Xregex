add_executable(test)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test)
set(TEST_LIST "test.c")

target_sources(test
PRIVATE
    ${TEST_LIST}
)
target_link_libraries(test
PRIVATE
    Xregex
)
