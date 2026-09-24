cmake_minimum_required(VERSION 3.17)
project(flint C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)

option(FLINT_ENABLE_WARNINGS "Enable -Wall -Wextra on flint and its tests" ON)
option(FLINT_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

function(flint_apply_c_flags tgt)
    if(FLINT_ENABLE_WARNINGS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${tgt} PRIVATE -Wall -Wextra)
    endif()
    if(FLINT_SANITIZE)
        target_compile_options(${tgt} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer)
        target_link_options(${tgt} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

set(SOURCES
        src/linkedlist.c
        src/set.c
        src/stack.c
        src/queue.c
        src/binarytree.c
        src/input.c
        src/process.c
        src/math.c
        src/string.c
        src/vector.c
        src/crypto.c
        src/network.c
        src/memory.c
        src/ringbuf.c
        src/bytering.c
        src/statemachine.c
        src/compat.c
        src/bst.c
        src/bytebuf.c
        src/bitset.c
        src/strbuf.c
        src/heap.c
        src/hashmap.c
        src/hashset.c
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

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # input.c uses strtonum from libbsd
    target_link_libraries(flint PRIVATE bsd)
endif()

flint_apply_c_flags(flint)

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
        memory
        input
        ringbuf
        bytering
        statemachine
        bitops
        network
        bst
        bytebuf
        bitset
        strbuf
        heap
        hashmap
        hashset
    )

    foreach(t ${TESTS})
        add_executable(test_${t} tests/test_${t}.c)
        target_link_libraries(test_${t} PRIVATE flint)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_compile_definitions(test_${t} PRIVATE _DEFAULT_SOURCE)
        endif()
        flint_apply_c_flags(test_${t})
        add_test(NAME ${t} COMMAND test_${t})
    endforeach()

    if(APPLE)
        add_executable(test_macos tests/test_macos.c)
        target_link_libraries(test_macos PRIVATE flint)
        flint_apply_c_flags(test_macos)
        add_test(NAME macos COMMAND test_macos)
        set_tests_properties(macos PROPERTIES LABELS "slow")
    endif()

    add_executable(netmanual tests/netmanual.c)
    target_link_libraries(netmanual PRIVATE flint)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(netmanual PRIVATE _DEFAULT_SOURCE)
    endif()
    flint_apply_c_flags(netmanual)
endif()
