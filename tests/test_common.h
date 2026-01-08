#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "../include/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ANSI colors for test output
#define TEST_COLOR_RESET   "\033[0m"
#define TEST_COLOR_GREEN   "\033[32m"
#define TEST_COLOR_RED     "\033[31m"
#define TEST_COLOR_YELLOW  "\033[33m"
#define TEST_COLOR_BOLD    "\033[1m"

// Assertion macros that return 0 on failure
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(TEST_COLOR_RED "  [FAIL] %s:%d: %s" TEST_COLOR_RESET "\n", __FILE__, __LINE__, message); /* flawfinder: ignore */ \
            return 0; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf(TEST_COLOR_RED "  [FAIL] %s:%d: Expected %d, but got %d" TEST_COLOR_RESET "\n", /* flawfinder: ignore */ \
                   __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return 0; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL_STR(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            printf(TEST_COLOR_RED "  [FAIL] %s:%d: Expected '%s', but got '%s'" TEST_COLOR_RESET "\n", /* flawfinder: ignore */ \
                   __FILE__, __LINE__, (expected), (actual)); \
            return 0; \
        } \
    } while (0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf(TEST_COLOR_RED "  [FAIL] %s:%d: Pointer is NULL" TEST_COLOR_RESET "\n", __FILE__, __LINE__); /* flawfinder: ignore */ \
            return 0; \
        } \
    } while (0)

#endif // TEST_COMMON_H
