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
        src/queue.c
        src/binarytree.c
        src/input.c
        src/math.c
        src/string.c
        src/vector.c
        src/utility.c
        src/crypto.c
        src/parsing.c
        src/network.c
        src/memory.c
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
    enable_testing()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(FLINT_TEST_LIBS flint pthread bsd)
    else()
        set(FLINT_TEST_LIBS flint pthread)
    endif()

    set(TESTS
        linkedlist
        set
        stack
        queue
        binarytree
        math
        vector
        string
        crypto
        parsing
        memory
        utility
        input
    )

    foreach(t ${TESTS})
        add_executable(test_${t} tests/test_${t}.c)
        target_include_directories(test_${t} PRIVATE include)
        target_link_libraries(test_${t} ${FLINT_TEST_LIBS})
        add_test(NAME ${t} COMMAND test_${t})
    endforeach()

    # Network test (slow, uses sleep + sockets)
    add_executable(test_network tests/test_network.c)
    target_include_directories(test_network PRIVATE include)
    target_link_libraries(test_network ${FLINT_TEST_LIBS})
    add_test(NAME network COMMAND test_network)
    set_tests_properties(network PROPERTIES LABELS "slow")

    # macOS-only test
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        add_executable(test_macos tests/test_macos.c)
        target_include_directories(test_macos PRIVATE include)
        target_link_libraries(test_macos ${FLINT_TEST_LIBS})
        add_test(NAME macos COMMAND test_macos)
        set_tests_properties(macos PROPERTIES LABELS "slow")
    endif()

    add_executable(netmanual tests/netmanual.c)
    target_include_directories(netmanual PRIVATE include)
    target_link_libraries(netmanual ${FLINT_TEST_LIBS})
endif()
