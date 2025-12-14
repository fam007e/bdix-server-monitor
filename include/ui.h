/**
 * @file ui.h
 * @brief User interface and terminal output functions
 * @version 1.0.0
 */

#ifndef BDIX_UI_H
#define BDIX_UI_H

#include "common.h"
#include "colors.h"
#include "server.h"
#include "checker.h"
#include <pthread.h>

/**
 * @brief UI configuration
 */
typedef struct {
    bool show_only_ok;              // Show only successful checks
    bool show_progress;             // Show progress indicators
    bool show_latency;              // Show latency information
    bool use_colors;                // Use colored output
    bool verbose;                   // Verbose output mode
} UIConfig;

/**
 * @brief Progress information
 */
typedef struct {
    _Atomic size_t current;
    _Atomic size_t total;
    pthread_mutex_t mutex;
} UIProgress;

/**
 * @brief Initialize UI subsystem
 *
 * @param config UI configuration
 * @return BDIX_SUCCESS on success
 */
int ui_init(const UIConfig *config);

/**
 * @brief Cleanup UI subsystem
 */
void ui_cleanup(void);

/**
 * @brief Print application header/banner
 */
void ui_print_header(void);

/**
 * @brief Print main menu
 *
 * @param thread_count Current thread count
 * @param only_ok Current "only OK" setting
 */
void ui_print_menu(int thread_count, bool only_ok);

/**
 * @brief Print server statistics
 *
 * @param data Pointer to server data
 */
void ui_print_server_stats(const ServerData *data);

/**
 * @brief Print checker statistics
 *
 * @param stats Pointer to checker statistics
 */
void ui_print_checker_stats(const CheckerStats *stats);

/**
 * @brief Export results to Markdown file
 *
 * @param data Pointer to server data
 * @param filename Output filename
 * @return BDIX_SUCCESS on success
 */
int ui_export_results_md(const ServerData *data, const char *filename);

/**
 * @brief Print server check result
 *
 * @param server Pointer to server
 * @param category Category name
 * @param current Current item number
 * @param total Total items
 * @param show_only_ok Only show if OK
 */
void ui_print_check_result(const Server *server, const char *category,
                           size_t current, size_t total, bool show_only_ok);

/**
 * @brief Print progress bar
 *
 * @param current Current progress
 * @param total Total items
 * @param width Bar width in characters
 */
void ui_print_progress(size_t current, size_t total, int width);

/**
 * @brief Print colored text
 *
 * @param color Color code
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_print_colored(const char *color, const char *format, ...);

/**
 * @brief Print error message
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_print_error(const char *format, ...);

/**
 * @brief Print success message
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_print_success(const char *format, ...);

/**
 * @brief Print warning message
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_print_warning(const char *format, ...);

/**
 * @brief Print info message
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_print_info(const char *format, ...);

/**
 * @brief Draw a box with content
 *
 * @param title Box title
 * @param content Box content
 * @param width Box width
 */
void ui_draw_box(const char *title, const char *content, int width);

/**
 * @brief Get user input with prompt
 *
 * @param prompt Prompt to display
 * @param buffer Buffer to store input
 * @param size Buffer size
 * @return true if input received, false on error
 */
bool ui_get_input(const char *prompt, char *buffer, size_t size);

/**
 * @brief Get integer input with validation
 *
 * @param prompt Prompt to display
 * @param min Minimum valid value
 * @param max Maximum valid value
 * @param default_val Default value
 * @return User input or default value
 */
int ui_get_int(const char *prompt, int min, int max, int default_val);

/**
 * @brief Clear screen
 */
void ui_clear_screen(void);

/**
 * @brief Wait for user to press enter
 */
void ui_wait_for_enter(void);

/**
 * @brief Initialize progress tracking
 *
 * @param progress Pointer to progress structure
 * @param total Total items
 * @return BDIX_SUCCESS on success
 */
int ui_progress_init(UIProgress *progress, size_t total);

/**
 * @brief Update progress
 *
 * @param progress Pointer to progress structure
 * @param increment Increment amount
 */
void ui_progress_update(UIProgress *progress, size_t increment);

/**
 * @brief Cleanup progress tracking
 *
 * @param progress Pointer to progress structure
 */
void ui_progress_cleanup(UIProgress *progress);

/**
 * @brief Thread-safe print function
 *
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void ui_safe_print(const char *format, ...);

#endif // BDIX_UI_H
