/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"
#include "string.h"

int main(void)
{
#ifdef LT_BUILD_EXAMPLES
#include "lt_ex_registry.c.inc"
#endif

#ifdef LT_BUILD_TESTS
#include "lt_test_registry.c.inc"
#endif

    return 0;
}
