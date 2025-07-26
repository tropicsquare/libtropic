/**
 * @file lt_test_common.c
 * @brief Common variables for functional tests.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>

#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

lt_ret_t (*lt_test_cleanup_function)(void) = NULL;

void lt_assert_fail_handler()
{
    if (NULL != lt_test_cleanup_function) {
        LT_LOG_INFO("Post-assert cleanup started.");
        if (LT_OK == lt_test_cleanup_function()) {
            LT_LOG_INFO("Post-assert cleanup successful!");
        }
        else {
            LT_LOG_ERROR("Post-assert cleanup failed!");
        }
    }
    else {
        LT_LOG_INFO("Cleanup function not defined -- skipped post-assert cleaning.");
    }
    LT_FINISH_TEST();
}

void hexdump_8byte(const uint8_t *data, uint16_t size)
{
    char line[3 * 8 + 1];  // 2 hex chars + 1 space per byte, plus null terminator

    for (uint16_t i = 0; i < size; i += 8) {
        uint16_t row_len = (size - i < 8) ? size - i : 8;
        char *p = line;

        for (uint16_t j = 0; j < row_len; j++) {
            p += sprintf(p, "%02x ", data[i + j]);
        }

        *p = '\0';  // null-terminate the string
        LT_LOG_INFO("%s", line);
    }
}