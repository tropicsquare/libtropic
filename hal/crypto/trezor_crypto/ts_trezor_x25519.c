#ifdef USE_TS_CRYPTO
#include <stdint.h>

#include "ed25519-donna.h"
#include "ed25519.h"
#include "ed25519-hash-custom.h"
#include "rand.h"
#include "memzero.h"
#include "ts_x25519.h"

void ts_X25519(const uint8_t *priv, const uint8_t *pub, uint8_t *secret)
{
    curve25519_scalarmult(secret, priv, pub);
}

void ts_X25519_scalarmult(const uint8_t *sk, uint8_t *pk)
{
    curve25519_scalarmult_basepoint(pk,sk);
}

#endif