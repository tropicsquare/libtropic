/**
 * @file lt_mbedtls_v4_ed25519.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "ed25519.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "lt_ed25519.h"

lt_ret_t lt_ed25519_sign_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    if (ed25519_verify(rs, msg, msg_len, pubkey)) {
        return LT_OK;
    }
    else {
        LT_LOG_ERROR("ED25519 signature verification failed!");
        return LT_CRYPTO_ERR;
    }
}
