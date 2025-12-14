/**
 * @file config.h
 * @brief Configuration file loading and parsing
 * @version 1.0.0
 */

#ifndef BDIX_CONFIG_H
#define BDIX_CONFIG_H

#include "common.h"
#include "server.h"

/**
 * @brief Load server configuration from JSON file
 *
 * @param filename Path to JSON configuration file
 * @param data Pointer to server data structure to populate
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int config_load_from_file(const char *filename, ServerData *data);

/**
 * @brief Load server configuration from JSON string
 *
 * @param json_str JSON string containing configuration
 * @param data Pointer to server data structure to populate
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int config_load_from_string(const char *json_str, ServerData *data);

/**
 * @brief Validate configuration file
 *
 * @param filename Path to JSON configuration file
 * @return true if valid, false otherwise
 */
bool config_validate_file(const char *filename);

/**
 * @brief Create sample configuration file
 *
 * @param filename Path where to create the sample file
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int config_create_sample(const char *filename);

/**
 * @brief Save server data to JSON file
 *
 * @param filename Path to output file
 * @param data Pointer to server data to save
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int config_save_to_file(const char *filename, const ServerData *data);

#endif // BDIX_CONFIG_H
