#ifndef LT_LIBTROPIC_LOGGING_H
#define LT_LIBTROPIC_LOGGING_H

/**
 * @file libtropic_logging.h
 * @brief Macros for logging.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>

// Only info-level loggers and decorators.
// This has no effect, test runner just simply copies these lines to the log.
#define LT_LOG(f_, ...) LT_LOG_INFO(f_, ##__VA_ARGS__)
#define LT_LOG_RESULT(f_, ...) LT_LOG_INFO("  result: " f_, ##__VA_ARGS__)
#define LT_LOG_VALUE(f_, ...) LT_LOG_INFO("\t\t- " f_, ##__VA_ARGS__)
#define LT_LOG_LINE(f_, ...)                                                                                           \
    LT_LOG_INFO(                                                                                                       \
        "\t----------------------------------------------------------------------------------------------------------" \
        "---" f_,                                                                                                      \
        ##__VA_ARGS__)

// Loggers with selectable message type.
#define LT_LOG_INFO(f_, ...) printf("%d\t;INFO;" f_ "\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_WARN(f_, ...) printf("%d\t;WARNING;" f_ "\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_ERROR(f_, ...) printf("%d\t;ERROR;" f_ "\r\n", __LINE__, ##__VA_ARGS__)
// Logger for system messages -- e.g. finishing a test.
#define LT_LOG_SYSTEM(f_, ...) printf("%d\t;SYSTEM;" f_ "\r\n", __LINE__, ##__VA_ARGS__)

// Assertions. Will log as a system message.
#define LT_ASSERT(expected, value)              \
    if (value == expected) {                    \
        LT_LOG_SYSTEM("ASSERT_OK");             \
    }                                           \
    else {                                      \
        LT_LOG_SYSTEM("ASSERT_FAIL %d", value); \
    };

#define LT_ASSERT_COND(value, condition, expected_if_true, expected_if_false) \
    if (value == (condition ? expected_if_true : expected_if_false)) {        \
        LT_LOG_SYSTEM("ASSERT_OK");                                           \
    }                                                                         \
    else {                                                                    \
        LT_LOG_SYSTEM("ASSERT_FAIL");                                         \
    }
// Used to stop the test. Will log as a system message.
#define LT_FINISH_TEST() LT_LOG_SYSTEM("TEST_FINISH")

#endif /* LT_LIBTROPIC_LOGGING_H */