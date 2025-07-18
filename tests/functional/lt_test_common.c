/**
 * @file lt_test_common.c
 * @brief Common variables for functional tests.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>
#include "libtropic_functional_tests.h"

void (*lt_test_cleanup_function)(void) = NULL;

void lt_assert_fail_handler() {
    if (lt_test_cleanup_function != NULL) {
        LT_LOG_INFO("Cleanup started.");
        lt_test_cleanup_function();
        LT_LOG_INFO("Cleanup finished.");
    } else {
        LT_LOG_INFO("Cleanup function not defined -- skipped cleaning.");
    }
    LT_FINISH_TEST();
}