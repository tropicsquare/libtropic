#ifndef TS_X25519_H
#define TS_X25519_H

/**
* @file ts_x25519.h
* @brief X25519 function declarations
* @author Tropic Square s.r.o.
*/

#include "stdint.h"

/**
 * @details This function computes x25519 shared secret
 * @param priv   Private key 32B long
 * @param pub    Public key 32B long
 * @param secret Shared secret 32B long
 */
void ts_X25519(const uint8_t *priv, const uint8_t *pub, uint8_t *secret);

/**
 * @brief X25519 scalar multiplication with a base point
 *
 * @param sk Secret key
 * @param pk Public key
 */
void ts_X25519_scalarmult(const uint8_t *sk, uint8_t *pk);

#endif
