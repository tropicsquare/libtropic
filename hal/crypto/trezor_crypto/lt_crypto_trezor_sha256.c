/**
 * @file lt_crypto_trezor_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifdef LT_USE_TREZOR_CRYPTO
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "memzero.h"
#include "hasher.h"
#include "sha2.h"
#include "hmac.h"
#include "lt_sha256.h"

void lt_sha256_init(void* ctx)
{
    Hasher *h = (Hasher *) ctx;
    memset(h, 0, sizeof(Hasher));
}

void lt_sha256_start(void *ctx)
{
    Hasher *h = (Hasher *) ctx;
    hasher_InitParam(h, HASHER_SHA2, NULL, 0);
}

void lt_sha256_update(void *ctx, const uint8_t *input, size_t len)
{
    Hasher *h = (Hasher *) ctx;
    hasher_Update(h, input, len);
}

void lt_sha256_finish(void * ctx, uint8_t *output)
{
    Hasher *h = (Hasher *) ctx;
    hasher_Final(h, output);
}

void lt_hmac_sha256( const uint8_t *key, size_t keylen,
                          const uint8_t *input, size_t ilen,
                          uint8_t *output )
{
    hmac_sha256(key, keylen, input, ilen, output);
}
#endif
