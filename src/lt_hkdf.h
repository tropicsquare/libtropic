#ifndef LT_HKDF_H
#define LT_HKDF_H

/**
 * @file   lt_hkdf.h
 * @brief  HKDF function declaration
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stdint.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The HMAC key derivation function as described in TROPIC01 datasheet.
 *
 * @param ck          CK parameter
 * @param ck_len      Length of CK parameter
 * @param input       Input data
 * @param input_len   Length of input data
 * @param nouts       Number of outputs
 * @param output_1    Output data 1
 * @param output_2    Output data 2
 */
lt_ret_t lt_hkdf(const uint8_t *ck, const uint32_t ck_len, const uint8_t *input, const uint32_t input_len,
                 const uint8_t nouts, uint8_t *output_1, uint8_t *output_2) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_HKDF_H
