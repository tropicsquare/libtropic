/**
 * @file main.c
 * @brief Common entrypoint for running functional tests against TROPIC01 model.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port_posix_tcp.h"

#if LT_USE_TREZOR_CRYPTO
#include "libtropic_trezor_crypto.h"
#elif LT_USE_MBEDTLS_V4
#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"
#endif

int main(void)
{
    // CFP initialization
#if LT_USE_MBEDTLS_V4
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("PSA Crypto initialization failed, status=%d (psa_status_t)", status);
        return -1;
    }
#endif

    // Handle initialization
    lt_handle_t lt_handle = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    lt_handle.l3.buff = l3_buffer;
    lt_handle.l3.buff_len = sizeof(l3_buffer);
#endif

    // Device mappings
    // Initialize device before handing handle to the test.
    lt_dev_posix_tcp_t device;
    device.addr = inet_addr("127.0.0.1");
    device.port = 28992;
    lt_handle.l2.device = &device;

    // Generate a seed for the PRNG and seed it.
    unsigned int prng_seed;
    if (0 != getentropy(&prng_seed, sizeof(prng_seed))) {
        LT_LOG_ERROR("main: getentropy() failed (%s)!", strerror(errno));
        return -1;
    }
    srand(prng_seed);
    LT_LOG_INFO("PRNG initialized with seed=%u\n", prng_seed);

    // CAL context (selectable)
#if LT_USE_TREZOR_CRYPTO
    lt_ctx_trezor_crypto_t
#elif LT_USE_MBEDTLS_V4
    lt_ctx_mbedtls_v4_t
#endif
        crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    // Test code (correct test function is selected automatically per binary)
    // __lt_handle__ identifier is used by the test registry.
    lt_handle_t *__lt_handle__ = &lt_handle;
#include "lt_test_registry.c.inc"

#if LT_USE_MBEDTLS_V4
    mbedtls_psa_crypto_free();
#endif

    return 0;
}