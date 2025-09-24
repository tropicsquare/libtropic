#ifndef LT_LIBTROPIC_MACROS
#define LT_LIBTROPIC_MACROS

/**
 * @file libtropic_macros.h
 * @brief Various helper macros.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.md file in the root directory of this source tree.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Wrapper for static assertion. */
// #define LT_STATIC_ASSERT(x) _Static_assert((x), "Static assertion failed");
#define LT_STATIC_ASSERT(x)

/** @brief Get struct member size at compile-time. */
#define LT_MEMBER_SIZE(type, member) (sizeof(((type *)0)->member))

/** @brief Mark variable as unused to sanitize compiler warnings. */
#define LT_UNUSED(x) (void)(x)

#ifdef __cplusplus
}
#endif

#endif  // LT_LIBTROPIC_MACROS