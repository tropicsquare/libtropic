/**
 * @file lt_trezor_crypto_ed25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "ed25519-donna/ed25519.h"
#include "lt_ed25519.h"

lt_ret_t lt_ed25519_sign_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    int ret = ed25519_sign_open(msg, msg_len, pubkey, rs);
    if (ret != RETURN_GOOD) {
        return LT_FAIL;
    }
    return LT_OK;
}