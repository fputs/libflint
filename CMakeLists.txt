cmake_minimum_required(VERSION 3.17)
project(flint C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)

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
        src/ringbuf.c
        src/bytering.c
        src/statemachine.c
)

add_library(flint ${SOURCES})

if(APPLE)
    target_sources(flint PRIVATE src/macos.c)
endif()

# Public headers for both in-tree tests and add_subdirectory() consumers.
# Feature-test macros stay PRIVATE so they do not leak into a parent project.
target_include_directories(flint PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(flint PRIVATE _DEFAULT_SOURCE _GNU_SOURCE)
endif()

find_package(Threads REQUIRED)
target_link_libraries(flint PRIVATE Threads::Threads)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(flint PRIVATE bsd)
endif()

if(CMAKE_PROJECT_NAME STREQUAL flint)
    enable_testing()

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
        ringbuf
        bytering
        statemachine
        bitops
    )

    foreach(t ${TESTS})
        add_executable(test_${t} tests/test_${t}.c)
        target_link_libraries(test_${t} PRIVATE flint)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_compile_definitions(test_${t} PRIVATE _DEFAULT_SOURCE)
        endif()
        add_test(NAME ${t} COMMAND test_${t})
    endforeach()

    # Network test (slow, uses sleep + sockets)
    add_executable(test_network tests/test_network.c)
    target_link_libraries(test_network PRIVATE flint)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(test_network PRIVATE _DEFAULT_SOURCE)
    endif()
    add_test(NAME network COMMAND test_network)
    set_tests_properties(network PROPERTIES LABELS "slow")

    if(APPLE)
        add_executable(test_macos tests/test_macos.c)
        target_link_libraries(test_macos PRIVATE flint)
        add_test(NAME macos COMMAND test_macos)
        set_tests_properties(macos PROPERTIES LABELS "slow")
    endif()

    add_executable(netmanual tests/netmanual.c)
    target_link_libraries(netmanual PRIVATE flint)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(netmanual PRIVATE _DEFAULT_SOURCE)
    endif()
endif()
