cmake_minimum_required(VERSION 3.10)

project(bdix-server-monitor VERSION 1.0.0 LANGUAGES C)

# C Standard settings (C17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler flags (match Makefile: -Wall -Wextra -Werror)
add_compile_options(-Wall -Wextra -Werror)

# Release optimizations (match Makefile: -O2)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
endif()

# Find Dependencies
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JANSSON REQUIRED jansson)

# Include directories
include_directories(include)
include_directories(${JANSSON_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/checker.c
    src/config.c
    src/main.c
    src/server.c
    src/thread_pool.c
    src/ui.c
)

# Main Executable
add_executable(bdix-monitor ${SOURCES})

# Link libraries
target_link_libraries(bdix-monitor
    PRIVATE
    CURL::libcurl
    Threads::Threads
    ${JANSSON_LIBRARIES}
    m # Math library
)

# Installation (optional, matches Makefile install target)
install(TARGETS bdix-monitor DESTINATION bin)

# Test Suite (matches Makefile tests target)
# Excludes src/main.c and includes tests/*.c
file(GLOB TEST_SOURCES tests/*.c)

if(TEST_SOURCES)
    enable_testing()

    # Filter out main.c from the main sources for the test executable
    list(FILTER SOURCES EXCLUDE REGEX "src/main.c")

    add_executable(test-suite ${SOURCES} ${TEST_SOURCES})
    target_link_libraries(test-suite
        PRIVATE
        CURL::libcurl
        Threads::Threads
        ${JANSSON_LIBRARIES}
        m
    )

    add_test(NAME BaseTest COMMAND test-suite)
endif()
