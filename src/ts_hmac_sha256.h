#ifndef TS_HMAC_SHA256_H
#define TS_HMAC_SHA256_H

/**
* @file   ts_hmac_sha256.h
* @brief  HMAC SHA256 function declarations
* @author Tropic Square s.r.o.
*/

#include <stddef.h>
#include <stdint.h>

/**
 * @details This function computes HMAC SHA256 algorithm
 *
 * @param key     Key data buffer
 * @param keylen  Length of data in key data buffer
 * @param input   Input data buffer
 * @param ilen    Length of data in input data buffer
 * @param output  Output buffer
 */
void ts_hmac_sha256( const uint8_t *key, size_t keylen,
                          const uint8_t *input, size_t ilen,
                          uint8_t *output );

#endif
