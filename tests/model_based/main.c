/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>
#include "string.h"
#include "libtropic_examples.h"

int main(void)
{
    // Full examples
#ifdef LT_EX_HELLO_WORLD
    /*int*/ lt_ex_hello_world();
#endif
#ifdef LT_EX_HW_WALLET
    /*int*/ lt_ex_hardware_wallet();
#endif

// Test routines
#ifdef LT_TEST_REVERSIBLE
    /*int*/ lt_test_reversible();
#endif
#ifdef LT_TEST_IREVERSIBLE
    /*int*/ lt_test_ireversible();
#endif
#ifdef LT_TEST_SAMPLES_1
    /*int*/ lt_test_samples_1();
#endif
}
