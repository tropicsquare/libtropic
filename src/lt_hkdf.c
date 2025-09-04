/**
 * @file   lt_hkdf.c
 * @brief  HKDF functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#if LT_USE_TREZOR_CRYPTO
#include "blake256.h"
#include "blake2b.h"
#include "groestl.h"
#include "hasher.h"
#include "hmac.h"
#include "lt_hmac_sha256.h"
#include "memzero.h"
#include "ripemd160.h"
#include "sha2.h"
#include "sha3.h"
#elif USE_MBEDTLS
// TBD
#endif

#include "libtropic_macros.h"
#include "lt_hkdf.h"

void lt_hkdf(uint8_t *ck, uint32_t ck_size, uint8_t *input, uint32_t input_size, uint8_t nouts, uint8_t *output_1,
             uint8_t *output_2)
{
    LT_UNUSED(nouts);

    uint8_t tmp[32] = {0};
    uint8_t one = 0x01;

    lt_hmac_sha256(ck, ck_size, input, input_size, tmp);
    lt_hmac_sha256(tmp, 32, &one, 1, output_1);

    uint8_t helper[33] = {0};
    memcpy(helper, output_1, 32);
    helper[32] = 2;

    lt_hmac_sha256(tmp, 32, helper, 33, output_2);
}
