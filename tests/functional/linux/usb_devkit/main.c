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

    ////////////////////////////////////////////////////////////////
    // CRYPTOGRAPHIC FUNCTION PROVIDER INITIALIZATION             //
    //                                                            //
    // In production, this would typically be done only once,     //
    // usually at the start of the application or before          //
    // the first use of cryptographic functions but no later than //
    // the first occurrence of any Libtropic function             //
    ////////////////////////////////////////////////////////////////
#if LT_USE_MBEDTLS_V4
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("PSA Crypto initialization failed, status=%d (psa_status_t)", status);
        return -1;
    }
#endif

    ////////////////////////////////////////////////////////////////
    // HANDLE INITIALIZATION                                      //
    //                                                            //
    // Libtropic handle is declared here (on stack) for           //
    // simplicity. In production, you put it on heap if needed.   //
    ////////////////////////////////////////////////////////////////
    lt_handle_t __lt_handle__ = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    __lt_handle__.l3.buff = l3_buffer;
    __lt_handle__.l3.buff_len = sizeof(l3_buffer);
#endif

    ////////////////////////////////////////////////////////////////
    // DEVICE MAPPINGS                                            //
    //                                                            //
    // Modify this according to your environment. Usually you     //
    // have to change at least the dev_path, as it is dynamically //
    // assigned by the OS (if you have multiple USB serial        //
    // devices).                                                  //
    ////////////////////////////////////////////////////////////////
    lt_dev_posix_usb_dongle_t device = {0};
    strcpy(device.dev_path, "/dev/ttyACM0");
    device.baud_rate = 115200;
    __lt_handle__.l2.device = &device;

    ////////////////////////////////////////////////////////////////
    // CRYPTO ABSTRACTION LAYER CONTEXT                           //
    //                                                            //
    // Context for the selected CAL implementation is chosen here //
    // based on the configuration macro. This is only for         //
    // convenient switching between different CALs for demo       //
    // purposes, in production applications you would typically   //
    // stick to a single CAL.                                     //
    ////////////////////////////////////////////////////////////////
#if LT_USE_TREZOR_CRYPTO
    lt_ctx_trezor_crypto_t
#elif LT_USE_MBEDTLS_V4
    lt_ctx_mbedtls_v4_t
#endif
        crypto_ctx;
    __lt_handle__.l3.crypto_ctx = &crypto_ctx;

    ////////////////////////////////////////////////////////////////
    // TEST CODE                                                  //
    ////////////////////////////////////////////////////////////////
#include "lt_test_registry.c.inc"

    ////////////////////////////////////////////////////////////////
    // CRYPTOGRAPHIC FUNCTION PROVIDER DEINITIALIZATION           //
    //                                                            //
    // In production, this would be done only once, typically     //
    // during termination of the application.                     //
    ////////////////////////////////////////////////////////////////
#if LT_USE_MBEDTLS_V4
    mbedtls_psa_crypto_free();
#endif

    return ret;
}