cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

set(executable_name dbms)
string(TOUPPER ${executable_name} executable_name_uppercase)
set(main_file main.cpp)

project(${executable_name} VERSION 1.0.0)

# Detect Build Type

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(${executable_name_uppercase}_DEBUG OFF)
else()
    set(${executable_name_uppercase}_DEBUG ON)
endif()

# Options:

# option(${executable_name_uppercase}_BUILD_TESTS "Build tests." OFF)

# Includes:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

include(DetectOS)
include(CompilerOptions)

# Default compiler options macro:

macro(target_set_default_flags target)

    if(OS STREQUAL "linux")
        set(common_flags "-std=c++20")
        set(debug_flags "")
        set(release_flags "")
    elseif(OS STREQUAL "darwin")
        set(common_flags "-std=c++20")
        set(debug_flags "")
        set(release_flags "")
    elseif(OS STREQUAL "windows")
        set(common_flags "/std:c++20")
        set(debug_flags "")
        set(release_flags "")
    endif()

    generate_common_flags(
        common_flags "${common_flags}"
        debug_flags "${debug_flags}"
        release_flags "${release_flags}"
    )

    if(${executable_name_uppercase}_DEBUG)
        target_compile_options(${executable_name} PRIVATE ${common_flags} ${debug_flags})
    else()
        target_compile_options(${executable_name} PRIVATE ${common_flags} ${release_flags})
    endif()

endmacro()

# Create executable:

set(src_files
    src/core/logger.cpp
    src/core/measurements.cpp
    src/core/utils.cpp
    src/core/random.cpp

    src/dbms.cpp
    src/memory_mapped_file.cpp
    src/sort.cpp
    src/hash_join.cpp
    src/cross_join.cpp
    src/merge_join.cpp
    src/binary_search_join.cpp
)

add_executable(${executable_name} ${src_files} main.cpp)

# Unify symbol visibility across all platforms:
set_target_properties(${executable_name} PROPERTIES
    C_VISIBILITY_PRESET hidden
    C_VISIBILITY_INLINE_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_INLINE_PRESET hidden
)

target_include_directories(${executable_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_compile_definitions(${executable_name} PUBLIC
    ${executable_name_uppercase}_DEBUG="$<BOOL:${executable_name_uppercase}_DEBUG>"
    ${executable_name_uppercase}_VERSION="${PROJECT_VERSION}"
    ${executable_name_uppercase}_BINARY_PATH="${CMAKE_BINARY_DIR}"
    ${executable_name_uppercase}_DATA_PATH="${CMAKE_SOURCE_DIR}/db"
    CORE_ASSERT_ENABLED=${CORE_ASSERT_ENABLED}
)

target_set_default_flags(${executable_name})

if(BUILD_TESTS)
    message(STATUS "Configuring tests")

    set(test_executable ${executable_name}_test)

    set(test_src_files
        tests/t-index.cpp
        tests/t-dbms.cpp
        tests/t-hash_join.cpp
        tests/t-cross_join.cpp
        tests/t-merge_join.cpp
        tests/t-binary_search_join.cpp
    )

    add_executable(${test_executable} ${src_files} ${test_src_files} test_main.cpp)

    target_include_directories(${test_executable} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )

    target_compile_definitions(${test_executable} PUBLIC
        ${executable_name_uppercase}_DEBUG="$<BOOL:${executable_name_uppercase}_DEBUG>"
        ${executable_name_uppercase}_VERSION="${PROJECT_VERSION}"
        ${executable_name_uppercase}_BINARY_PATH="${CMAKE_BINARY_DIR}"
        ${executable_name_uppercase}_DATA_PATH="${CMAKE_SOURCE_DIR}/db"
        CORE_ASSERT_ENABLED=${CORE_ASSERT_ENABLED}
    )

    target_set_default_flags(${test_executable})

    enable_testing()
    add_test(NAME ${test_executable} COMMAND ${test_executable})
endif()
