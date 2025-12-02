#ifndef LT_TEST_COMMON_H
#define LT_TEST_COMMON_H

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lt_ret_t (*lt_test_cleanup_function)(void);

#if LT_USE_SH0_ENG_SAMPLE
#define LT_TEST_SH0_PRIV sh0priv_eng_sample
#define LT_TEST_SH0_PUB sh0pub_eng_sample
#elif LT_USE_SH0_PROD0
#define LT_TEST_SH0_PRIV sh0priv_prod0
#define LT_TEST_SH0_PUB sh0pub_prod0
#endif

/**
 * @brief Called when `LT_TEST_ASSERT` or `LT_TEST_ASSERT_COND` fails.
 *
 */
void lt_assert_fail_handler(void);

// Assertions -- special variant for functional tests.
// Will log as a system message and call native assert function.
// On error, they will also call a cleanup function, if not NULL. The pointer
// is usually assigned at the start of a functional test, if the test requires a cleanup
// on each error (and exit of course) to be truly reversible.
//
// Note that parameters are stored to _val_ and _exp_ for a case when there
// are function calls passed to the macros. Without the helper variables
// the functions will be called mutliple times -- in the first comparison
// (if statement), LT_LOG_ERROR and finally in the assert(). This can cause
// unexpected behaviour.
#define LT_TEST_ASSERT(expected, value)                                            \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (expected);                                                    \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
            lt_assert_fail_handler();                                              \
        };                                                                         \
        assert(_exp_ == _val_);                                                    \
    }

#define LT_TEST_ASSERT_COND(value, condition, expected_if_true, expected_if_false) \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (condition ? expected_if_true : expected_if_false);            \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
            lt_assert_fail_handler();                                              \
        }                                                                          \
        assert(_exp_ == _val_);                                                    \
    }

// Used to instruct the test runner that the test finshed and may disconnect (useful in embedded ports).
#define LT_FINISH_TEST()               \
    {                                  \
        LT_LOG_INFO("TEST FINISHED!"); \
    }

/**
 * @brief Non-test function to dump bytes in 8 byte rows.
 *
 * @param data Buffer to dump
 * @param size Size of the buffer to dump
 */
void hexdump_8byte(const uint8_t *data, uint16_t size);

/**
 * @brief Printf-like wrapper for LT_LOG_INFO used with lt_print_chip_id().
 *
 * @param format  A printf-style format string describing how to format the subsequent arguments. Must be a
 * null-terminated string.
 * @param ...     Additional arguments corresponding to the format specifiers in `format`.
 *
 * @return       The number of characters printed (excluding the terminating null byte), or a negative value if an
 * output error occurs.
 */
int chip_id_printf_wrapper(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif  // LT_TEST_COMMON_H