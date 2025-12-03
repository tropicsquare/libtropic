/**
 * @file lt_trezor_crypto_x25519.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#include "ed25519-donna/ed25519.h"
#include "libtropic_common.h"
#include "lt_x25519.h"

lt_ret_t lt_X25519(const uint8_t *priv, const uint8_t *pub, uint8_t *secret)
{
    curve25519_scalarmult(secret, priv, pub);
    return LT_OK;
}

lt_ret_t lt_X25519_scalarmult(const uint8_t *sk, uint8_t *pk)
{
    curve25519_scalarmult_basepoint(pk, sk);
    return LT_OK;
}