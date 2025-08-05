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

    lt_handle_t __lt_handle__ = {0};

#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_PACKET_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

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
