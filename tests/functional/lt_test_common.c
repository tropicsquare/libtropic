/**
 * @file lt_test_common.c
 * @brief Common variables for functional tests.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

lt_ret_t (*lt_test_cleanup_function)(void) = NULL;

void lt_assert_fail_handler(void)
{
    if (NULL != lt_test_cleanup_function) {
        LT_LOG_INFO("Post-assert cleanup started.");
        lt_ret_t ret = lt_test_cleanup_function();
        if (LT_OK == ret) {
            LT_LOG_INFO("Post-assert cleanup successful!");
        }
        else {
            LT_LOG_ERROR("Post-assert cleanup failed, ret=%s.", lt_ret_verbose(ret));
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
            p += sprintf(p, "%02" PRIx8 " ", data[i + j]);
        }

        *p = '\0';  // null-terminate the string
        LT_LOG_INFO("%s", line);
    }
}

int chip_id_printf_wrapper(const char *format, ...)
{
    int ret;
    char buff[LT_CHIP_ID_FIELD_MAX_SIZE * 3];
    va_list args;

    // Format the message
    va_start(args, format);
    ret = vsnprintf(buff, sizeof(buff), format, args);
    if (ret < 0) {
        return ret;
    }
    va_end(args);

    // Strip trailing \r\n
    size_t len = strlen(buff);
    while (len > 0 && (buff[len - 1] == '\n' || buff[len - 1] == '\r')) {
        buff[--len] = '\0';
    }

    // Log the message
    LT_LOG_INFO("%s", buff);

    return ret;
}