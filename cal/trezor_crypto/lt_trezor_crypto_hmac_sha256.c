/**
 * @file lt_trezor_crypto_hmac_sha256.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <stdint.h>

#include "hmac.h"
#include "libtropic_common.h"
#include "lt_hmac_sha256.h"

lt_ret_t lt_hmac_sha256(const uint8_t *key, const uint32_t key_len, const uint8_t *input, const uint32_t input_len,
                        uint8_t *output)
{
    hmac_sha256(key, key_len, input, input_len, output);
    return LT_OK;
}