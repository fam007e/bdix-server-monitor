/**
 * @file main.c
 * @brief Main entry point for BDIX Server Monitor
 * @version 1.0.0
 */

#include "common.h"
#include "colors.h"
#include "server.h"
#include "checker.h"
#include "config.h"
#include "ui.h"
#include <getopt.h>



/**
 * @brief Program options structure
 */
typedef struct {
    char config_file[MAX_PATH_LENGTH]; /* flawfinder: ignore - bounds checked with safe_strncpy */
    int thread_count;
    bool check_ftp;
    bool check_tv;
    bool check_others;
    bool check_all;
    bool only_ok;
    bool no_color;
    bool interactive;
    bool show_stats;
} ProgramOptions;

/**
 * @brief Print usage information
 */
/* flawfinder: ignore - all printf calls below use compile-time constant format strings */
static void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name); // flawfinder: ignore
    printf("BDIX Server Monitor - Check FTP, TV, and other BDIX servers\n\n"); // flawfinder: ignore
    printf("Options:\n"); // flawfinder: ignore
    printf("  -c, --config FILE      Configuration file (default: data/server.json)\n"); // flawfinder: ignore
    printf("  -t, --threads NUM      Number of threads (default: %d, range: %d-%d)\n", // flawfinder: ignore
           DEFAULT_THREADS, MIN_THREADS, MAX_THREADS);
    printf("  -f, --ftp              Check only FTP servers\n"); // flawfinder: ignore
    printf("  -v, --tv               Check only TV servers\n"); // flawfinder: ignore
    printf("  -o, --others           Check only other servers\n"); // flawfinder: ignore
    printf("  -a, --all              Check all servers (default)\n"); // flawfinder: ignore
    printf("  -q, --quiet            Show only successful checks\n"); // flawfinder: ignore
    printf("  -n, --no-color         Disable colored output\n"); // flawfinder: ignore
    printf("  -i, --interactive      Start in interactive mode (default)\n"); // flawfinder: ignore
    printf("  -s, --stats            Show statistics only\n"); // flawfinder: ignore
    printf("  -h, --help             Show this help message\n"); // flawfinder: ignore
    printf("  -V, --version          Show version information\n"); // flawfinder: ignore
    printf("\nExamples:\n"); // flawfinder: ignore
    printf("  %s                           # Interactive mode\n", program_name); // flawfinder: ignore
    printf("  %s --all --threads 32        # Check all with 32 threads\n", program_name); // flawfinder: ignore
    printf("  %s --ftp --quiet             # Check FTP, show only OK\n", program_name); // flawfinder: ignore
    printf("\n"); // flawfinder: ignore
}

/**
 * @brief Print version information
 */
/* flawfinder: ignore - all printf calls below use compile-time constant format strings */
static void print_version(void) {
    printf("BDIX Server Monitor v%s\n", BDIX_VERSION_STRING); // flawfinder: ignore
    printf("Built: %s %s\n", __DATE__, __TIME__); // flawfinder: ignore
    printf("Compiler: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__); // flawfinder: ignore
    printf("\n"); // flawfinder: ignore
}

/**
 * @brief Parse command line arguments
 */
static int parse_arguments(int argc, char *argv[], ProgramOptions *opts) {
    // Set defaults
    // Config file will be determined later if not specified
    memset(opts->config_file, 0, sizeof(opts->config_file));
    opts->thread_count = DEFAULT_THREADS;
    opts->check_ftp = false;
    opts->check_tv = false;
    opts->check_others = false;
    opts->check_all = false;
    opts->only_ok = false;
    opts->no_color = false;
    opts->interactive = true;
    opts->show_stats = false;

    static struct option long_options[] = {
        {"config",      required_argument, 0, 'c'},
        {"threads",     required_argument, 0, 't'},
        {"ftp",         no_argument,       0, 'f'},
        {"tv",          no_argument,       0, 'v'},
        {"others",      no_argument,       0, 'o'},
        {"all",         no_argument,       0, 'a'},
        {"quiet",       no_argument,       0, 'q'},
        {"no-color",    no_argument,       0, 'n'},
        {"interactive", no_argument,       0, 'i'},
        {"stats",       no_argument,       0, 's'},
        {"help",        no_argument,       0, 'h'},
        {"version",     no_argument,       0, 'V'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "c:t:fvoaqnishV", /* flawfinder: ignore */
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                safe_strncpy(opts->config_file, optarg, sizeof(opts->config_file));
                break;
            case 't':
                {
                    char *endptr;
                    long val = strtol(optarg, &endptr, 10);
                    if (*endptr != '\0' || val < MIN_THREADS || val > MAX_THREADS) {
                         fprintf(stderr, "Error: Thread count must be between %d and %d\n", /* flawfinder: ignore */
                                MIN_THREADS, MAX_THREADS);
                         return BDIX_ERROR_INVALID_INPUT;
                    }
                    opts->thread_count = (int)val;
                }
                break;
            case 'f':
                opts->check_ftp = true;
                opts->interactive = false;
                break;
            case 'v':
                opts->check_tv = true;
                opts->interactive = false;
                break;
            case 'o':
                opts->check_others = true;
                opts->interactive = false;
                break;
            case 'a':
                opts->check_all = true;
                opts->interactive = false;
                break;
            case 'q':
                opts->only_ok = true;
                break;
            case 'n':
                opts->no_color = true;
                break;
            case 'i':
                opts->interactive = true;
                break;
            case 's':
                opts->show_stats = true;
                opts->interactive = false;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'V':
                print_version();
                exit(EXIT_SUCCESS);
            default:
                print_usage(argv[0]);
                return BDIX_ERROR_INVALID_INPUT;
        }
    }

    // If no specific check selected, default to all
    if (!opts->check_ftp && !opts->check_tv && !opts->check_others &&
        !opts->show_stats && !opts->interactive) {
        opts->check_all = true;
    }

    return BDIX_SUCCESS;
}

/**
 * @brief Interactive menu mode
 */
static void interactive_mode(ServerData *data, CheckerConfig *config) {
    char input[MAX_INPUT_LENGTH]; /* flawfinder: ignore - bounds checked with ui_get_input */
    int thread_count = DEFAULT_THREADS;
    bool only_ok = false;
    CheckerStats stats;

    while (true) {
        ui_print_menu(thread_count, only_ok);

        if (!ui_get_input("Enter choice: ", input, sizeof(input))) {
            break;
        }

        char *endptr;
        long val = strtol(input, &endptr, 10);
        if (*endptr != '\0') {
             ui_print_error("Invalid number format\n");
             continue;
        }
        int choice = (int)val;
        checker_stats_init(&stats);

        switch (choice) {
            case 1: // Check FTP
                ui_print_info("Checking FTP servers...\n");
                checker_check_multiple(data, config, thread_count,
                                     true, false, false, &stats);
                checker_stats_print(&stats);
                break;

            case 2: // Check TV
                ui_print_info("Checking TV servers...\n");
                checker_check_multiple(data, config, thread_count,
                                     false, true, false, &stats);
                checker_stats_print(&stats);
                break;

            case 3: // Check Others
                ui_print_info("Checking other servers...\n");
                checker_check_multiple(data, config, thread_count,
                                     false, false, true, &stats);
                checker_stats_print(&stats);
                break;

            case 4: // Check All
                ui_print_info("Checking all servers...\n");
                checker_check_multiple(data, config, thread_count,
                                     true, true, true, &stats);
                checker_stats_print(&stats);
                break;

            case 5: // Set thread count
                thread_count = ui_get_int("Enter thread count: ",
                                         MIN_THREADS, MAX_THREADS, thread_count);
                ui_print_success("Thread count set to: %d\n", thread_count);
                break;

            case 6: // Toggle show only OK
                only_ok = !only_ok;
                config->verbose = !only_ok;
                ui_print_success("Show only OK: %s\n", only_ok ? "ON" : "OFF");
                break;

            case 7: // Statistics
                ui_print_server_stats(data);
                break;

            case 8: // Reload config
                ui_print_info("Reloading configuration...\n");
                server_data_free(data);
                if (server_data_init(data) != BDIX_SUCCESS) {
                    ui_print_error("Failed to reinitialize server data\n");
                    return;
                }
                if (config_load_from_file("data/server.json", data) == BDIX_SUCCESS) {
                    ui_print_success("Configuration reloaded successfully\n");
                    ui_print_server_stats(data);
                } else {
                    ui_print_error("Failed to reload configuration\n");
                }
                break;

            case 9: // Save Results to Markdown
                {
                char filename[64]; /* flawfinder: ignore - bounds checked with strftime */
                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    strftime(filename, sizeof(filename), "bdix_results_%Y%m%d_%H%M%S.md", t);

                    ui_export_results_md(data, filename);
                }
                break;

            case 0: // Exit
                ui_print_success("Exiting... Goodbye!\n");
                return;

            default:
                ui_print_error("Invalid choice. Please try again.\n");
        }

        if (choice >= 1 && choice <= 4) {
            ui_wait_for_enter();
        }
    }
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[]) {
    ServerData data = {0};
    CheckerConfig config;
    CheckerStats stats;
    ProgramOptions opts;
    int ret = EXIT_SUCCESS;

    // Parse arguments
    if (parse_arguments(argc, argv, &opts) != BDIX_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Setup colors
    if (opts.no_color) {
        colors_disable();
    } else {
        colors_init();
    }

    // Initialize UI
    UIConfig ui_config = {
        .show_only_ok = opts.only_ok,
        .show_progress = true,
        .show_latency = true,
        .use_colors = !opts.no_color,
        .verbose = !opts.only_ok
    };
    ui_init(&ui_config);

    // Initialize checker
    if (checker_init() != BDIX_SUCCESS) {
        ui_print_error("Failed to initialize checker\n");
        return EXIT_FAILURE;
    }

    // Resources initialized, ensure cleanup via goto

    // Print header
    if (!opts.show_stats) {
        ui_print_header();
    }

    // Initialize server data
    if (server_data_init(&data) != BDIX_SUCCESS) {
        ui_print_error("Failed to initialize server data\n");
        ret = EXIT_FAILURE;
        goto cleanup;
    }

    // Determine configuration file
    if (strlen(opts.config_file) == 0) { /* flawfinder: ignore - opts.config_file is always null-terminated */
        // Try to find configuration in standard locations
        if (config_validate_file("data/server.json")) {
            safe_strncpy(opts.config_file, "data/server.json", sizeof(opts.config_file));
            ui_print_info("Found configuration at: data/server.json\n");
        } else if (config_validate_file("../data/server.json")) {
            safe_strncpy(opts.config_file, "../data/server.json", sizeof(opts.config_file));
            ui_print_info("Found configuration at: ../data/server.json\n");
        } else {
            // Default to local data/server.json if nothing found
            safe_strncpy(opts.config_file, "data/server.json", sizeof(opts.config_file));
            ui_print_warning("No configuration found, using default: data/server.json\n");
        }
    }

    // Load configuration
    ui_print_info("Loading servers from: %s\n", opts.config_file);

    if (config_load_from_file(opts.config_file, &data) != BDIX_SUCCESS) {
        // Only create sample if we are using the default local path and it failed
        if (strcmp(opts.config_file, "data/server.json") == 0) {
            ui_print_warning("Failed to load configuration file\n");
            ui_print_info("Creating sample configuration at data/server.json\n");

            // Create sample config
            if (config_create_sample("data/server.json") == BDIX_SUCCESS) {
                ui_print_success("Sample configuration created. Please edit and rerun.\n");
            } else {
                ui_print_error("Failed to create sample configuration\n");
            }
            ret = EXIT_FAILURE;
            goto cleanup;
        } else {
            ui_print_error("Failed to load configuration from %s\n", opts.config_file);
            ret = EXIT_FAILURE;
            goto cleanup;
        }
    }

    ui_print_success("Loaded: %zu FTP, %zu TV, %zu other servers\n\n",
                    data.ftp.count, data.tv.count, data.others.count);

    // Get default checker config
    config = checker_get_default_config();
    config.verbose = !opts.only_ok;

    // Initialize statistics
    checker_stats_init(&stats);

    // Handle different modes
    if (opts.show_stats) {
        ui_print_server_stats(&data);
        ret = EXIT_SUCCESS;
        goto cleanup;
    }

    if (opts.interactive) {
        interactive_mode(&data, &config);
        ret = EXIT_SUCCESS;
        goto cleanup;
    }

    // Non-interactive mode: check servers based on options
    bool check_ftp = opts.check_ftp || opts.check_all;
    bool check_tv = opts.check_tv || opts.check_all;
    bool check_others = opts.check_others || opts.check_all;

    if (checker_check_multiple(&data, &config, opts.thread_count,
                               check_ftp, check_tv, check_others,
                               &stats) != BDIX_SUCCESS) {
        ui_print_error("Server checking failed\n");
        ret = EXIT_FAILURE;
    }

    // Print final statistics
    printf("\n"); /* flawfinder: ignore */
    checker_stats_print(&stats);

cleanup:
    checker_cleanup();
    ui_cleanup();
    server_data_free(&data);

    return ret;
}
