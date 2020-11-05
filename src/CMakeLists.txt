set(SRC_LIST "xregex.c" "stack.c" "vector.c" "automaton2.c" "intvector.c")
#"lexical.c"
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(Xregex SHARED)
target_sources(Xregex 
PRIVATE
    ${SRC_LIST}
)


set_target_properties(Xregex
    PROPERTIES
        PUBLIC_HEADER
            ${CMAKE_BUILD_RPATH}/include/xregex.h
)

target_include_directories(Xregex
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)
