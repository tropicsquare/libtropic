#ifndef LT_HMAC_SHA256_H
#define LT_HMAC_SHA256_H

/**
 * @file   lt_hmac_sha256.h
 * @brief  HMAC SHA256 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Length of the hash produced by HMAC function based on SHA256.
 */
#define LT_HMAC_SHA256_HASH_LEN 32

/**
 * @details This function computes HMAC SHA256 algorithm
 *
 * @param key     Key data buffer
 * @param keylen  Length of data in key data buffer
 * @param input   Input data buffer
 * @param ilen    Length of data in input data buffer
 * @param output  Output buffer
 */
void lt_hmac_sha256(const uint8_t *key, size_t keylen, const uint8_t *input, size_t ilen, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif // LT_HMAC_SHA256_H
