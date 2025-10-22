/**
 * @file lt_mbedtls_hmac_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "mbedtls/md.h"
#include "libtropic_common.h"
#include "lt_hmac_sha256.h"

lt_ret_t lt_hmac_sha256(const uint8_t *key, const uint32_t key_len, const uint8_t *input, const uint32_t input_len,
                        uint8_t *output)
{
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (md_info == NULL) {
        return LT_CRYPTO_ERR;
    }

    int ret = mbedtls_md_hmac(md_info, key, key_len, input, input_len, output);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
