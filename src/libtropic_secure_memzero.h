#ifndef LIBTROPIC_SECURE_MEMZERO_H
#define LIBTROPIC_SECURE_MEMZERO_H

/**
 * @file libtropic_secure_memzero.h
 * @brief Declarations for secure memory zeroing.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stddef.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Securely overwrites the memory with zeros.
 * @note This function is primarily used for internal purposes.
 *
 * @param ptr    Pointer to the memory
 * @param count  Number of bytes to overwrite with zeros
 */
void lt_secure_memzero(void *const ptr, size_t count);

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_SECURE_MEMZERO_H