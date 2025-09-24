#ifndef LT_ECDSA_H
#define LT_ECDSA_H

/**
 * @file lt_ecdsa.h
 * @brief ECDSA function declarations.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Checks if ECDSA signature is correct
 *
 * @param msg        Message to be checked
 * @param msg_len    Length of the message
 * @param pubkey     Signer's public key (64B)
 * @param rs         R and S part of the message's signature (64B)
 * @return int       0 if signature is valid, otherwise 1
 */
int lt_ecdsa_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_ECDSA_H
