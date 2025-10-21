#ifndef LT_LIBTROPIC_MACROS
#define LT_LIBTROPIC_MACROS

/**
 * @file libtropic_macros.h
 * @brief Various helper macros.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
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

/** @brief Get max value.
 *  @details This macro uses "statement expressions", which prevent double evaluation in contrast
 *           to standard macros.
 *  @note This uses GCC/Clang compatible extension.
 */
#define lt_max(a, b)               \
    __extension__({                \
        __typeof__(a) ___a = (a);  \
        __typeof__(b) ___b = (b);  \
        ___a > ___b ? ___a : ___b; \
    })

#define lt_min(a, b)               \
    __extension__({                \
        __typeof__(a) ___a = (a);  \
        __typeof__(b) ___b = (b);  \
        ___a < ___b ? ___a : ___b; \
    })

/**
 * @brief Get max value from compile-time constants at compile-time.
 */
#define LT_COMPTIME_MAX(a, b) __builtin_choose_expr((a) > (b), a, b)

#ifdef __cplusplus
}
#endif

#endif  // LT_LIBTROPIC_MACROS