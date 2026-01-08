/**
 * @file common.h
 * @brief Common definitions, macros, and includes for BDIX Server Monitor
 * @version 1.0.0
 */

#ifndef BDIX_COMMON_H
#define BDIX_COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

// Version information
#define BDIX_VERSION_MAJOR 1
#define BDIX_VERSION_MINOR 0
#define BDIX_VERSION_PATCH 0
#define BDIX_VERSION_STRING "1.0.0"

// Security and performance constants
#define MAX_URL_LENGTH 512
#define MAX_SERVERS_PER_CATEGORY 2000
#define MIN_THREADS 1
#define MAX_THREADS 64
#define DEFAULT_THREADS 15
#define HTTP_TIMEOUT_SECONDS 10
#define HTTP_CONNECT_TIMEOUT 5
#define MAX_INPUT_LENGTH 256
#define MAX_PATH_LENGTH 1024

// Buffer sizes
#define SMALL_BUFFER 64
#define MEDIUM_BUFFER 256
#define LARGE_BUFFER 1024

// Return codes
#define BDIX_SUCCESS 0
#define BDIX_ERROR -1
#define BDIX_ERROR_MEMORY -2
#define BDIX_ERROR_INVALID_INPUT -3
#define BDIX_ERROR_FILE_NOT_FOUND -4
#define BDIX_ERROR_JSON_PARSE -5
#define BDIX_ERROR_NETWORK -6
#define BDIX_ERROR_THREAD -7

// Utility macros
#define UNUSED(x) (void)(x)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// RAII-like defer macro for automatic cleanup
#define DEFER_CONCAT_(a, b) a ## b
#define DEFER_CONCAT(a, b) DEFER_CONCAT_(a, b)
#define DEFER_VAR(line) DEFER_CONCAT(_defer_var_, line)

#define defer(code) \
    for (int DEFER_VAR(__LINE__) = 1; \
         DEFER_VAR(__LINE__); \
         DEFER_VAR(__LINE__) = 0, code)

// Logging macros with levels
/* flawfinder: ignore - all format strings in LOG_* macros are compile-time constants */
#ifdef DEBUG
    #define LOG_DEBUG(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__) // flawfinder: ignore
#else
    #define LOG_DEBUG(fmt, ...) do {} while(0)
#endif

#define LOG_INFO(fmt, ...) \
    fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__) // flawfinder: ignore

#define LOG_WARN(fmt, ...) \
    fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__) // flawfinder: ignore

#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__) // flawfinder: ignore

// Memory allocation wrappers with error checking
static inline void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr && size > 0) {
        LOG_ERROR("Memory allocation failed: %zu bytes", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void* safe_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (!ptr && nmemb > 0 && size > 0) {
        LOG_ERROR("Memory allocation failed: %zu elements of %zu bytes", nmemb, size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void* safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        LOG_ERROR("Memory reallocation failed: %zu bytes", size);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

// Safe string operations
static inline char* safe_strdup(const char *s) {
    if (!s) return NULL;
    char *dup = strdup(s);
    if (!dup) {
        LOG_ERROR("String duplication failed");
        exit(EXIT_FAILURE);
    }
    return dup;
}

static inline void safe_strncpy(char *dest, const char *src, size_t n) {
    if (!dest || !src || n == 0) return;
    strncpy(dest, src, n - 1); // flawfinder: ignore
    dest[n - 1] = '\0';
}

// Time utilities
static inline double get_time_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }
    return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
}

static inline void sleep_ms(long milliseconds) {
    struct timespec ts = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000L
    };
    nanosleep(&ts, NULL);
}

// String trimming utility
static inline void trim_string(char *str) {
    if (!str) return;

    // Trim leading whitespace
    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    // Trim trailing whitespace
    /* flawfinder: ignore - start is always within valid string bounds */
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    // Move trimmed string to beginning and null-terminate
    size_t len = end - start + 1;
    if (start != str) {
        memmove(str, start, len);
    }
    str[len] = '\0';
}

// URL validation
static inline bool is_valid_url(const char *url) {
    /* flawfinder: ignore - null checked before strlen, url is always null-terminated */
    if (!url || strlen(url) == 0 || strlen(url) >= MAX_URL_LENGTH) {
        return false;
    }

    // Basic check for http:// or https://
    if (strncmp(url, "http://", 7) != 0 &&
        strncmp(url, "https://", 8) != 0) {
        return false;
    }

    return true;
}

#endif // BDIX_COMMON_H
