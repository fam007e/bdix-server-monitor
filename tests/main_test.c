#include "test_common.h"

// Test statistics
static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

// Test Runner Macros
#define RUN_TEST(test_func) \
    do { \
        printf(TEST_COLOR_YELLOW "[RUNNING] %s" TEST_COLOR_RESET "\n", #test_func); /* flawfinder: ignore */ \
        g_tests_run++; \
        if (test_func()) { \
            printf(TEST_COLOR_GREEN "[PASS] %s" TEST_COLOR_RESET "\n", #test_func); /* flawfinder: ignore */ \
            g_tests_passed++; \
        } else { \
            printf(TEST_COLOR_RED "[FAIL] %s" TEST_COLOR_RESET "\n", #test_func); /* flawfinder: ignore */ \
            g_tests_failed++; \
        } \
    } while (0)

#define PRINT_TEST_SUMMARY() \
    do { \
        printf("\n" TEST_COLOR_BOLD "Test Summary:" TEST_COLOR_RESET "\n"); /* flawfinder: ignore */ \
        printf("  Total:  %d\n", g_tests_run); /* flawfinder: ignore */ \
        printf("  Passed: " TEST_COLOR_GREEN "%d" TEST_COLOR_RESET "\n", g_tests_passed); /* flawfinder: ignore */ \
        printf("  Failed: " TEST_COLOR_RED "%d" TEST_COLOR_RESET "\n", g_tests_failed); /* flawfinder: ignore */ \
    } while (0)

// External declarations for test functions
extern int test_server_category_init(void);
extern int test_server_category_add(void);
extern int test_server_category_resize(void);
extern int test_server_data_lifecycle(void);
extern int test_server_update_status(void);

extern int test_checker_init_cleanup(void);
extern int test_checker_config(void);
extern int test_checker_stats(void);

extern int test_config_load_string(void);
extern int test_config_load_invalid(void);
extern int test_config_sample_creation(void);

int main(void) {
    printf(TEST_COLOR_BOLD "Running BDIX Server Monitor Test Suite...\n\n" TEST_COLOR_RESET); // flawfinder: ignore

    // Server Tests
    printf(TEST_COLOR_BOLD "--- Server Module Tests ---\n" TEST_COLOR_RESET); // flawfinder: ignore
    RUN_TEST(test_server_category_init);
    RUN_TEST(test_server_category_add);
    RUN_TEST(test_server_category_resize);
    RUN_TEST(test_server_data_lifecycle);
    RUN_TEST(test_server_update_status);
    printf("\n"); // flawfinder: ignore

    // Checker Tests
    printf(TEST_COLOR_BOLD "--- Checker Module Tests ---\n" TEST_COLOR_RESET); // flawfinder: ignore
    RUN_TEST(test_checker_init_cleanup);
    RUN_TEST(test_checker_config);
    RUN_TEST(test_checker_stats);
    printf("\n"); // flawfinder: ignore

    // Config Tests
    printf(TEST_COLOR_BOLD "--- Config Module Tests ---\n" TEST_COLOR_RESET); // flawfinder: ignore
    RUN_TEST(test_config_load_string);
    RUN_TEST(test_config_load_invalid);
    RUN_TEST(test_config_sample_creation);

    PRINT_TEST_SUMMARY();

    return g_tests_failed > 0 ? 1 : 0;
}
