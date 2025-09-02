#ifndef LT_LIBTROPIC_LOGGING_H
#define LT_LIBTROPIC_LOGGING_H

/**
 * @file libtropic_logging.h
 * @brief Macros for logging.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <assert.h>
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

/** @brief Dummy macro used when no logging is configured. */
#define LT_LOG_DISABLED(...)                                     \
    do {                                                         \
        if (0) {                                                 \
            /* A base string ensures printf is always valid. */  \
            /* Using a single space is minimal and effective. */ \
            printf(" " __VA_ARGS__);                             \
        }                                                        \
    } while (0)

#if LT_LOG_ENABLE_INFO
#define LT_LOG_INFO(f_, ...) printf("INFO    [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#else
#define LT_LOG_INFO(f_, ...) LT_LOG_DISABLED(f_, ##__VA_ARGS__)
#endif

#if LT_LOG_ENABLE_WARN
#define LT_LOG_WARN(f_, ...) printf("WARNING [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#else
#define LT_LOG_WARN(f_, ...) LT_LOG_DISABLED(f_, ##__VA_ARGS__)
#endif

#if LT_LOG_ENABLE_ERROR
#define LT_LOG_ERROR(f_, ...) printf("ERROR   [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#else
#define LT_LOG_ERROR(f_, ...) LT_LOG_DISABLED(f_, ##__VA_ARGS__)
#endif

#if LT_LOG_ENABLE_DEBUG
#define LT_LOG_DEBUG(f_, ...) printf("DEBUG   [%4d] " f_ "\r\n", __LINE__, ##__VA_ARGS__)
#else
#define LT_LOG_DEBUG(f_, ...) LT_LOG_DISABLED(f_, ##__VA_ARGS__)
#endif

// Assertions. Will log as a system message and call native assert function.
// Note that parameters are stored to _val_ and _exp_ for a case when there
// are function calls passed to the macros. Without the helper variables
// the functions will be called mutliple times -- in the first comparison
// (if statement), LT_LOG_ERROR and finally in the assert(). This can cause
// unexpected behaviour.
#define LT_ASSERT(expected, value)                                                 \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (expected);                                                    \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
        };                                                                         \
        assert(_exp_ == _val_);                                                    \
    }

#define LT_ASSERT_COND(value, condition, expected_if_true, expected_if_false)      \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (condition ? expected_if_true : expected_if_false);            \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
        }                                                                          \
        assert(_exp_ == _val_);                                                    \
    }

#endif /* LT_LIBTROPIC_LOGGING_H */