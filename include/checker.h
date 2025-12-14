/**
 * @file checker.h
 * @brief HTTP server checking functionality
 * @version 1.0.0
 */

#ifndef BDIX_CHECKER_H
#define BDIX_CHECKER_H

#include "common.h"
#include "server.h"

/**
 * @brief Checker configuration
 */
typedef struct {
    int timeout_seconds;            // HTTP timeout
    int connect_timeout_seconds;    // Connection timeout
    bool follow_redirects;          // Follow HTTP redirects
    int max_redirects;              // Maximum number of redirects
    bool verify_ssl;                // Verify SSL certificates
    bool verbose;                   // Verbose output
} CheckerConfig;

/**
 * @brief Checker statistics
 */
typedef struct {
    _Atomic size_t total_checked;
    _Atomic size_t online_count;
    _Atomic size_t offline_count;
    _Atomic size_t timeout_count;
    _Atomic size_t error_count;
    _Atomic double total_latency_ms;
    _Atomic double min_latency_ms;
    _Atomic double max_latency_ms;
} CheckerStats;

/**
 * @brief Initialize checker subsystem
 *
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int checker_init(void);

/**
 * @brief Cleanup checker subsystem
 */
void checker_cleanup(void);

/**
 * @brief Get default checker configuration
 *
 * @return Default configuration structure
 */
CheckerConfig checker_get_default_config(void);

/**
 * @brief Check a single server
 *
 * @param server Pointer to server to check
 * @param config Pointer to checker configuration
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int checker_check_server(Server *server, const CheckerConfig *config);

/**
 * @brief Check all servers in a category
 *
 * @param category Pointer to server category
 * @param config Pointer to checker configuration
 * @param thread_count Number of threads to use
 * @param stats Pointer to statistics (optional)
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int checker_check_category(ServerCategory *category, const CheckerConfig *config,
                           int thread_count, CheckerStats *stats);

/**
 * @brief Check multiple categories
 *
 * @param data Pointer to server data
 * @param config Pointer to checker configuration
 * @param thread_count Number of threads to use
 * @param check_ftp Check FTP category
 * @param check_tv Check TV category
 * @param check_others Check others category
 * @param stats Pointer to statistics (optional)
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int checker_check_multiple(ServerData *data, const CheckerConfig *config,
                           int thread_count, bool check_ftp, bool check_tv,
                           bool check_others, CheckerStats *stats);

/**
 * @brief Initialize statistics structure
 *
 * @param stats Pointer to statistics structure
 */
void checker_stats_init(CheckerStats *stats);

/**
 * @brief Update statistics with server check result
 *
 * @param stats Pointer to statistics
 * @param server Pointer to checked server
 */
void checker_stats_update(CheckerStats *stats, const Server *server);

/**
 * @brief Print statistics summary
 *
 * @param stats Pointer to statistics
 */
void checker_stats_print(const CheckerStats *stats);

/**
 * @brief Get average latency from statistics
 *
 * @param stats Pointer to statistics
 * @return Average latency in milliseconds
 */
double checker_stats_get_avg_latency(const CheckerStats *stats);

#endif // BDIX_CHECKER_H
