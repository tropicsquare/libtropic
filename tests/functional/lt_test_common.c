/**
 * @file lt_test_common.c
 * @brief Common variables for functional tests.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>

void (*lt_test_cleanup_function)(void) = NULL;