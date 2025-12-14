#include "test_common.h"
#include "../include/server.h"

int test_server_category_init(void) {
    ServerCategory cat;
    int result = server_category_init(&cat, CATEGORY_FTP, "FTP Test");

    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(CATEGORY_FTP, cat.type);
    TEST_ASSERT_EQUAL_STR("FTP Test", cat.name);
    TEST_ASSERT_EQUAL_INT(0, cat.count);
    TEST_ASSERT(cat.capacity > 0, "Capacity should be initialized > 0");
    TEST_ASSERT_NOT_NULL(cat.urls);
    TEST_ASSERT_NOT_NULL(cat.servers);

    server_category_free(&cat);
    return 1;
}

int test_server_category_add(void) {
    ServerCategory cat;
    server_category_init(&cat, CATEGORY_TV, "TV Test");

    const char *url1 = "http://example.com";
    const char *url2 = "http://test.com";

    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, server_category_add(&cat, url1));
    TEST_ASSERT_EQUAL_INT(1, cat.count);
    TEST_ASSERT_EQUAL_STR(url1, cat.urls[0]);
    TEST_ASSERT_EQUAL_STR(url1, cat.servers[0].url);

    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, server_category_add(&cat, url2));
    TEST_ASSERT_EQUAL_INT(2, cat.count);
    TEST_ASSERT_EQUAL_STR(url2, cat.urls[1]);

    server_category_free(&cat);
    return 1;
}

int test_server_category_resize(void) {
    ServerCategory cat;
    server_category_init(&cat, CATEGORY_OTHERS, "Resize Test");

    // Add more servers than initial capacity (assuming initial is small, e.g. 10 or 16)
    // We add 100 to force resize
    for (int i = 0; i < 100; i++) {
        char url[64];
        snprintf(url, sizeof(url), "http://server%d.com", i);
        int res = server_category_add(&cat, url);
        if (res != BDIX_SUCCESS) {
            printf("Failed to add server %d\n", i);
            server_category_free(&cat);
            return 0;
        }
    }

    TEST_ASSERT_EQUAL_INT(100, cat.count);
    TEST_ASSERT(cat.capacity >= 100, "Capacity should have increased");

    Server *s = server_category_get(&cat, 99);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STR("http://server99.com", s->url);

    server_category_free(&cat);
    return 1;
}

int test_server_data_lifecycle(void) {
    ServerData data;
    TEST_ASSERT_EQUAL_INT(BDIX_SUCCESS, server_data_init(&data));

    TEST_ASSERT_EQUAL_INT(CATEGORY_FTP, data.ftp.type);
    TEST_ASSERT_EQUAL_INT(CATEGORY_TV, data.tv.type);
    TEST_ASSERT_EQUAL_INT(CATEGORY_OTHERS, data.others.type);

    server_data_free(&data);
    return 1;
}

int test_server_update_status(void) {
    Server s;
    memset(&s, 0, sizeof(Server));
    strcpy(s.url, "http://test.com");

    server_update_status(&s, BDIX_STATUS_ONLINE, 45.5, 200);

    TEST_ASSERT_EQUAL_INT(BDIX_STATUS_ONLINE, s.status);
    // Floating point comparison needs epsilon, but let's check basic equality for now
    // or use a range if we had a macro. 45.5 should be exact in double usually.
    TEST_ASSERT(s.latency_ms == 45.5, "Latency mismatch");
    TEST_ASSERT_EQUAL_INT(200, s.response_code);
    TEST_ASSERT(s.last_checked > 0, "Timestamp should be set");

    return 1;
}
