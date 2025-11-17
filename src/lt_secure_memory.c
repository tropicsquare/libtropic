/**
 * @file lt_secure_memory.c
 * @brief Secure memory operations (clearing sensitive data)
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_secure_memory.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void lt_secure_zero(void *ptr, size_t len)
{
    if (ptr == NULL || len == 0) {
        return;
    }

#if defined(__STDC_LIB_EXT1__)
    // C11 Annex K - preferred if available
    memset_s(ptr, len, 0, len);
#elif defined(__unix__) || defined(__APPLE__)
    // POSIX explicit_bzero
    explicit_bzero(ptr, len);
#elif defined(_MSC_VER)
    // Microsoft SecureZeroMemory
    SecureZeroMemory(ptr, len);
#else
    // Fallback: volatile pointer prevents optimization
    // This is portable and guaranteed to work even with aggressive optimization
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
#endif
}
