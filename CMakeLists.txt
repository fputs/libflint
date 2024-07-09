cmake_minimum_required(VERSION 3.17)
project(flint C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    add_compile_definitions(flint __USE_XOPEN_EXTENDED)
endif()

set(CMAKE_C_STANDARD 99)
include_directories(include)

set(SOURCES
        src/linkedlist.c
        src/set.c
        src/stack.c
        src/binarytree.c
        src/input.c
        src/math.c
        src/string.c
        src/vector.c
        src/utility.c
        src/crypto.c
        src/parsing.c
        src/network.c
)

if ((${CMAKE_SYSTEM_NAME} STREQUAL "Darwin"))
    add_library(flint ${SOURCES} src/macos.c)
else()
    add_library(flint ${SOURCES})
endif()

if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    target_link_libraries(flint pthread bsd)
endif()

if(${CMAKE_PROJECT_NAME} STREQUAL flint)
    add_executable(tests tests/tests.c)
    target_include_directories(tests PRIVATE include)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_link_libraries(tests flint pthread bsd)
    else()
        target_link_libraries(tests flint pthread)
    endif()

    add_executable(netmanual tests/netmanual.c)
    target_include_directories(netmanual PRIVATE include)

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_link_libraries(netmanual flint pthread bsd)
    else()
        target_link_libraries(netmanual flint pthread)
    endif()
endif()
