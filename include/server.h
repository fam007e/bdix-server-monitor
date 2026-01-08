/**
 * @file server.h
 * @brief Server data structures and management functions
 * @version 1.0.0
 */

#ifndef BDIX_SERVER_H
#define BDIX_SERVER_H

#include "common.h"

/**
 * @brief Server category types
 */
typedef enum {
    CATEGORY_FTP,
    CATEGORY_TV,
    CATEGORY_OTHERS,
    CATEGORY_COUNT
} ServerCategoryType;

/**
 * @brief Server status
 */
typedef enum {
    BDIX_STATUS_UNKNOWN,
    BDIX_STATUS_ONLINE,
    BDIX_STATUS_OFFLINE,
    BDIX_STATUS_TIMEOUT,
    BDIX_STATUS_ERROR
} ServerStatus;

/**
 * @brief Individual server information
 */
typedef struct {
    char url[MAX_URL_LENGTH]; /* flawfinder: ignore - bounds checked with safe_strncpy */
    ServerStatus status;
    double latency_ms;
    long response_code;
    time_t last_checked;
} Server;

/**
 * @brief Server category structure with flexible array
 */
typedef struct {
    char **urls;                    // Array of server URLs
    Server *servers;                // Array of server information
    size_t count;                   // Current number of servers
    size_t capacity;                // Allocated capacity
    ServerCategoryType type;        // Category type
    const char *name;               // Category name
} ServerCategory;

/**
 * @brief Complete server data structure
 */
typedef struct {
    ServerCategory ftp;
    ServerCategory tv;
    ServerCategory others;
    size_t total_servers;
} ServerData;

/**
 * @brief Initialize a server category
 *
 * @param category Pointer to category to initialize
 * @param type Category type
 * @param name Category name
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int server_category_init(ServerCategory *category, ServerCategoryType type, const char *name);

/**
 * @brief Free server category resources
 *
 * @param category Pointer to category to free
 */
void server_category_free(ServerCategory *category);

/**
 * @brief Add a server URL to a category
 *
 * @param category Pointer to category
 * @param url Server URL to add
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int server_category_add(ServerCategory *category, const char *url);

/**
 * @brief Get server at index
 *
 * @param category Pointer to category
 * @param index Server index
 * @return Pointer to server or NULL if invalid
 */
Server* server_category_get(ServerCategory *category, size_t index);

/**
 * @brief Update server status
 *
 * @param server Pointer to server
 * @param status New status
 * @param latency_ms Latency in milliseconds
 * @param response_code HTTP response code
 */
void server_update_status(Server *server, ServerStatus status,
                         double latency_ms, long response_code);

/**
 * @brief Initialize server data structure
 *
 * @param data Pointer to server data
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int server_data_init(ServerData *data);

/**
 * @brief Free server data resources
 *
 * @param data Pointer to server data
 */
void server_data_free(ServerData *data);

/**
 * @brief Get category by type
 *
 * @param data Pointer to server data
 * @param type Category type
 * @return Pointer to category or NULL
 */
ServerCategory* server_data_get_category(ServerData *data, ServerCategoryType type);

/**
 * @brief Get total server count
 *
 * @param data Pointer to server data
 * @return Total number of servers
 */
size_t server_data_count(const ServerData *data);

/**
 * @brief Print server statistics
 *
 * @param data Pointer to server data
 */
void server_data_print_stats(const ServerData *data);

/**
 * @brief Get category name as string
 *
 * @param type Category type
 * @return Category name string
 */
const char* server_category_name(ServerCategoryType type);

/**
 * @brief Get status name as string
 *
 * @param status Server status
 * @return Status name string
 */
const char* server_status_name(ServerStatus status);

#endif // BDIX_SERVER_H
