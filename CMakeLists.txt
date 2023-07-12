cmake_minimum_required(VERSION 3.17)
project(flint C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
include_directories(include)

set(SOURCES
        src/linkedlist.c
        src/set.c
        src/stack.c
        src/binarytree.c
        src/input.c
        src/math.c
)

add_library(flint ${SOURCES})
if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    target_link_libraries(flint bsd)
endif()

if(${CMAKE_PROJECT_NAME} STREQUAL flint)
    add_executable(tests tests/tests.c)
    target_include_directories(tests PRIVATE include)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        target_link_libraries(tests flint)
    else()
        target_link_libraries(tests flint bsd)
    endif()
endif()
