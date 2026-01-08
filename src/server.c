/**
 * @file server.c
 * @brief Server data structures and management implementation
 * @version 1.0.0
 */

#include "server.h"

// Global category names
static const char* CATEGORY_NAMES[] = {
    [CATEGORY_FTP] = "FTP",
    [CATEGORY_TV] = "TV",
    [CATEGORY_OTHERS] = "Others"
};

/**
 * @brief Get category name as string
 */
const char* server_category_name(ServerCategoryType type) {
    if (type >= CATEGORY_COUNT) {
        return "Unknown";
    }
    return CATEGORY_NAMES[type];
}

/**
 * @brief Get status name as string
 */
const char* server_status_name(ServerStatus status) {
    switch (status) {
        case BDIX_STATUS_UNKNOWN: return "UNKNOWN";
        case BDIX_STATUS_ONLINE:  return "ONLINE";
        case BDIX_STATUS_OFFLINE: return "OFFLINE";
        case BDIX_STATUS_TIMEOUT: return "TIMEOUT";
        case BDIX_STATUS_ERROR:   return "ERROR";
        default:                  return "INVALID";
    }
}

/**
 * @brief Initialize a server category
 */
int server_category_init(ServerCategory *category, ServerCategoryType type, const char *name) {
    if (!category || !name) {
        LOG_ERROR("Invalid parameters for category initialization");
        return BDIX_ERROR_INVALID_INPUT;
    }

    // Initialize with designated initializer for safety
    *category = (ServerCategory){
        .urls = safe_calloc(32, sizeof(char*)),
        .servers = safe_calloc(32, sizeof(Server)),
        .count = 0,
        .capacity = 32,
        .type = type,
        .name = name
    };

    LOG_DEBUG("Initialized category '%s' with capacity 32", name);
    return BDIX_SUCCESS;
}

/**
 * @brief Free server category resources
 */
void server_category_free(ServerCategory *category) {
    if (!category) {
        return;
    }

    LOG_DEBUG("Freeing category '%s' with %zu servers",
              category->name ? category->name : "Unknown", category->count);

    // Free all URL strings
    if (category->urls) {
        for (size_t i = 0; i < category->count; i++) {
            free(category->urls[i]);
            category->urls[i] = NULL;
        }
        free(category->urls);
        category->urls = NULL;
    }

    // Free server array
    if (category->servers) {
        free(category->servers);
        category->servers = NULL;
    }

    // Clear all fields
    memset(category, 0, sizeof(ServerCategory));
}

/**
 * @brief Add a server URL to a category
 */
int server_category_add(ServerCategory *category, const char *url) {
    if (!category || !url) {
        LOG_ERROR("Invalid parameters for adding server");
        return BDIX_ERROR_INVALID_INPUT;
    }

    // Validate URL
    if (!is_valid_url(url)) {
        LOG_WARN("Invalid URL format: %s", url);
        return BDIX_ERROR_INVALID_INPUT;
    }

    // Check capacity limit
    if (category->count >= MAX_SERVERS_PER_CATEGORY) {
        LOG_ERROR("Maximum servers limit reached for category '%s'", category->name);
        return BDIX_ERROR;
    }

    // Resize if needed
    if (category->count >= category->capacity) {
        size_t new_capacity = category->capacity * 2;

        // Reallocate URLs array
        char **new_urls = safe_realloc(category->urls, new_capacity * sizeof(char*));
        category->urls = new_urls;

        // Reallocate servers array
        Server *new_servers = safe_realloc(category->servers, new_capacity * sizeof(Server));
        category->servers = new_servers;

        // Zero out new memory
        memset(&category->servers[category->capacity], 0,
               (new_capacity - category->capacity) * sizeof(Server));

        category->capacity = new_capacity;
        LOG_DEBUG("Resized category '%s' to capacity %zu", category->name, new_capacity);
    }

    // Add URL string
    category->urls[category->count] = safe_strdup(url);

    // Initialize server structure
    Server *server = &category->servers[category->count];
    safe_strncpy(server->url, url, MAX_URL_LENGTH);
    server->status = BDIX_STATUS_UNKNOWN;
    server->latency_ms = -1.0;
    server->response_code = 0;
    server->last_checked = 0;

    category->count++;

    LOG_DEBUG("Added server to '%s': %s (count: %zu)",
              category->name, url, category->count);

    return BDIX_SUCCESS;
}

/**
 * @brief Get server at index
 */
Server* server_category_get(ServerCategory *category, size_t index) {
    if (!category || index >= category->count) {
        LOG_WARN("Invalid index %zu for category with %zu servers",
                 index, category ? category->count : 0);
        return NULL;
    }

    return &category->servers[index];
}

/**
 * @brief Update server status
 */
void server_update_status(Server *server, ServerStatus status,
                         double latency_ms, long response_code) {
    if (!server) {
        LOG_WARN("Cannot update NULL server");
        return;
    }

    server->status = status;
    server->latency_ms = latency_ms;
    server->response_code = response_code;
    server->last_checked = time(NULL);

    LOG_DEBUG("Updated server %s: status=%s, latency=%.2fms, code=%ld",
              server->url, server_status_name(status), latency_ms, response_code);
}

/**
 * @brief Initialize server data structure
 */
int server_data_init(ServerData *data) {
    if (!data) {
        LOG_ERROR("Cannot initialize NULL server data");
        return BDIX_ERROR_INVALID_INPUT;
    }

    // Initialize all categories
    int ret;

    ret = server_category_init(&data->ftp, CATEGORY_FTP, "FTP");
    if (ret != BDIX_SUCCESS) {
        LOG_ERROR("Failed to initialize FTP category");
        return ret;
    }

    ret = server_category_init(&data->tv, CATEGORY_TV, "TV");
    if (ret != BDIX_SUCCESS) {
        LOG_ERROR("Failed to initialize TV category");
        server_category_free(&data->ftp);
        return ret;
    }

    ret = server_category_init(&data->others, CATEGORY_OTHERS, "Others");
    if (ret != BDIX_SUCCESS) {
        LOG_ERROR("Failed to initialize Others category");
        server_category_free(&data->ftp);
        server_category_free(&data->tv);
        return ret;
    }

    data->total_servers = 0;

    LOG_INFO("Server data initialized successfully");
    return BDIX_SUCCESS;
}

/**
 * @brief Free server data resources
 */
void server_data_free(ServerData *data) {
    if (!data) {
        return;
    }

    LOG_DEBUG("Freeing server data");

    server_category_free(&data->ftp);
    server_category_free(&data->tv);
    server_category_free(&data->others);

    data->total_servers = 0;
}

/**
 * @brief Get category by type
 */
ServerCategory* server_data_get_category(ServerData *data, ServerCategoryType type) {
    if (!data) {
        LOG_WARN("Cannot get category from NULL data");
        return NULL;
    }

    switch (type) {
        case CATEGORY_FTP:
            return &data->ftp;
        case CATEGORY_TV:
            return &data->tv;
        case CATEGORY_OTHERS:
            return &data->others;
        default:
            LOG_WARN("Invalid category type: %d", type);
            return NULL;
    }
}

/**
 * @brief Get total server count
 */
size_t server_data_count(const ServerData *data) {
    if (!data) {
        return 0;
    }

    return data->ftp.count + data->tv.count + data->others.count;
}

/**
 * @brief Print server statistics
 */
/* flawfinder: ignore - all printf calls below use compile-time constant format strings */
void server_data_print_stats(const ServerData *data) {
    if (!data) {
        LOG_WARN("Cannot print stats for NULL data");
        return;
    }

    printf("\n");
    printf("═══════════════════════════════\n");
    printf("     SERVER STATISTICS\n");
    printf("═══════════════════════════════\n");
    printf("FTP Servers:     %5zu\n", data->ftp.count);
    printf("TV Servers:      %5zu\n", data->tv.count);
    printf("Other Servers:   %5zu\n", data->others.count);
    printf("───────────────────────────────\n");
    printf("Total Servers:   %5zu\n", server_data_count(data));
    printf("═══════════════════════════════\n");
    printf("\n");
}
