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
        } else {
            LT_LOG_ERROR("Post-assert cleanup failed!");
        }
    }
    else {
        LT_LOG_INFO("Cleanup function not defined -- skipped post-assert cleaning.");
    }
    LT_FINISH_TEST();
}