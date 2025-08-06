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
#ifdef LT_BUILD_TESTS
#include "lt_test_registry.c.inc"
#endif

// When examples are being built, special variable containing example return value is defined.
// Otherwise, 0 is always returned (in case of building tests).
#ifdef LT_BUILD_EXAMPLES
#include "lt_ex_registry.c.inc"
    return __lt_ex_return_val__;
#else
    return 0;
#endif
}
