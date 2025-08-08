#ifndef LT_LIBTROPIC_MACROS
#define LT_LIBTROPIC_MACROS

/**
 * @file libtropic_macros.h
 * @brief Various helper macros.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.md file in the root directory of this source tree.
 */

#define STATIC_ASSERT(x) _Static_assert((x), "Static assertion failed");

// Get struct member size at compile-time.
#define MEMBER_SIZE(type, member) (sizeof(((type *)0)->member))

#endif  // LT_LIBTROPIC_MACROS