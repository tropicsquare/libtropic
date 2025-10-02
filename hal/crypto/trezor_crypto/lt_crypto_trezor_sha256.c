/**
 * @file lt_crypto_trezor_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#if LT_CRYPTO_TREZOR
#include <stdint.h>
#include <string.h>

#include "hasher.h"
#include "lt_sha256.h"

void lt_sha256_init(void *ctx)
{
    Hasher *h = (Hasher *)ctx;
    memset(h, 0, sizeof(Hasher));
}

void lt_sha256_start(void *ctx)
{
    Hasher *h = (Hasher *)ctx;
    hasher_InitParam(h, HASHER_SHA2, NULL, 0);
}

void lt_sha256_update(void *ctx, const uint8_t *input, size_t len)
{
    Hasher *h = (Hasher *)ctx;
    hasher_Update(h, input, len);
}

void lt_sha256_finish(void *ctx, uint8_t *output)
{
    Hasher *h = (Hasher *)ctx;
    hasher_Final(h, output);
}
#endif
