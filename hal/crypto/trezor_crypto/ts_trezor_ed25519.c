#ifdef USE_TS_CRYPTO
#include <stdint.h>

#include "ed25519-donna.h"
#include "ed25519.h"
#include "ed25519-hash-custom.h"
#include "rand.h"
#include "memzero.h"
#include "ts_x25519.h"

void ts_ed25519_sign_open(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    ed25519_sign_open(msg, msg_len, pubkey, rs);
}

#endif
