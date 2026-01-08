/**
 * @file checker.c
 * @brief HTTP server checking implementation
 * @version 1.0.0
 */

#include "checker.h"
#include "thread_pool.h"
#include "ui.h"
#include <curl/curl.h>

/**
 * @brief CURL write callback that discards data
 */
static size_t curl_discard_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    UNUSED(contents);
    UNUSED(userp);
    return size * nmemb;
}

/**
 * @brief Initialize checker subsystem
 */
int checker_init(void) {
    CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
    if (code != CURLE_OK) {
        LOG_ERROR("Failed to initialize CURL: %s", curl_easy_strerror(code));
        return BDIX_ERROR;
    }

    LOG_INFO("Checker subsystem initialized");
    return BDIX_SUCCESS;
}

/**
 * @brief Cleanup checker subsystem
 */
void checker_cleanup(void) {
    curl_global_cleanup();
    LOG_INFO("Checker subsystem cleaned up");
}

/**
 * @brief Get default checker configuration
 */
CheckerConfig checker_get_default_config(void) {
    return (CheckerConfig){
        .timeout_seconds = HTTP_TIMEOUT_SECONDS,
        .connect_timeout_seconds = HTTP_CONNECT_TIMEOUT,
        .follow_redirects = false,
        .max_redirects = 0,
        .verify_ssl = true,
        .verbose = true
    };
}

/**
 * @brief Check a single server
 */
int checker_check_server(Server *server, const CheckerConfig *config) {
    if (!server || !config) {
        LOG_ERROR("Invalid parameters for server check");
        return BDIX_ERROR_INVALID_INPUT;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to initialize CURL handle");
        return BDIX_ERROR;
    }

    // Configure CURL for secure operation
    curl_easy_setopt(curl, CURLOPT_URL, server->url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  // HEAD request
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)config->timeout_seconds);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, (long)config->connect_timeout_seconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, config->follow_redirects ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, (long)config->max_redirects);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, config->verify_ssl ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, config->verify_ssl ? 2L : 0L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);  // Thread safety
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_discard_callback);

    // Disable verbose output
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

    // Measure latency
    double start_time = get_time_ms();
    CURLcode res = curl_easy_perform(curl);
    double latency_ms = get_time_ms() - start_time;

    // Get response code
    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    // Determine status
    ServerStatus status;
    if (res == CURLE_OK) {
        if (response_code >= 200 && response_code < 400) {
            status = BDIX_STATUS_ONLINE;
        } else {
            status = BDIX_STATUS_OFFLINE;
        }
    } else if (res == CURLE_OPERATION_TIMEDOUT) {
        status = BDIX_STATUS_TIMEOUT;
    } else {
        status = BDIX_STATUS_ERROR;
        LOG_DEBUG("CURL error for %s: %s", server->url, curl_easy_strerror(res));
    }

    // Update server status
    server_update_status(server, status, latency_ms, response_code);

    curl_easy_cleanup(curl);
    return BDIX_SUCCESS;
}

/**
 * @brief Work item for thread pool
 */
typedef struct {
    Server *server;
    const CheckerConfig *config;
    CheckerStats *stats;
    const char *category_name;
    size_t index;
    size_t total;
    bool show_only_ok;
} CheckWorkItem;

/**
 * @brief Thread worker function for checking servers
 */
static void* check_worker(void *arg) {
    CheckWorkItem *work = (CheckWorkItem*)arg;

    if (!work) {
        return NULL;
    }

    // Check the server
    checker_check_server(work->server, work->config);

    // Update statistics
    if (work->stats) {
        checker_stats_update(work->stats, work->server);
    }

    // Print result
    ui_print_check_result(work->server, work->category_name,
                         work->index + 1, work->total, work->show_only_ok);

    // Free work item
    free(work);

    return NULL;
}

/**
 * @brief Check all servers in a category
 */
int checker_check_category(ServerCategory *category, const CheckerConfig *config,
                           int thread_count, CheckerStats *stats) {
    if (!category || !config) {
        LOG_ERROR("Invalid parameters for category check");
        return BDIX_ERROR_INVALID_INPUT;
    }

    if (category->count == 0) {
        LOG_INFO("No servers to check in category '%s'", category->name);
        return BDIX_SUCCESS;
    }

    // Create thread pool
    ThreadPool *pool = thread_pool_create(thread_count);
    if (!pool) {
        LOG_ERROR("Failed to create thread pool");
        return BDIX_ERROR_THREAD;
    }

    LOG_INFO("Checking %zu servers in '%s' category with %d threads",
             category->count, category->name, thread_count);

    // Submit all work items
    for (size_t i = 0; i < category->count; i++) {
        CheckWorkItem *work = safe_malloc(sizeof(CheckWorkItem));

        work->server = &category->servers[i];
        work->config = config;
        work->stats = stats;
        work->category_name = category->name;
        work->index = i;
        work->total = category->count;
        work->show_only_ok = !config->verbose;

        if (thread_pool_add_work(pool, check_worker, work) != BDIX_SUCCESS) {
            LOG_ERROR("Failed to add work to thread pool");
            free(work);
            thread_pool_destroy(pool);
            return BDIX_ERROR_THREAD;
        }
    }

    // Wait for all work to complete
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    LOG_INFO("Completed checking '%s' category", category->name);
    return BDIX_SUCCESS;
}

/**
 * @brief Check multiple categories
 */
int checker_check_multiple(ServerData *data, const CheckerConfig *config,
                           int thread_count, bool check_ftp, bool check_tv,
                           bool check_others, CheckerStats *stats) {
    if (!data || !config) {
        LOG_ERROR("Invalid parameters for multiple category check");
        return BDIX_ERROR_INVALID_INPUT;
    }

    // Count total servers to check
    size_t total = 0;
    if (check_ftp) total += data->ftp.count;
    if (check_tv) total += data->tv.count;
    if (check_others) total += data->others.count;

    if (total == 0) {
        LOG_INFO("No servers to check");
        return BDIX_SUCCESS;
    }

    printf("\n"); /* flawfinder: ignore */
    LOG_INFO("Checking %zu servers with %d threads...", total, thread_count);
    printf("\n"); /* flawfinder: ignore */

    // Check each category
    int ret = BDIX_SUCCESS;

    if (check_ftp && data->ftp.count > 0) {
        ret = checker_check_category(&data->ftp, config, thread_count, stats);
        if (ret != BDIX_SUCCESS) {
            LOG_ERROR("Failed to check FTP category");
            return ret;
        }
    }

    if (check_tv && data->tv.count > 0) {
        ret = checker_check_category(&data->tv, config, thread_count, stats);
        if (ret != BDIX_SUCCESS) {
            LOG_ERROR("Failed to check TV category");
            return ret;
        }
    }

    if (check_others && data->others.count > 0) {
        ret = checker_check_category(&data->others, config, thread_count, stats);
        if (ret != BDIX_SUCCESS) {
            LOG_ERROR("Failed to check Others category");
            return ret;
        }
    }

    printf("\n"); /* flawfinder: ignore */
    LOG_INFO("All checks completed");

    return BDIX_SUCCESS;
}

/**
 * @brief Initialize statistics structure
 */
void checker_stats_init(CheckerStats *stats) {
    if (!stats) {
        return;
    }

    atomic_store(&stats->total_checked, 0);
    atomic_store(&stats->online_count, 0);
    atomic_store(&stats->offline_count, 0);
    atomic_store(&stats->timeout_count, 0);
    atomic_store(&stats->error_count, 0);
    atomic_store(&stats->total_latency_ms, 0.0);
    atomic_store(&stats->min_latency_ms, INFINITY);
    atomic_store(&stats->max_latency_ms, 0.0);

    LOG_DEBUG("Statistics initialized");
}

/**
 * @brief Update statistics with server check result
 */
void checker_stats_update(CheckerStats *stats, const Server *server) {
    if (!stats || !server) {
        return;
    }

    // Increment total
    atomic_fetch_add(&stats->total_checked, 1);

    // Update status counters
    switch (server->status) {
        case BDIX_STATUS_ONLINE:
            atomic_fetch_add(&stats->online_count, 1);
            break;
        case BDIX_STATUS_OFFLINE:
            atomic_fetch_add(&stats->offline_count, 1);
            break;
        case BDIX_STATUS_TIMEOUT:
            atomic_fetch_add(&stats->timeout_count, 1);
            break;
        case BDIX_STATUS_ERROR:
            atomic_fetch_add(&stats->error_count, 1);
            break;
        default:
            break;
    }

    // Update latency statistics (only for successful checks)
    if (server->status == BDIX_STATUS_ONLINE && server->latency_ms >= 0) {
        double current_total = atomic_load(&stats->total_latency_ms);
        while (!atomic_compare_exchange_weak(&stats->total_latency_ms, &current_total,
                                             current_total + server->latency_ms)) {
            // Retry on failure
        }

        // Update min latency
        double current_min = atomic_load(&stats->min_latency_ms);
        while (server->latency_ms < current_min) {
            if (atomic_compare_exchange_weak(&stats->min_latency_ms, &current_min,
                                            server->latency_ms)) {
                break;
            }
        }

        // Update max latency
        double current_max = atomic_load(&stats->max_latency_ms);
        while (server->latency_ms > current_max) {
            if (atomic_compare_exchange_weak(&stats->max_latency_ms, &current_max,
                                            server->latency_ms)) {
                break;
            }
        }
    }
}

/**
 * @brief Get average latency from statistics
 */
double checker_stats_get_avg_latency(const CheckerStats *stats) {
    if (!stats) {
        return 0.0;
    }

    size_t online = atomic_load(&stats->online_count);
    if (online == 0) {
        return 0.0;
    }

    double total = atomic_load(&stats->total_latency_ms);
    return total / online;
}

/**
 * @brief Print statistics summary
 */
/* flawfinder: ignore - all printf calls below use compile-time constant format strings */
void checker_stats_print(const CheckerStats *stats) {
    if (!stats) {
        return;
    }

    size_t total = atomic_load(&stats->total_checked);
    size_t online = atomic_load(&stats->online_count);
    size_t offline = atomic_load(&stats->offline_count);
    size_t timeout = atomic_load(&stats->timeout_count);
    size_t error = atomic_load(&stats->error_count);

    double min_latency = atomic_load(&stats->min_latency_ms);
    double max_latency = atomic_load(&stats->max_latency_ms);
    double avg_latency = checker_stats_get_avg_latency(stats);

    printf("\n");
    printf("═══════════════════════════════════════════\n");
    printf("           CHECK STATISTICS\n");
    printf("═══════════════════════════════════════════\n");
    printf("Total Checked:   %5zu\n", total);
    printf("───────────────────────────────────────────\n");
    printf("Online:          %5zu  (%.1f%%)\n", online,
           total > 0 ? (online * 100.0 / total) : 0.0);
    printf("Offline:         %5zu  (%.1f%%)\n", offline,
           total > 0 ? (offline * 100.0 / total) : 0.0);
    printf("Timeout:         %5zu  (%.1f%%)\n", timeout,
           total > 0 ? (timeout * 100.0 / total) : 0.0);
    printf("Error:           %5zu  (%.1f%%)\n", error,
           total > 0 ? (error * 100.0 / total) : 0.0);

    if (online > 0) {
        printf("───────────────────────────────────────────\n");
        printf("Min Latency:     %.2f ms\n", min_latency);
        printf("Max Latency:     %.2f ms\n", max_latency);
        printf("Avg Latency:     %.2f ms\n", avg_latency);
    }

    printf("═══════════════════════════════════════════\n");
    printf("\n");
}
