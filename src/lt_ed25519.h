#ifndef LT_ED25519_H
#define LT_ED25519_H

/**
 * @file lt_ed25519.h
 * @brief ED25519 function declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "stdint.h"

/**
 * @brief  Checks if ed25519 signature is correct
 *
 * @param msg        Message to be checked
 * @param msg_len    Length of the message
 * @param pubkey     Signer's public key
 * @param rs         R and S part of the message's signature
 * @return int       0 if signature is valid, otherwise 1
 */
int lt_ed25519_sign_open(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

#endif
