#ifndef LT_LIBTROPIC_MACROS
#define LT_LIBTROPIC_MACROS

/**
 * @file libtropic_macros.h
 * @brief Various helper macros.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.md file in the root directory of this source tree.
 */

#ifdef __cplusplus
extern "C" {
#endif

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

/** @brief Get min value.
 *  @details This macro uses "statement expressions", which prevent double evaluation in contrast
 *           to standard macros.
 *  @note This uses GCC/Clang compatible extension.
 */
#define lt_min(a, b)               \
    __extension__({                \
        __typeof__(a) ___a = (a);  \
        __typeof__(b) ___b = (b);  \
        ___a < ___b ? ___a : ___b; \
    })

/**
 * @brief Macro for applying GNU/Clang printf format attributes.
 *
 * Enables compile-time format string validation for custom print-like functions.
 * Expands to `__attribute__((format(printf, fmt, args)))` on supported compilers,
 * or evaluates to empty on unsupported compilers (e.g., MSVC).
 *
 * @param fmt 1-based index of the format string parameter.
 * @param args 1-based index of the first variadic argument (`...`).
 */
#if defined(__GNUC__) || defined(__clang__)
#define LT_PRINTF_ATTR(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define LT_PRINTF_ATTR(fmt, args) /* Evaluates to empty on MSVC / other compilers */
#endif

#ifndef __cplusplus

/**
 * @brief Get max value from compile-time constants at compile-time.
 * @note C-only version.
 */
#define LT_COMPTIME_MAX(a, b) __builtin_choose_expr((a) > (b), a, b)

/** @brief Wrapper for static assertion.
 *  @note C-only version.
 */
#define LT_STATIC_ASSERT(x) _Static_assert((x), "Static assertion failed");

#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * @brief Get max value from compile-time constants at compile-time.
 * @note C++-only version.
 */
template <typename T>
constexpr T LT_COMPTIME_MAX(T a, T b)
{
    return (a > b) ? a : b;
}

/** @brief Wrapper for static assertion.
 *  @note C++-only version.
 */
#define LT_STATIC_ASSERT(x) static_assert((x), "Static assertion failed");

#endif

#endif  // LT_LIBTROPIC_MACROS
