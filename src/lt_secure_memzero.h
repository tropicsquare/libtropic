#ifndef LT_SECURE_MEMZERO_H
#define LT_SECURE_MEMZERO_H

/**
 * @file lt_secure_memzero.h
 * @brief Declarations for secure memory zeroing.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <stddef.h>

#include "libtropic_common.h"

/**
 * @brief Securely overwrites the memory with zeros.
 *
 * @param ptr    Pointer to the memory
 * @param count  Number of bytes to overwrite with zeros
 */
void lt_secure_memzero(void *const ptr, size_t count);

#endif  // LT_SECURE_MEMZERO_H