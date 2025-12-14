/**
 * @file colors.h
 * @brief ANSI color codes and text formatting for terminal output
 * @version 1.0.0
 */

#ifndef BDIX_COLORS_H
#define BDIX_COLORS_H

#include <stdbool.h>

// Global flag to enable/disable colors
extern bool g_colors_enabled;

// ANSI color codes
#define ANSI_RESET         "\033[0m"
#define ANSI_BOLD          "\033[1m"
#define ANSI_DIM           "\033[2m"
#define ANSI_ITALIC        "\033[3m"
#define ANSI_UNDERLINE     "\033[4m"
#define ANSI_BLINK         "\033[5m"
#define ANSI_REVERSE       "\033[7m"
#define ANSI_HIDDEN        "\033[8m"

// Regular colors
#define ANSI_BLACK         "\033[30m"
#define ANSI_RED           "\033[31m"
#define ANSI_GREEN         "\033[32m"
#define ANSI_YELLOW        "\033[33m"
#define ANSI_BLUE          "\033[34m"
#define ANSI_MAGENTA       "\033[35m"
#define ANSI_CYAN          "\033[36m"
#define ANSI_WHITE         "\033[37m"

// Bright colors
#define ANSI_BRIGHT_BLACK   "\033[90m"
#define ANSI_BRIGHT_RED     "\033[91m"
#define ANSI_BRIGHT_GREEN   "\033[92m"
#define ANSI_BRIGHT_YELLOW  "\033[93m"
#define ANSI_BRIGHT_BLUE    "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN    "\033[96m"
#define ANSI_BRIGHT_WHITE   "\033[97m"

// Background colors
#define ANSI_BG_BLACK      "\033[40m"
#define ANSI_BG_RED        "\033[41m"
#define ANSI_BG_GREEN      "\033[42m"
#define ANSI_BG_YELLOW     "\033[43m"
#define ANSI_BG_BLUE       "\033[44m"
#define ANSI_BG_MAGENTA    "\033[45m"
#define ANSI_BG_CYAN       "\033[46m"
#define ANSI_BG_WHITE      "\033[47m"

// Semantic color aliases
#define COLOR_RESET        ANSI_RESET
#define COLOR_ERROR        ANSI_BRIGHT_RED
#define COLOR_SUCCESS      ANSI_BRIGHT_GREEN
#define COLOR_WARNING      ANSI_BRIGHT_YELLOW
#define COLOR_INFO         ANSI_BRIGHT_CYAN
#define COLOR_DEBUG        ANSI_DIM
#define COLOR_HEADER       ANSI_BRIGHT_CYAN ANSI_BOLD
#define COLOR_PROMPT       ANSI_BRIGHT_YELLOW
#define COLOR_URL          ANSI_CYAN
#define COLOR_CATEGORY     ANSI_YELLOW
#define COLOR_LATENCY      ANSI_MAGENTA
#define COLOR_PROGRESS     ANSI_BLUE
#define COLOR_HIGHLIGHT    ANSI_BOLD ANSI_WHITE

// Box drawing characters
#define BOX_HORIZONTAL     "═"
#define BOX_VERTICAL       "║"
#define BOX_TOP_LEFT       "╔"
#define BOX_TOP_RIGHT      "╗"
#define BOX_BOTTOM_LEFT    "╚"
#define BOX_BOTTOM_RIGHT   "╝"
#define BOX_TEE_LEFT       "╠"
#define BOX_TEE_RIGHT      "╣"
#define BOX_TEE_TOP        "╦"
#define BOX_TEE_BOTTOM     "╩"
#define BOX_CROSS          "╬"

// Unicode symbols
#define SYMBOL_CHECK       "✓"
#define SYMBOL_CROSS       "✗"
#define SYMBOL_ARROW_RIGHT "→"
#define SYMBOL_ARROW_LEFT  "←"
#define SYMBOL_BULLET      "•"
#define SYMBOL_STAR        "★"
#define SYMBOL_LOADING     "⣾⣽⣻⢿⡿⣟⣯⣷"

/**
 * @brief Get color code string (empty if colors disabled)
 * @param color The ANSI color code
 * @return Color string or empty string
 */
static inline const char* get_color(const char *color) {
    return g_colors_enabled ? color : "";
}

/**
 * @brief Initialize color support (check if terminal supports colors)
 */
void colors_init(void);

/**
 * @brief Enable colored output
 */
static inline void colors_enable(void) {
    g_colors_enabled = true;
}

/**
 * @brief Disable colored output
 */
static inline void colors_disable(void) {
    g_colors_enabled = false;
}

/**
 * @brief Check if colors are enabled
 * @return true if colors are enabled
 */
static inline bool colors_are_enabled(void) {
    return g_colors_enabled;
}

#endif // BDIX_COLORS_H
