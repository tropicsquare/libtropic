/**
 * @file lt_crypto_trezor_hmac_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifdef LT_USE_TREZOR_CRYPTO
#include <stdint.h>

#include "hmac.h"
#include "lt_hmac_sha256.h"

void lt_hmac_sha256(const uint8_t *key, size_t keylen, const uint8_t *input, size_t ilen, uint8_t *output)
{
    hmac_sha256(key, keylen, input, ilen, output);
}
#endif
