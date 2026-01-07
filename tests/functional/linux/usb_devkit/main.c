/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <string.h>
#include <time.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port_posix_usb_dongle.h"

#if LT_USE_TREZOR_CRYPTO
#include "libtropic_trezor_crypto.h"
#elif LT_USE_MBEDTLS_V4
#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"
#endif

int main(void)
{
    int ret = 0;

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
    lt_dev_posix_usb_dongle_t device = {0};
    strcpy(device.dev_path, "/dev/ttyACM0");
    device.baud_rate = 115200;
    lt_handle.l2.device = &device;

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

    return ret;
}