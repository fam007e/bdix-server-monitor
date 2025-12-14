#include "test_common.h"
#include "../include/config.h"

int test_config_load_string(void) {
    const char *json_data =
        "{"
        "  \"ftp\": [\"http://ftp1.com\", \"http://ftp2.com\"],"
        "  \"tv\": [\"http://tv1.com\"],"
        "  \"others\": []"
        "}";

    ServerData data;
    server_data_init(&data);

    int result = config_load_from_string(json_data, &data);

    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(2, data.ftp.count);
    TEST_ASSERT_EQUAL_INT(1, data.tv.count);
    TEST_ASSERT_EQUAL_INT(0, data.others.count);
    TEST_ASSERT_EQUAL_INT(3, data.total_servers);

    TEST_ASSERT_EQUAL_STR("http://ftp1.com", data.ftp.urls[0]);
    TEST_ASSERT_EQUAL_STR("http://tv1.com", data.tv.urls[0]);

    server_data_free(&data);
    return 1;
}

int test_config_load_invalid(void) {
    const char *invalid_json = "{ \"ftp\": [ unquoted ] }";
    ServerData data;
    server_data_init(&data);

    int result = config_load_from_string(invalid_json, &data);
    TEST_ASSERT(result != BDIX_SUCCESS, "Should fail on invalid JSON");

    server_data_free(&data);
    return 1;
}

int test_config_sample_creation(void) {
    const char *test_file = "test_sample_config.json";

    // Ensure cleanup
    remove(test_file);

    int result = config_create_sample(test_file);
    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, result);

    // Verify it exists and is valid
    TEST_ASSERT(config_validate_file(test_file), "Created sample should be valid");

    // Clean up
    remove(test_file);
    return 1;
}
