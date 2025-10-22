/**
 * @file lt_trezor_crypto_ecdsa.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "ecdsa.h"
#include "hasher.h"
#include "libtropic_common.h"
#include "lt_ecdsa.h"
#include "nist256p1.h"

lt_ret_t lt_ecdsa_sign_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    // Prepare pubkey with 0x04 prefix
    uint8_t pubkey_with_prefix[65];
    pubkey_with_prefix[0] = 0x04;
    memcpy(&pubkey_with_prefix[1], pubkey, 64);

    int ret = ecdsa_verify(&nist256p1, HASHER_SHA2, pubkey_with_prefix, rs, msg, msg_len);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}