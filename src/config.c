/**
 * @file config.c
 * @brief Configuration file loading and parsing implementation
 * @version 1.0.0
 */

#include "config.h"
#include <jansson.h>
#include <sys/stat.h>

/**
 * @brief Parse JSON array and add servers to category
 */
static int parse_server_array(json_t *array, ServerCategory *category) {
    if (!json_is_array(array) || !category) {
        return BDIX_ERROR_INVALID_INPUT;
    }

    size_t index;
    json_t *value;

    json_array_foreach(array, index, value) {
        if (!json_is_string(value)) {
            LOG_WARN("Skipping non-string value in array at index %zu", index);
            continue;
        }

        const char *url = json_string_value(value);
        if (!url || strlen(url) == 0) { /* flawfinder: ignore - null checked above */
            LOG_WARN("Skipping empty URL at index %zu", index);
            continue;
        }

        // Add server to category
        if (server_category_add(category, url) != BDIX_SUCCESS) {
            LOG_WARN("Failed to add server: %s", url);
        }
    }

    LOG_DEBUG("Finished parsing category '%s'", category->name);
    return BDIX_SUCCESS;
}

/**
 * @brief Load server configuration from JSON file
 */
int config_load_from_file(const char *filename, ServerData *data) {
    if (!filename || !data) {
        LOG_ERROR("Invalid parameters for config loading");
        return BDIX_ERROR_INVALID_INPUT;
    }

    LOG_INFO("Loading configuration from: %s", filename);

    // Check if file exists
    struct stat st;
    if (stat(filename, &st) != 0) {
        LOG_ERROR("Configuration file not found: %s", filename);
        return BDIX_ERROR_FILE_NOT_FOUND;
    }

    // Load JSON file
    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root) {
        LOG_ERROR("JSON parsing error at line %d, column %d: %s",
                  error.line, error.column, error.text);
        return BDIX_ERROR_JSON_PARSE;
    }

    // Validate root is an object
    if (!json_is_object(root)) {
        LOG_ERROR("JSON root must be an object");
        json_decref(root);
        return BDIX_ERROR_JSON_PARSE;
    }

    int ret = BDIX_SUCCESS;

    // Parse FTP servers
    json_t *ftp_array = json_object_get(root, "ftp");
    if (ftp_array) {
        if (parse_server_array(ftp_array, &data->ftp) != BDIX_SUCCESS) {
            LOG_WARN("Failed to parse FTP servers");
        }
    } else {
        LOG_WARN("No 'ftp' array found in configuration");
    }

    // Parse TV servers
    json_t *tv_array = json_object_get(root, "tv");
    if (tv_array) {
        if (parse_server_array(tv_array, &data->tv) != BDIX_SUCCESS) {
            LOG_WARN("Failed to parse TV servers");
        }
    } else {
        LOG_WARN("No 'tv' array found in configuration");
    }

    // Parse other servers
    json_t *others_array = json_object_get(root, "others");
    if (others_array) {
        if (parse_server_array(others_array, &data->others) != BDIX_SUCCESS) {
            LOG_WARN("Failed to parse other servers");
        }
    } else {
        LOG_WARN("No 'others' array found in configuration");
    }

    // Update total count
    data->total_servers = server_data_count(data);

    json_decref(root);

    if (data->total_servers == 0) {
        LOG_WARN("No servers loaded from configuration");
        return BDIX_ERROR;
    }

    LOG_INFO("Successfully loaded %zu servers from configuration", data->total_servers);
    return ret;
}

/**
 * @brief Load server configuration from JSON string
 */
int config_load_from_string(const char *json_str, ServerData *data) {
    if (!json_str || !data) {
        LOG_ERROR("Invalid parameters for config loading from string");
        return BDIX_ERROR_INVALID_INPUT;
    }

    LOG_DEBUG("Loading configuration from string");

    // Parse JSON string
    json_error_t error;
    json_t *root = json_loads(json_str, 0, &error);

    if (!root) {
        LOG_ERROR("JSON parsing error: %s", error.text);
        return BDIX_ERROR_JSON_PARSE;
    }

    // Validate root is an object
    if (!json_is_object(root)) {
        LOG_ERROR("JSON root must be an object");
        json_decref(root);
        return BDIX_ERROR_JSON_PARSE;
    }

    // Parse arrays (same as file loading)
    json_t *ftp_array = json_object_get(root, "ftp");
    if (ftp_array) {
        parse_server_array(ftp_array, &data->ftp);
    }

    json_t *tv_array = json_object_get(root, "tv");
    if (tv_array) {
        parse_server_array(tv_array, &data->tv);
    }

    json_t *others_array = json_object_get(root, "others");
    if (others_array) {
        parse_server_array(others_array, &data->others);
    }

    data->total_servers = server_data_count(data);

    json_decref(root);

    LOG_INFO("Loaded %zu servers from JSON string", data->total_servers);
    return BDIX_SUCCESS;
}

/**
 * @brief Validate configuration file
 */
bool config_validate_file(const char *filename) {
    if (!filename) {
        return false;
    }

    // Check file exists
    struct stat st;
    if (stat(filename, &st) != 0) {
        LOG_WARN("File does not exist: %s", filename);
        return false;
    }

    // Try to parse JSON
    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root) {
        LOG_WARN("Invalid JSON: %s", error.text);
        return false;
    }

    bool valid = json_is_object(root);

    if (valid) {
        // Check for required arrays
        json_t *ftp = json_object_get(root, "ftp");
        json_t *tv = json_object_get(root, "tv");
        json_t *others = json_object_get(root, "others");

        if (!ftp && !tv && !others) {
            LOG_WARN("No server arrays found in configuration");
            valid = false;
        }
    }

    json_decref(root);
    return valid;
}

/**
 * @brief Create sample configuration file
 */
int config_create_sample(const char *filename) {
    if (!filename) {
        LOG_ERROR("Invalid filename for sample config");
        return BDIX_ERROR_INVALID_INPUT;
    }

    LOG_INFO("Creating sample configuration at: %s", filename);

    // Create directory if needed
#ifdef _WIN32
    #include <direct.h>
    #define fs_mkdir(path, mode) _mkdir(path)
#else
    #define fs_mkdir(path, mode) mkdir(path, mode)
#endif
    char dir_path[MAX_PATH_LENGTH]; /* flawfinder: ignore - bounds checked with safe_strncpy */
    safe_strncpy(dir_path, filename, sizeof(dir_path));

    char *last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        fs_mkdir(dir_path, 0755);  // Create directory, ignore errors if exists
    }

    // Open file for writing
    FILE *fp = fopen(filename, "w"); // flawfinder: ignore
    if (!fp) {
        LOG_ERROR("Cannot create file: %s (errno: %d)", filename, errno);
        return BDIX_ERROR_FILE_NOT_FOUND;
    }

    // Write sample JSON configuration
    /* flawfinder: ignore - all format strings below are compile-time constants */
    fprintf(fp, "{\n");
    fprintf(fp, "  \"ftp\": [\n");
    fprintf(fp, "    \"http://ftp.amigait.com\",\n");
    fprintf(fp, "    \"http://ftp.dhakacom.net\",\n");
    fprintf(fp, "    \"http://ftp.circleftp.net\",\n");
    fprintf(fp, "    \"http://ftp.somewhereinblog.net\",\n");
    fprintf(fp, "    \"http://server1.amader.gov.bd\"\n");
    fprintf(fp, "  ],\n");
    fprintf(fp, "  \"tv\": [\n");
    fprintf(fp, "    \"http://tv.bdix.app\",\n");
    fprintf(fp, "    \"http://iptv.live.net.bd\",\n");
    fprintf(fp, "    \"http://bdixsports.com\"\n");
    fprintf(fp, "  ],\n");
    fprintf(fp, "  \"others\": [\n");
    fprintf(fp, "    \"http://cache.google.com\",\n");
    fprintf(fp, "    \"http://mirror.dhakacom.com\",\n");
    fprintf(fp, "    \"http://bdix.net\"\n");
    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");

    fclose(fp);

    LOG_INFO("Sample configuration created successfully");
    return BDIX_SUCCESS;
}

/**
 * @brief Save server data to JSON file
 */
int config_save_to_file(const char *filename, const ServerData *data) {
    if (!filename || !data) {
        LOG_ERROR("Invalid parameters for config saving");
        return BDIX_ERROR_INVALID_INPUT;
    }

    LOG_INFO("Saving configuration to: %s", filename);

    // Create JSON root object
    json_t *root = json_object();
    if (!root) {
        LOG_ERROR("Failed to create JSON object");
        return BDIX_ERROR;
    }

    // Create FTP array
    json_t *ftp_array = json_array();
    for (size_t i = 0; i < data->ftp.count; i++) {
        json_array_append_new(ftp_array, json_string(data->ftp.urls[i]));
    }
    json_object_set_new(root, "ftp", ftp_array);

    // Create TV array
    json_t *tv_array = json_array();
    for (size_t i = 0; i < data->tv.count; i++) {
        json_array_append_new(tv_array, json_string(data->tv.urls[i]));
    }
    json_object_set_new(root, "tv", tv_array);

    // Create others array
    json_t *others_array = json_array();
    for (size_t i = 0; i < data->others.count; i++) {
        json_array_append_new(others_array, json_string(data->others.urls[i]));
    }
    json_object_set_new(root, "others", others_array);

    // Save to file with pretty printing
    if (json_dump_file(root, filename, JSON_INDENT(2)) != 0) {
        LOG_ERROR("Failed to save JSON to file: %s", filename);
        json_decref(root);
        return BDIX_ERROR;
    }

    json_decref(root);

    LOG_INFO("Configuration saved successfully");
    return BDIX_SUCCESS;
}
