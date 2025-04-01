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
    // Test routines
#ifdef LT_EX_TEST_REVERSIBLE
    lt_ex_test_reversible();
#endif
#ifdef LT_EX_TEST_IREVERSIBLE
    lt_ex_test_ireversible();
#endif

    // Full examples
#ifdef LT_FEATURES_FWUPDATE
    lt_ex_fwupdate();
#endif
#ifdef LT_EX_HELLO_WORLD
    /*int*/ lt_ex_hello_world();
#endif
#ifdef LT_EX_HW_WALLET
    /*int*/ lt_ex_hardware_wallet();
#endif
#ifdef LT_TEST_SAMPLES_1
    /*int*/ lt_test_samples_1();
#endif
}
