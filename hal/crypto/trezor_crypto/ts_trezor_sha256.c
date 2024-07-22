#ifdef USE_TS_CRYPTO
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "memzero.h"
#include "hasher.h"
#include "sha2.h"
#include "hmac.h"
#include "ts_sha256.h"

void ts_sha256_init(void* ctx)
{
    Hasher *h = (Hasher *) ctx;
    memset(h, 0, sizeof(Hasher));
}

void ts_sha256_start(void *ctx)
{
    Hasher *h = (Hasher *) ctx;
    hasher_InitParam(h, HASHER_SHA2, NULL, 0);
}

void ts_sha256_update(void *ctx, const uint8_t *input, size_t len)
{
    Hasher *h = (Hasher *) ctx;
    hasher_Update(h, input, len);
}

void ts_sha256_finish(void * ctx, uint8_t *output)
{
    Hasher *h = (Hasher *) ctx;
    hasher_Final(h, output);
}

void ts_hmac_sha256( const uint8_t *key, size_t keylen,
                          const uint8_t *input, size_t ilen,
                          uint8_t *output )
{
    hmac_sha256(key, keylen, input, ilen, output);
}
#endif