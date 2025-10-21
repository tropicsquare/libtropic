#ifndef LT_ED25519_H
#define LT_ED25519_H

/**
 * @file lt_ed25519.h
 * @brief ED25519 function declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Verifies Ed25519 signature.
 *
 * @param msg        Message to be verified
 * @param msg_len    Length of the message
 * @param pubkey     Signer's public key
 * @param rs         R and S part of the message's signature
 * @return           LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ed25519_sign_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_ED25519_H
