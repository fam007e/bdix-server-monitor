# 📚 BDIX Server Monitor - API Reference

**Version:** 1.0.0
**Language:** C17
**Last Updated:** December 2025

---

## Table of Contents

1. [Overview](#overview)
2. [Common Utilities](#common-utilities)
3. [Server Management API](#server-management-api)
4. [Checker API](#checker-api)
5. [Configuration API](#configuration-api)
6. [User Interface API](#user-interface-api)
7. [Thread Pool API](#thread-pool-api)
8. [Colors and Formatting](#colors-and-formatting)
9. [Error Codes](#error-codes)
10. [Usage Examples](#usage-examples)

---

## Overview

The BDIX Server Monitor provides a modular C API for checking server availability, managing server lists, and displaying results with a beautiful terminal UI. All APIs are thread-safe and memory-safe.

### Design Principles

- **Safety First:** All functions validate inputs and check for errors
- **Thread Safety:** Concurrent operations are properly synchronized
- **Memory Management:** No memory leaks, all resources properly freed
- **Error Handling:** Comprehensive error codes and logging
- **Performance:** Optimized for high-throughput server checking

---

## Common Utilities

**Header:** `include/common.h`

### Constants

```c
#define MAX_URL_LENGTH 512
#define MAX_SERVERS_PER_CATEGORY 2000
#define MIN_THREADS 1
#define MAX_THREADS 64
#define DEFAULT_THREADS 15
#define HTTP_TIMEOUT_SECONDS 10
```

### Error Codes

```c
#define BDIX_SUCCESS 0
#define BDIX_ERROR -1
#define BDIX_ERROR_MEMORY -2
#define BDIX_ERROR_INVALID_INPUT -3
#define BDIX_ERROR_FILE_NOT_FOUND -4
#define BDIX_ERROR_JSON_PARSE -5
#define BDIX_ERROR_NETWORK -6
#define BDIX_ERROR_THREAD -7
```

### Utility Macros

```c
#define UNUSED(x) (void)(x)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// RAII-like cleanup
#define defer(code) \
    for (int _once = 1; _once; _once = 0, code)
```

### Safe Memory Functions

#### `safe_malloc()`
```c
void* safe_malloc(size_t size);
```
**Description:** Allocate memory with automatic error checking.
**Parameters:**
- `size` - Number of bytes to allocate
**Returns:** Pointer to allocated memory (never NULL, exits on failure)
**Example:**
```c
char *buffer = safe_malloc(1024);
defer({ free(buffer); });
```

#### `safe_calloc()`
```c
void* safe_calloc(size_t nmemb, size_t size);
```
**Description:** Allocate zero-initialized memory.
**Parameters:**
- `nmemb` - Number of elements
- `size` - Size of each element
**Returns:** Pointer to allocated memory

#### `safe_strdup()`
```c
char* safe_strdup(const char *s);
```
**Description:** Duplicate a string safely.
**Parameters:**
- `s` - Source string
**Returns:** Newly allocated string copy

### Timing Functions

#### `get_time_ms()`
```c
double get_time_ms(void);
```
**Description:** Get current time in milliseconds.
**Returns:** Time in milliseconds since arbitrary epoch
**Example:**
```c
double start = get_time_ms();
// ... operation ...
double elapsed = get_time_ms() - start;
printf("Took %.2f ms\n", elapsed);
```

---

## Server Management API

**Header:** `include/server.h`

### Data Structures

#### `ServerStatus`
```c
typedef enum {
    STATUS_UNKNOWN,
    STATUS_ONLINE,
    STATUS_OFFLINE,
    STATUS_TIMEOUT,
    STATUS_ERROR
} ServerStatus;
```

#### `ServerCategoryType`
```c
typedef enum {
    CATEGORY_FTP,
    CATEGORY_TV,
    CATEGORY_OTHERS,
    CATEGORY_COUNT
} ServerCategoryType;
```

#### `Server`
```c
typedef struct {
    char url[MAX_URL_LENGTH];
    ServerStatus status;
    double latency_ms;
    long response_code;
    time_t last_checked;
} Server;
```

#### `ServerCategory`
```c
typedef struct {
    char **urls;                    // Array of server URLs
    Server *servers;                // Array of server information
    size_t count;                   // Current number of servers
    size_t capacity;                // Allocated capacity
    ServerCategoryType type;        // Category type
    const char *name;               // Category name
} ServerCategory;
```

#### `ServerData`
```c
typedef struct {
    ServerCategory ftp;
    ServerCategory tv;
    ServerCategory others;
    size_t total_servers;
} ServerData;
```

### Functions

#### `server_category_init()`
```c
int server_category_init(ServerCategory *category,
                        ServerCategoryType type,
                        const char *name);
```
**Description:** Initialize a server category.
**Parameters:**
- `category` - Pointer to category structure
- `type` - Category type enum
- `name` - Category name string
**Returns:** `BDIX_SUCCESS` or error code
**Thread Safety:** Not thread-safe
**Example:**
```c
ServerCategory ftp_servers;
if (server_category_init(&ftp_servers, CATEGORY_FTP, "FTP") != BDIX_SUCCESS) {
    fprintf(stderr, "Failed to initialize category\n");
    return -1;
}
defer({ server_category_free(&ftp_servers); });
```

#### `server_category_free()`
```c
void server_category_free(ServerCategory *category);
```
**Description:** Free all resources associated with a category.
**Parameters:**
- `category` - Pointer to category structure
**Thread Safety:** Not thread-safe

#### `server_category_add()`
```c
int server_category_add(ServerCategory *category, const char *url);
```
**Description:** Add a server URL to a category.
**Parameters:**
- `category` - Pointer to category
- `url` - Server URL (must start with http:// or https://)
**Returns:** `BDIX_SUCCESS` or error code
**Thread Safety:** Not thread-safe
**Validation:**
- URL length must be < `MAX_URL_LENGTH`
- URL must be valid format
- Category must not exceed `MAX_SERVERS_PER_CATEGORY`
**Example:**
```c
if (server_category_add(&ftp_servers, "http://ftp.example.com") != BDIX_SUCCESS) {
    fprintf(stderr, "Failed to add server\n");
}
```

#### `server_category_get()`
```c
Server* server_category_get(ServerCategory *category, size_t index);
```
**Description:** Get server at specific index.
**Parameters:**
- `category` - Pointer to category
- `index` - Zero-based index
**Returns:** Pointer to server or NULL if invalid
**Thread Safety:** Read-only safe after initialization

#### `server_update_status()`
```c
void server_update_status(Server *server, ServerStatus status,
                         double latency_ms, long response_code);
```
**Description:** Update server status after checking.
**Parameters:**
- `server` - Pointer to server
- `status` - New status
- `latency_ms` - Measured latency
- `response_code` - HTTP response code
**Thread Safety:** Not thread-safe per server

#### `server_data_init()`
```c
int server_data_init(ServerData *data);
```
**Description:** Initialize complete server data structure.
**Parameters:**
- `data` - Pointer to server data
**Returns:** `BDIX_SUCCESS` or error code
**Example:**
```c
ServerData data;
if (server_data_init(&data) != BDIX_SUCCESS) {
    return -1;
}
defer({ server_data_free(&data); });
```

#### `server_data_free()`
```c
void server_data_free(ServerData *data);
```
**Description:** Free all server data resources.
**Parameters:**
- `data` - Pointer to server data

#### `server_data_count()`
```c
size_t server_data_count(const ServerData *data);
```
**Description:** Get total number of servers across all categories.
**Parameters:**
- `data` - Pointer to server data
**Returns:** Total server count

#### `server_data_print_stats()`
```c
void server_data_print_stats(const ServerData *data);
```
**Description:** Print formatted statistics about loaded servers.
**Parameters:**
- `data` - Pointer to server data

---

## Checker API

**Header:** `include/checker.h`

### Data Structures

#### `CheckerConfig`
```c
typedef struct {
    int timeout_seconds;            // HTTP timeout
    int connect_timeout_seconds;    // Connection timeout
    bool follow_redirects;          // Follow HTTP redirects
    int max_redirects;              // Maximum number of redirects
    bool verify_ssl;                // Verify SSL certificates
    bool verbose;                   // Verbose output
} CheckerConfig;
```

#### `CheckerStats`
```c
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
```

### Functions

#### `checker_init()`
```c
int checker_init(void);
```
**Description:** Initialize the checker subsystem (must be called before use).
**Returns:** `BDIX_SUCCESS` or error code
**Thread Safety:** Must be called from main thread before any checking
**Example:**
```c
if (checker_init() != BDIX_SUCCESS) {
    fprintf(stderr, "Failed to initialize checker\n");
    return -1;
}
defer({ checker_cleanup(); });
```

#### `checker_cleanup()`
```c
void checker_cleanup(void);
```
**Description:** Cleanup checker subsystem resources.

#### `checker_get_default_config()`
```c
CheckerConfig checker_get_default_config(void);
```
**Description:** Get default checker configuration.
**Returns:** Default configuration structure
**Example:**
```c
CheckerConfig config = checker_get_default_config();
config.timeout_seconds = 5;  // Customize
```

#### `checker_check_server()`
```c
int checker_check_server(Server *server, const CheckerConfig *config);
```
**Description:** Check a single server's availability.
**Parameters:**
- `server` - Pointer to server structure
- `config` - Pointer to checker configuration
**Returns:** `BDIX_SUCCESS` or error code
**Side Effects:** Updates `server->status`, `server->latency_ms`, etc.
**Thread Safety:** Thread-safe (uses separate CURL handle)
**Example:**
```c
Server server;
strcpy(server.url, "http://example.com");
CheckerConfig config = checker_get_default_config();

if (checker_check_server(&server, &config) == BDIX_SUCCESS) {
    printf("Status: %s, Latency: %.2f ms\n",
           server_status_name(server.status),
           server.latency_ms);
}
```

#### `checker_check_category()`
```c
int checker_check_category(ServerCategory *category,
                          const CheckerConfig *config,
                          int thread_count,
                          CheckerStats *stats);
```
**Description:** Check all servers in a category using thread pool.
**Parameters:**
- `category` - Pointer to server category
- `config` - Checker configuration
- `thread_count` - Number of threads to use (1-64)
- `stats` - Optional statistics structure (can be NULL)
**Returns:** `BDIX_SUCCESS` or error code
**Thread Safety:** Thread-safe
**Example:**
```c
CheckerStats stats;
checker_stats_init(&stats);

checker_check_category(&data.ftp, &config, 16, &stats);
checker_stats_print(&stats);
```

#### `checker_check_multiple()`
```c
int checker_check_multiple(ServerData *data,
                          const CheckerConfig *config,
                          int thread_count,
                          bool check_ftp,
                          bool check_tv,
                          bool check_others,
                          CheckerStats *stats);
```
**Description:** Check multiple categories.
**Parameters:**
- `data` - Pointer to server data
- `config` - Checker configuration
- `thread_count` - Number of threads
- `check_ftp` - Check FTP category
- `check_tv` - Check TV category
- `check_others` - Check others category
- `stats` - Optional statistics
**Returns:** `BDIX_SUCCESS` or error code

#### `checker_stats_init()`
```c
void checker_stats_init(CheckerStats *stats);
```
**Description:** Initialize statistics structure.
**Parameters:**
- `stats` - Pointer to statistics structure

#### `checker_stats_update()`
```c
void checker_stats_update(CheckerStats *stats, const Server *server);
```
**Description:** Update statistics with server result.
**Parameters:**
- `stats` - Pointer to statistics
- `server` - Pointer to checked server
**Thread Safety:** Thread-safe (uses atomics)

#### `checker_stats_print()`
```c
void checker_stats_print(const CheckerStats *stats);
```
**Description:** Print formatted statistics summary.
**Parameters:**
- `stats` - Pointer to statistics

---

## Configuration API

**Header:** `include/config.h`

### Functions

#### `config_load_from_file()`
```c
int config_load_from_file(const char *filename, ServerData *data);
```
**Description:** Load server configuration from JSON file.
**Parameters:**
- `filename` - Path to JSON file
- `data` - Pointer to server data structure
**Returns:** `BDIX_SUCCESS` or error code
**JSON Format:**
```json
{
  "ftp": ["http://server1.com", "http://server2.com"],
  "tv": ["http://tv1.com"],
  "others": ["http://other1.com"]
}
```
**Example:**
```c
ServerData data;
server_data_init(&data);

if (config_load_from_file("servers.json", &data) != BDIX_SUCCESS) {
    fprintf(stderr, "Failed to load config\n");
    return -1;
}
```

#### `config_create_sample()`
```c
int config_create_sample(const char *filename);
```
**Description:** Create sample configuration file.
**Parameters:**
- `filename` - Path where to create file
**Returns:** `BDIX_SUCCESS` or error code

#### `config_validate_file()`
```c
bool config_validate_file(const char *filename);
```
**Description:** Validate configuration file format.
**Parameters:**
- `filename` - Path to JSON file
**Returns:** `true` if valid, `false` otherwise

---

## User Interface API

**Header:** `include/ui.h`

### Functions

#### `ui_init()`
```c
int ui_init(const UIConfig *config);
```
**Description:** Initialize UI subsystem.
**Parameters:**
- `config` - UI configuration (can be NULL for defaults)
**Returns:** `BDIX_SUCCESS`

#### `ui_print_header()`
```c
void ui_print_header(void);
```
**Description:** Print application header banner.

#### `ui_print_menu()`
```c
void ui_print_menu(int thread_count, bool only_ok);
```
**Description:** Print interactive menu.
**Parameters:**
- `thread_count` - Current thread count setting
- `only_ok` - Current "only OK" filter setting

#### `ui_print_check_result()`
```c
void ui_print_check_result(const Server *server,
                          const char *category,
                          size_t current,
                          size_t total,
                          bool show_only_ok);
```
**Description:** Print individual server check result.
**Parameters:**
- `server` - Checked server
- `category` - Category name
- `current` - Current item number
- `total` - Total items
- `show_only_ok` - Filter setting
**Thread Safety:** Thread-safe


#### `ui_export_results_md()`
```c
int ui_export_results_md(const ServerData *data, const char *filename);
```
**Description:** Export online servers to a Markdown file.
**Parameters:**
- `data` - Pointer to server data
- `filename` - Output filename
**Returns:** `BDIX_SUCCESS` or error code

#### `ui_safe_print()`
```c
void ui_safe_print(const char *format, ...);
```
**Description:** Thread-safe formatted print.
**Parameters:**
- `format` - Printf-style format string
- `...` - Variable arguments
**Thread Safety:** Thread-safe (uses mutex)

---

## Thread Pool API

**Header:** `include/thread_pool.h`

### Data Structures

#### `ThreadPool`
```c
typedef struct ThreadPool ThreadPool;  // Opaque type
```

#### `thread_pool_func_t`
```c
typedef void* (*thread_pool_func_t)(void *arg);
```

### Functions

#### `thread_pool_create()`
```c
ThreadPool* thread_pool_create(size_t thread_count);
```
**Description:** Create thread pool with worker threads.
**Parameters:**
- `thread_count` - Number of worker threads (1-64)
**Returns:** Pointer to thread pool or NULL on error
**Example:**
```c
ThreadPool *pool = thread_pool_create(16);
if (!pool) {
    fprintf(stderr, "Failed to create thread pool\n");
    return -1;
}
defer({ thread_pool_destroy(pool); });
```

#### `thread_pool_add_work()`
```c
int thread_pool_add_work(ThreadPool *pool,
                        thread_pool_func_t function,
                        void *arg);
```
**Description:** Add work item to pool.
**Parameters:**
- `pool` - Thread pool
- `function` - Worker function
- `arg` - Argument to pass to function
**Returns:** `BDIX_SUCCESS` or error code
**Thread Safety:** Thread-safe

#### `thread_pool_wait()`
```c
int thread_pool_wait(ThreadPool *pool);
```
**Description:** Wait for all queued work to complete.
**Parameters:**
- `pool` - Thread pool
**Returns:** `BDIX_SUCCESS`
**Blocks until:** All work is done

#### `thread_pool_destroy()`
```c
void thread_pool_destroy(ThreadPool *pool);
```
**Description:** Destroy thread pool and free resources.
**Parameters:**
- `pool` - Thread pool
**Blocks until:** All threads exit

---

## Colors and Formatting

**Header:** `include/colors.h`

### Color Codes

```c
#define COLOR_SUCCESS      "\033[92m"  // Bright green
#define COLOR_ERROR        "\033[91m"  // Bright red
#define COLOR_WARNING      "\033[93m"  // Bright yellow
#define COLOR_INFO         "\033[96m"  // Bright cyan
#define COLOR_HEADER       "\033[96;1m" // Bold cyan
```

### Functions

```c
void colors_init(void);         // Auto-detect terminal support
void colors_enable(void);       // Force enable
void colors_disable(void);      // Force disable
bool colors_are_enabled(void);  // Check status
```

---

## Usage Examples

### Complete Server Checking Example

```c
#include "common.h"
#include "server.h"
#include "checker.h"
#include "config.h"
#include "ui.h"

int main(void) {
    // Initialize subsystems
    checker_init();
    ui_init(NULL);
    defer({
        checker_cleanup();
        ui_cleanup();
    });

    // Load servers
    ServerData data;
    server_data_init(&data);
    defer({ server_data_free(&data); });

    if (config_load_from_file("servers.json", &data) != BDIX_SUCCESS) {
        fprintf(stderr, "Failed to load config\n");
        return 1;
    }

    // Check servers
    CheckerConfig config = checker_get_default_config();
    CheckerStats stats;
    checker_stats_init(&stats);

    checker_check_multiple(&data, &config, 16,
                          true, true, true, &stats);

    // Print results
    checker_stats_print(&stats);

    return 0;
}
```

---

## Thread Safety Summary

| API Module | Thread Safety |
|-----------|---------------|
| Server Management | Not thread-safe (initialize before threads) |
| Checker | Thread-safe (each thread uses own CURL handle) |
| Configuration | Not thread-safe (load before threads) |
| UI | Thread-safe (uses mutexes) |
| Thread Pool | Thread-safe |
| Statistics | Thread-safe (uses atomics) |

---

## Memory Management Guidelines

1. Always use `defer()` macro for cleanup
2. Use `safe_*` functions for allocation
3. Free in reverse order of allocation
4. Call cleanup functions on exit paths
5. Use Valgrind to verify no leaks

---

## Error Handling Best Practices

```c
// Always check return values
if (server_category_add(&cat, url) != BDIX_SUCCESS) {
    LOG_ERROR("Failed to add server: %s", url);
    return BDIX_ERROR;
}

// Use defer for cleanup
Server *srv = safe_malloc(sizeof(Server));
defer({ free(srv); });

// Log errors appropriately
if (!config_validate_file(filename)) {
    LOG_WARN("Invalid config file: %s", filename);
}
```

---

**End of API Reference**