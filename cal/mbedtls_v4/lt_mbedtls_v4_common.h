#ifndef LT_MBEDTLS_V4_COMMON_H
#define LT_MBEDTLS_V4_COMMON_H

/**
 * @file lt_mbedtls_v4_common.h
 * @brief Common declarations used with MbedTLS v4.0.0.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>

#include "libtropic_common.h"

/** @brief PSA Crypto initialization state */
extern bool lt_mbedtls_psa_crypto_initialized;

#endif  // LT_MBEDTLS_V4_COMMON_H
