#ifndef LT_SECURE_MEMORY_H
#define LT_SECURE_MEMORY_H

/**
 * @file lt_secure_memory.h
 * @brief Secure memory operations
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Securely zero memory (cannot be optimized away by compiler)
 * 
 * This function clears sensitive data from memory in a way that prevents
 * compiler optimizations from removing the operation. Use this instead of
 * memset() when clearing cryptographic keys, passwords, or other sensitive data.
 * 
 * The implementation uses platform-specific secure zeroing functions when
 * available (memset_s, explicit_bzero, SecureZeroMemory), or falls back to
 * a volatile pointer technique that is portable and standards-compliant.
 * 
 * @param ptr Pointer to memory to clear
 * @param len Number of bytes to clear
 * 
 * @note This function is safe to call with NULL pointer (no-op)
 * @note For maximum security, ensure this function is not inlined at call sites
 */
void lt_secure_zero(void *ptr, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LT_SECURE_MEMORY_H */
