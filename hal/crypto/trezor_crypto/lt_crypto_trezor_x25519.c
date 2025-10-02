/**
 * @file lt_crypto_trezor_x25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#if LT_CRYPTO_TREZOR
#include <stdint.h>

#include "ed25519-donna/ed25519-donna.h"
#include "ed25519-donna/ed25519-hash-custom.h"
#include "ed25519-donna/ed25519.h"
#include "lt_x25519.h"
#include "memzero.h"
#include "rand.h"

void lt_X25519(const uint8_t *priv, const uint8_t *pub, uint8_t *secret) { curve25519_scalarmult(secret, priv, pub); }

void lt_X25519_scalarmult(const uint8_t *sk, uint8_t *pk) { curve25519_scalarmult_basepoint(pk, sk); }

#endif
