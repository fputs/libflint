cmake_minimum_required(VERSION 3.17)
project(flint C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
include_directories(include)

set(SOURCES
    src/lflinkedlist.c
    src/lfset.c
    src/lfstack.c
    src/lfbinarytree.c
    src/lfinput.c
    src/lfmath.c
)

add_library(flint ${SOURCES})

if(${CMAKE_PROJECT_NAME} STREQUAL flint)
    add_executable(tests tests/tests.c)
    target_include_directories(tests PRIVATE include)
    target_link_libraries(tests flint)
endif()

