#include "test_common.h"
#include "../include/checker.h"

int test_checker_init_cleanup(void) {
    // Just verify we can init and cleanup without crashing
    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, checker_init());
    checker_cleanup();
    return 1;
}

int test_checker_config(void) {
    CheckerConfig cfg = checker_get_default_config();

    // Check reasonable defaults
    TEST_ASSERT(cfg.timeout_seconds > 0, "Timeout should be positive");
    TEST_ASSERT(cfg.connect_timeout_seconds > 0, "Connect timeout should be positive");
    TEST_ASSERT(cfg.max_redirects >= 0, "Max redirects should be non-negative");

    return 1;
}

int test_checker_stats(void) {
    CheckerStats stats;
    checker_stats_init(&stats);

    TEST_ASSERT_EQUAL_INT(0, stats.total_checked);
    TEST_ASSERT_EQUAL_INT(0, stats.online_count);

    Server s;
    memset(&s, 0, sizeof(Server));
    s.status = BDIX_STATUS_ONLINE;
    s.latency_ms = 100.0;

    checker_stats_update(&stats, &s);

    TEST_ASSERT_EQUAL_INT(1, stats.total_checked);
    TEST_ASSERT_EQUAL_INT(1, stats.online_count);
    TEST_ASSERT_EQUAL_INT(0, stats.offline_count);
    TEST_ASSERT(stats.total_latency_ms == 100.0, "Total latency mismatch");

    s.status = BDIX_STATUS_OFFLINE;
    checker_stats_update(&stats, &s);

    TEST_ASSERT_EQUAL_INT(2, stats.total_checked);
    TEST_ASSERT_EQUAL_INT(1, stats.online_count);
    TEST_ASSERT_EQUAL_INT(1, stats.offline_count);

    return 1;
}
