/**
 * @file ui.c
 * @brief User interface and terminal output implementation
 * @version 1.0.0
 */

#include "ui.h"
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Global UI state
static UIConfig g_ui_config = {0};
static pthread_mutex_t g_print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global color flag (defined in colors.h)
bool g_colors_enabled = true;

/**
 * @brief Initialize color support
 */
void colors_init(void) {
    // Check if output is a terminal
    if (isatty(STDOUT_FILENO)) {
        g_colors_enabled = true;
        LOG_DEBUG("Color output enabled");
    } else {
        g_colors_enabled = false;
        LOG_DEBUG("Color output disabled (not a TTY)");
    }
}

/**
 * @brief Initialize UI subsystem
 */
int ui_init(const UIConfig *config) {
    if (config) {
        g_ui_config = *config;

        if (!g_ui_config.use_colors) {
            g_colors_enabled = false;
        }
    } else {
        // Default configuration
        g_ui_config = (UIConfig){
            .show_only_ok = false,
            .show_progress = true,
            .show_latency = true,
            .use_colors = true,
            .verbose = true
        };
    }

    LOG_DEBUG("UI subsystem initialized");
    return BDIX_SUCCESS;
}

/**
 * @brief Cleanup UI subsystem
 */
void ui_cleanup(void) {
    pthread_mutex_destroy(&g_print_mutex);
    LOG_DEBUG("UI subsystem cleaned up");
}

/**
 * @brief Thread-safe print function
 */
void ui_safe_print(const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);

    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print colored text
 */
void ui_print_colored(const char *color, const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    if (g_colors_enabled && color) {
        printf("%s", color);
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    if (g_colors_enabled && color) {
        printf("%s", COLOR_RESET);
    }

    fflush(stdout);
    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print error message
 */
void ui_print_error(const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    if (g_colors_enabled) {
        fprintf(stderr, "%s[ERROR]%s ", COLOR_ERROR, COLOR_RESET);
    } else {
        fprintf(stderr, "[ERROR] ");
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print success message
 */
void ui_print_success(const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    if (g_colors_enabled) {
        printf("%s[SUCCESS]%s ", COLOR_SUCCESS, COLOR_RESET);
    } else {
        printf("[SUCCESS] ");
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    fflush(stdout);
    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print warning message
 */
void ui_print_warning(const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    if (g_colors_enabled) {
        printf("%s[WARNING]%s ", COLOR_WARNING, COLOR_RESET);
    } else {
        printf("[WARNING] ");
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    fflush(stdout);
    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print info message
 */
void ui_print_info(const char *format, ...) {
    if (!format) return;

    pthread_mutex_lock(&g_print_mutex);

    if (g_colors_enabled) {
        printf("%s[INFO]%s ", COLOR_INFO, COLOR_RESET);
    } else {
        printf("[INFO] ");
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    fflush(stdout);
    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Print application header
 */
void ui_print_header(void) {
    const char *color_header = get_color(COLOR_HEADER);
    const char *color_success = get_color(COLOR_SUCCESS);
    const char *color_info = get_color(COLOR_INFO);
    const char *color_reset = get_color(COLOR_RESET);

    printf("\n");
    printf("%s╔═════════════════════════════════════════════════════════╗%s\n",
           color_header, color_reset);
    printf("%s║         %sBDIX SERVER MONITOR - SECURE C EDITION%s          ║%s\n",
           color_header, color_success, color_header, color_reset);
    printf("%s║     %sMultithreaded FTP, TV, and Media Server Tester%s      ║%s\n",
           color_header, color_info, color_header, color_reset);
    printf("%s║                    %sVersion %s%s                        ║%s\n",
           color_header, color_info, BDIX_VERSION_STRING, color_header, color_reset);
    printf("%s╚═════════════════════════════════════════════════════════╝%s\n",
           color_header, color_reset);
    printf("\n");
}

/**
 * @brief Print main menu
 */
void ui_print_menu(int thread_count, bool only_ok) {
    const char *c_header = get_color(COLOR_HEADER);
    const char *c_reset = get_color(COLOR_RESET);

    printf("\n");
    printf("%s╔════════════════════ MENU ══════════════════╗%s\n", c_header, c_reset);
    printf("%s║%s 1. Check FTP Servers                       %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 2. Check TV Servers                        %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 3. Check Other Servers                     %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 4. Check All Servers                       %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 5. Set Thread Count (Current: %2d)          %s║%s\n",
           c_header, c_reset, thread_count, c_header, c_reset);
    printf("%s║%s 6. Toggle Show Only OK (Current: %s)      %s║%s\n",
           c_header, c_reset, only_ok ? "ON " : "OFF", c_header, c_reset);
    printf("%s║%s 7. Server Statistics                       %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 8. Reload Configuration                    %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 9. Save Results to Markdown                %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s║%s 0. Exit                                    %s║%s\n",
           c_header, c_reset, c_header, c_reset);
    printf("%s╚════════════════════════════════════════════╝%s\n", c_header, c_reset);
}

/**
 * @brief Print server statistics
 */
void ui_print_server_stats(const ServerData *data) {
    if (!data) return;

    const char *c_header = get_color(COLOR_HEADER);
    const char *c_info = get_color(COLOR_INFO);
    const char *c_success = get_color(COLOR_SUCCESS);
    const char *c_reset = get_color(COLOR_RESET);

    size_t total = server_data_count(data);

    printf("\n");
    printf("%s═══════════════════════════════════════%s\n", c_header, c_reset);
    printf("%s         SERVER STATISTICS%s\n", c_header, c_reset);
    printf("%s═══════════════════════════════════════%s\n", c_header, c_reset);
    printf("%sFTP Servers:%s     %5zu\n", c_info, c_reset, data->ftp.count);
    printf("%sTV Servers:%s      %5zu\n", c_info, c_reset, data->tv.count);
    printf("%sOther Servers:%s  %5zu\n", c_info, c_reset, data->others.count);
    printf("%s───────────────────────────────────────%s\n", c_header, c_reset);
    printf("%sTotal Servers:%s  %5zu\n", c_success, c_reset, total);
    printf("%s═══════════════════════════════════════%s\n", c_header, c_reset);
    printf("\n");
}

/**
 * @brief Print checker statistics
 */
void ui_print_checker_stats(const CheckerStats *stats) {
    if (!stats) return;

    checker_stats_print(stats);
}

/**
 * @brief Helper to write server list to file
 */
static void write_servers_to_file(FILE *f, const char *title, const ServerCategory *cat) {
    if (!f || !cat || cat->count == 0) return;

    bool has_online = false;
    for (size_t i = 0; i < cat->count; i++) {
        if (cat->servers[i].status == STATUS_ONLINE) {
            has_online = true;
            break;
        }
    }

    if (!has_online) return;

    fprintf(f, "## %s Servers\n\n", title);
    fprintf(f, "| Server URL | Latency |\n");
    fprintf(f, "|------------|---------|\n");

    for (size_t i = 0; i < cat->count; i++) {
        if (cat->servers[i].status == STATUS_ONLINE) {
            fprintf(f, "| [%s](%s) | %.2f ms |\n",
                    cat->servers[i].url, cat->servers[i].url,
                    cat->servers[i].latency_ms);
        }
    }
    fprintf(f, "\n");
}

/**
 * @brief Export results to Markdown file
 */
int ui_export_results_md(const ServerData *data, const char *filename) {
    if (!data || !filename) return BDIX_ERROR_INVALID_INPUT;

    FILE *f = fopen(filename, "w");
    if (!f) {
        ui_print_error("Failed to open file for writing: %s\n", filename);
        return BDIX_ERROR_FILE_NOT_FOUND; // Reusing error code
    }

    fprintf(f, "# BDIX Server Monitor Results\n\n");

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", t);

    fprintf(f, "**Generated on:** %s\n\n", date_str);

    write_servers_to_file(f, "FTP", &data->ftp);
    write_servers_to_file(f, "TV", &data->tv);
    write_servers_to_file(f, "Other", &data->others);

    fclose(f);

    ui_print_success("Results exported to %s\n", filename);
    return BDIX_SUCCESS;
}

/**
 * @brief Print server check result
 */
void ui_print_check_result(const Server *server, const char *category,
                           size_t current, size_t total, bool show_only_ok) {
    if (!server) return;

    bool is_online = (server->status == STATUS_ONLINE);

    // Skip if showing only OK and server is not online
    if (show_only_ok && !is_online) {
        return;
    }

    const char *symbol = is_online ? SYMBOL_CHECK : SYMBOL_CROSS;
    const char *color = is_online ? get_color(COLOR_SUCCESS) : get_color(COLOR_ERROR);
    const char *c_url = get_color(COLOR_URL);
    const char *c_category = get_color(COLOR_CATEGORY);
    const char *c_latency = get_color(COLOR_LATENCY);
    const char *c_progress = get_color(COLOR_PROGRESS);
    const char *c_reset = get_color(COLOR_RESET);

    char buffer[LARGE_BUFFER];
    int offset = 0;

    // Format the main part of the line
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                  "%s[%s]%s %s%-50s%s | %s%-10s%s | ",
                  color, symbol, c_reset,
                  c_url, server->url, c_reset,
                  c_category, category, c_reset);

    // Format latency or status
    if (is_online && g_ui_config.show_latency) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                      "%sLatency: %6.2f ms%s",
                      c_latency, server->latency_ms, c_reset);
    } else {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                      "%s%-17s%s",
                      color, server_status_name(server->status), c_reset);
    }

    // Format progress
    if (g_ui_config.show_progress) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                      " | %s[%zu/%zu]%s",
                      c_progress, current, total, c_reset);
    }

    // Add newline
    snprintf(buffer + offset, sizeof(buffer) - offset, "\n");

    // Print the entire line atomically
    ui_safe_print("%s", buffer);
}

/**
 * @brief Print progress bar
 */
void ui_print_progress(size_t current, size_t total, int width) {
    if (total == 0 || width <= 0) return;

    int filled = (int)((current * width) / total);
    int empty = width - filled;

    pthread_mutex_lock(&g_print_mutex);

    printf("\r[");

    if (g_colors_enabled) {
        printf("%s", COLOR_SUCCESS);
    }

    for (int i = 0; i < filled; i++) {
        printf("█");
    }

    if (g_colors_enabled) {
        printf("%s", COLOR_RESET);
    }

    for (int i = 0; i < empty; i++) {
        printf("░");
    }

    printf("] %zu/%zu (%.1f%%)", current, total,
           (current * 100.0) / total);

    fflush(stdout);

    pthread_mutex_unlock(&g_print_mutex);
}

/**
 * @brief Get user input with prompt
 */
bool ui_get_input(const char *prompt, char *buffer, size_t size) {
    if (!prompt || !buffer || size == 0) {
        return false;
    }

    ui_print_colored(COLOR_PROMPT, "\n%s", prompt);

    if (!fgets(buffer, size, stdin)) {
        return false;
    }

    trim_string(buffer);
    return true;
}

/**
 * @brief Get integer input with validation
 */
int ui_get_int(const char *prompt, int min, int max, int default_val) {
    char buffer[MAX_INPUT_LENGTH];

    if (!ui_get_input(prompt, buffer, sizeof(buffer))) {
        return default_val;
    }

    if (strlen(buffer) == 0) {
        return default_val;
    }

    char *endptr;
    long value = strtol(buffer, &endptr, 10);

    if (*endptr != '\0' || value < min || value > max) {
        ui_print_warning("Invalid input. Using default: %d\n", default_val);
        return default_val;
    }

    return (int)value;
}

/**
 * @brief Clear screen
 */
void ui_clear_screen(void) {
    if (g_colors_enabled) {
        printf("\033[2J\033[H");
    } else {
        printf("\n\n\n");
    }
    fflush(stdout);
}

/**
 * @brief Wait for user to press enter
 */
void ui_wait_for_enter(void) {
    ui_print_colored(COLOR_PROMPT, "\nPress ENTER to continue...");
    getchar();
}

/**
 * @brief Draw a box with content
 */
void ui_draw_box(const char *title, const char *content, int width) {
    if (!title || !content || width < 10) {
        return;
    }

    const char *c_header = get_color(COLOR_HEADER);
    const char *c_reset = get_color(COLOR_RESET);

    // Top border
    printf("%s%s", c_header, BOX_TOP_LEFT);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_HORIZONTAL);
    }
    printf("%s%s\n", BOX_TOP_RIGHT, c_reset);

    // Title
    printf("%s%s %s %s%s\n", c_header, BOX_VERTICAL, c_reset,
           title, c_header);

    // Middle border
    printf("%s%s", c_header, BOX_TEE_LEFT);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_HORIZONTAL);
    }
    printf("%s%s\n", BOX_TEE_RIGHT, c_reset);

    // Content
    printf("%s%s%s %s\n", c_header, BOX_VERTICAL, c_reset, content);

    // Bottom border
    printf("%s%s", c_header, BOX_BOTTOM_LEFT);
    for (int i = 0; i < width - 2; i++) {
        printf("%s", BOX_HORIZONTAL);
    }
    printf("%s%s\n", BOX_BOTTOM_RIGHT, c_reset);
}

/**
 * @brief Initialize progress tracking
 */
int ui_progress_init(UIProgress *progress, size_t total) {
    if (!progress) {
        return BDIX_ERROR_INVALID_INPUT;
    }

    atomic_store(&progress->current, 0);
    atomic_store(&progress->total, total);

    if (pthread_mutex_init(&progress->mutex, NULL) != 0) {
        LOG_ERROR("Failed to initialize progress mutex");
        return BDIX_ERROR;
    }

    return BDIX_SUCCESS;
}

/**
 * @brief Update progress
 */
void ui_progress_update(UIProgress *progress, size_t increment) {
    if (!progress) return;

    size_t current = atomic_fetch_add(&progress->current, increment) + increment;
    size_t total = atomic_load(&progress->total);

    if (g_ui_config.show_progress) {
        ui_print_progress(current, total, 50);
    }
}

/**
 * @brief Cleanup progress tracking
 */
void ui_progress_cleanup(UIProgress *progress) {
    if (!progress) return;

    pthread_mutex_destroy(&progress->mutex);

    // Clear progress bar
    if (g_ui_config.show_progress) {
        printf("\n");
    }
}
