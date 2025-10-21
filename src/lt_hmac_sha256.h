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

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Length of the hash produced by HMAC function based on SHA256.
 */
#define LT_HMAC_SHA256_HASH_LEN 32

/**
 * @brief Computes the HMAC SHA256 algorithm.
 *
 * @param key        Key data buffer
 * @param key_len    Length of data in key buffer
 * @param input      Input data buffer
 * @param input_len  Length of data in input buffer
 * @param output     Output buffer
 * @return           LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_hmac_sha256(const uint8_t *key, const uint32_t key_len, const uint8_t *input, const uint32_t input_len,
                        uint8_t *output) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_HMAC_SHA256_H
