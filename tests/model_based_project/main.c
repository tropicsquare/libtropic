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
// Full examples
#ifdef LT_EX_HELLO_WORLD
    /*int*/ lt_ex_hello_world();
#endif
#ifdef LT_EX_HELLO_WORLD_SEPARATE_API
    /*int*/ lt_ex_hello_world_separate_API();
#endif
#ifdef LT_EX_HW_WALLET
    /*int*/ lt_ex_hardware_wallet();
#endif
#ifdef LT_EX_MACANDD
    /*int*/ lt_ex_macandd();
#endif

#ifdef LT_BUILD_TESTS
#include "lt_test_registry.c.inc"
#endif

    return 0;
}
