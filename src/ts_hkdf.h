#ifndef TS_HKDF_H
#define TS_HKDF_H

/**
* @file   ts_hkdf.h
* @brief  HKDF function declaration
* @author Tropic Square s.r.o.
*/

#include <stdint.h>
#include <stdbool.h>

/**
 * @details The HMAC key derivation function as described in datasheet
 *
 * @param ck          CK parameter
 * @param ck_size      Length of CK parameter
 * @param input       Input data
 * @param input_size  Size of input data
 * @param nouts       Number of outputs
 * @param output_1    Output data 1
 * @param output_2    Output data 2
 */
void ts_hkdf(uint8_t *ck, uint32_t ck_size, uint8_t *input, uint32_t input_size, uint8_t nouts, uint8_t *output_1, uint8_t *output_2);

#endif
