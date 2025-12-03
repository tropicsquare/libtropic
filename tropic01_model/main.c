/**
 * @file main.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "libtropic_port_posix_tcp.h"
#if LT_USE_TREZOR_CRYPTO
#include "libtropic_trezor_crypto.h"
#elif LT_USE_MBEDTLS_V4
#include <inttypes.h>

#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"
#endif

int main(void)
{
    int ret = 0;

#if LT_USE_MBEDTLS_V4
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("PSA Crypto initialization failed, status=%" PRId32 " (psa_status_t)", status);
        return -1;
    }
#endif

#ifdef LT_BUILD_TESTS
    // Disable buffering on stdout and stderr (problem in GitHub CI)
    ret = setvbuf(stdout, NULL, _IONBF, 0);
    if (ret != 0) {
        LT_LOG_ERROR("setvbuf(stdout, ...) failed, ret=%d", ret);
        return ret;
    }
    ret = setvbuf(stderr, NULL, _IONBF, 0);
    if (ret != 0) {
        LT_LOG_ERROR("setvbuf(stderr, ...) failed, ret=%d", ret);
        return ret;
    }
#endif

    lt_handle_t __lt_handle__ = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    __lt_handle__.l3.buff = l3_buffer;
    __lt_handle__.l3.buff_len = sizeof(l3_buffer);
#endif
    // Initialize device before handing handle to the test.
    lt_dev_posix_tcp_t device;
    device.addr = inet_addr("127.0.0.1");
    device.port = 28992;
    __lt_handle__.l2.device = &device;

    // Initialize crypto context.
#if LT_USE_TREZOR_CRYPTO
    lt_ctx_trezor_crypto_t
#elif LT_USE_MBEDTLS_V4
    lt_ctx_mbedtls_v4_t
#endif
        crypto_ctx;
    __lt_handle__.l3.crypto_ctx = &crypto_ctx;

    // Generate seed for the PRNG.
    unsigned int prng_seed;
    if (0 != getentropy(&prng_seed, sizeof(prng_seed))) {
        LT_LOG_ERROR("main: getentropy() failed (%s)!", strerror(errno));
        return -1;
    }

    // Seed the PRNG.
    // Note: We use rand() for random numbers, which is not cryptographically secure, but it is okay here because the
    // TCP port is targeted for use with the model only. Thanks to this, we can log the used seed and if needed,
    // reproduce the random tests.
    srand(prng_seed);
    LT_LOG_INFO("PRNG initialized with seed=%u\n", prng_seed);

#ifdef LT_BUILD_TESTS
#include "lt_test_registry.c.inc"
#endif

// When examples are being built, special variable containing example return value is defined.
// Otherwise, 0 is always returned (in case of building tests).
#ifdef LT_BUILD_EXAMPLES
#include "lt_ex_registry.c.inc"
    ret = __lt_ex_return_val__;
#endif

#if LT_USE_MBEDTLS_V4
    mbedtls_psa_crypto_free();
#endif

    return ret;
}
