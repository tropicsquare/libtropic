/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdio.h>

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

// Test routines
#ifdef LT_TEST_CHIP_ID
    /*int*/ lt_test_chip_id();
#endif
#ifdef LT_TEST_WRITE_PAIRING_KEYS
    /*int*/ lt_test_write_pairing_keys();
#endif
#ifdef LT_TEST_WRITE_R_CONFIG
    /*int*/ lt_test_write_r_config();
#endif
#ifdef LT_TEST_PING
    /*int*/ lt_test_ping();
#endif
#ifdef LT_TEST_ENGINEERING_SAMPLE_01_PING
    /*int*/ lt_test_engineering_sample_01_ping();
#endif
#ifdef LT_TEST_ECC_EDDSA
    /*int*/ lt_test_ecc_eddsa();
#endif
#ifdef LT_TEST_R_MEM
    /*int*/ lt_test_r_mem();
#endif
#ifdef LT_TEST_ERASE_R_CONFIG
    /*int*/ lt_test_erase_r_config();
#endif
}
