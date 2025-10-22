#ifndef LT_ECDSA_H
#define LT_ECDSA_H

/**
 * @file lt_ecdsa.h
 * @brief ECDSA function declarations.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Verifies ECDSA signature.
 *
 * @param msg        Message to be verified
 * @param msg_len    Length of the message
 * @param pubkey     Signer's public key (64B)
 * @param rs         R and S part of the message's signature (64B)
 * @return           LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecdsa_sign_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_ECDSA_H
