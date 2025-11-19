/**
 * @file lt_secure_memzero.c
 * @brief Implementation of secure memory zeroing.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_secure_memzero.h"

#include <stddef.h>
#include <string.h>
// explicit_bzero() may be declared in strings.h, so include it if it's available.
#ifdef LT_HAVE_STRINGS_H
#include <strings.h>
#endif

#include "libtropic_logging.h"

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif

void lt_secure_memzero(void *const ptr, const size_t count)
{
    if (!ptr) {
        LT_LOG_ERROR("lt_secure_memzero: ptr=NULL!");
    }

#if defined(_WIN32) || defined(__WIN32__)
    SecureZeroMemory(ptr, count);

#elif defined(LT_HAVE_MEMSET_EXPLICIT)
    memset_explicit(ptr, 0, count);

#elif defined(LT_HAVE_EXPLICIT_BZERO)
    explicit_bzero(ptr, count);

#elif defined(LT_HAVE_EXPLICIT_MEMSET)
    explicit_memset(ptr, 0, count);

#elif defined(LT_HAVE_MEMSET_S)
    if (0 != memset_s(ptr, count, 0, count)) {
        LT_LOG_ERROR("lt_secure_memzero: memset_s failed!");
    }

#else
#warning Zeroing of the memory is done using volatile pointers, which may not be safe.

    volatile unsigned char *volatile _ptr = (volatile unsigned char *volatile)ptr;
    for (size_t i = 0U; i < count; i++) {
        _ptr[i] = 0U;
    }
#endif
}