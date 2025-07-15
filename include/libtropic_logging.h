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
#ifdef LT_USE_ASSERT
#include <assert.h>
#endif

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
#define LT_LOG_INFO(f_, ...) printf("INFO    [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_WARN(f_, ...) printf("WARNING [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_ERROR(f_, ...) printf("ERROR   [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
// Logger for system messages -- e.g. finishing a test.
#define LT_LOG_SYSTEM(f_, ...) printf("SYSTEM  [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)

// Assertions. Will log as a system message.
#ifdef LT_USE_ASSERT
#define LT_ASSERT(expected, value) assert(expected == value);
#else
#define LT_ASSERT(expected, value)                                                  \
    {                                                                               \
        int _val_ = (value);                                                        \
        int _exp_ = (expected);                                                     \
        if (_val_ == _exp_) {                                                       \
            LT_LOG_SYSTEM("ASSERT PASSED!");                                        \
        }                                                                           \
        else {                                                                      \
            LT_LOG_SYSTEM("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
        };                                                                          \
    }
#endif

#ifdef LT_USE_ASSERT
#define LT_ASSERT_COND(value, condition, expected_if_true, expected_if_false) \
    assert(value == (condition ? expected_if_true : expected_if_false));
#else
#define LT_ASSERT_COND(value, condition, expected_if_true, expected_if_false)       \
    int _val_ = (value);                                                            \
    int _exp_ = (condition ? expected_if_true : expected_if_false);                 \
    if (_val_ == _exp_) {                                                           \
        LT_LOG_SYSTEM("ASSERT PASSED!");                                            \
    }                                                                               \
    else {                                                                          \
        LT_LOG_SYSTEM("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_);     \
    }
#endif

// Used to stop the test. Will log as a system message.
#define LT_FINISH_TEST() LT_LOG_SYSTEM("TEST FINISHED!")

#endif /* LT_LIBTROPIC_LOGGING_H */