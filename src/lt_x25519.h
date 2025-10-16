#ifndef LT_X25519_H
#define LT_X25519_H

/**
 * @file lt_x25519.h
 * @brief X25519 function declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes x25519 shared secret.
 *
 * @param  privkey   Private key (32B)
 * @param  pubkey    Public key (32B)
 * @param  secret    Shared secret (32B)
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_X25519(const uint8_t *privkey, const uint8_t *pubkey, uint8_t *secret) __attribute__((warn_unused_result));

/**
 * @brief Performs X25519 scalar multiplication with a base point.
 *
 * @param sk Secret key
 * @param pk Public key
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_X25519_scalarmult(const uint8_t *sk, uint8_t *pk) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_X25519_H
