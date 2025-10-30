/**
 * @file main.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "libtropic_port_unix_tcp.h"
#if LT_USE_TREZOR_CRYPTO
#include "lt_trezor_crypto.h"
#elif LT_USE_MBEDTLS_V4
#include "lt_mbedtls_v4.h"
#endif

int main(void)
{
#ifdef LT_BUILD_TESTS
    // Disable buffering on stdout and stderr (problem in GitHub CI)
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif

    lt_handle_t __lt_handle__ = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    __lt_handle__.l3.buff = l3_buffer;
    __lt_handle__.l3.buff_len = sizeof(l3_buffer);
#endif
    // Initialize device before handing handle to the test.
    lt_dev_unix_tcp_t device;
    device.addr = inet_addr("127.0.0.1");
    device.port = 28992;
    device.rng_seed = (unsigned int)time(NULL);
    __lt_handle__.l2.device = &device;

#if LT_USE_TREZOR_CRYPTO
    lt_ctx_trezor_crypto_t crypto_ctx;
#elif LT_USE_MBEDTLS_V4
    lt_ctx_mbedtls_v4_t crypto_ctx;
#endif
    __lt_handle__.l3.crypto_ctx = &crypto_ctx;

    LT_LOG_INFO("RNG initialized with seed=%u\n", device.rng_seed);

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
