/**
 * @file lt_crypto_trezor_ed25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifdef LT_USE_TREZOR_CRYPTO
#include <stdint.h>

#include "ed25519-donna/ed25519-donna.h"
#include "ed25519-donna/ed25519.h"
#include "ed25519-donna/ed25519-hash-custom.h"
#include "rand.h"
#include "memzero.h"
#include "lt_x25519.h"

int lt_ed25519_sign_open(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    return ed25519_sign_open(msg, msg_len, pubkey, rs);
}

#endif
