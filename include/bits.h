/**
 * @file bits.h
 * @brief Struct and function declarations for dealing with bit assignment.
 * @author Ondrej Ille
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifndef BITS_H
#define BITS_H

#define BITS_PER_LONG 32

// ## allows token concatenation
// X = 1 and Y = 10 would return 110
#define __AC(X,Y)   (X##Y)
#define _AC(X,Y)    __AC(X,Y)

#define _U(x)      (_AC(x, U))
#define U(x)       (_U(x))

#define BIT(nr) (1U << (nr))

#define BIT64(nr) (((u64)(1)) << ((u64)(nr)))

// BIT defines a bit mask for the specified bit number from 0 to whatever fits into an unsigned long
// so BIT(10) should evaluate to decimal 1024 (which is binary 1 left shifted by 10 bits)

#define GENMASK_INPUT_CHECK(h, l) 0

// h is high index, l is low index in a bitfield
// __GENMASK returns 32 bit number with 1s in the h-to-l field
// if h = 4 and l = 1, __GENMASK would return 00000000000000000000000000011110
#define __GENMASK(h, l) \
    (((~U(0)) - (U(1) << (l)) + 1) & \
     (~U(0) >> (BITS_PER_LONG - 1 - (h))))

#define GENMASK(h, l) \
    (GENMASK_INPUT_CHECK(h, l) + __GENMASK(h, l))

#define __bf_shf(x) (__builtin_ffsll(x) - 1)

/*
 * Example:
 *
 *  #define REG_FIELD_A  GENMASK(6, 0)
 *  #define REG_FIELD_B  BIT(7)
 *  #define REG_FIELD_C  GENMASK(15, 8)
 *  #define REG_FIELD_D  GENMASK(31, 16)
 *
 * Get:
 *  a = FIELD_GET(REG_FIELD_A, reg);
 *  b = FIELD_GET(REG_FIELD_B, reg);
 *
 * Set:
 *  reg = FIELD_PREP(REG_FIELD_A, 1) |
 *    FIELD_PREP(REG_FIELD_B, 0) |
 *    FIELD_PREP(REG_FIELD_C, c) |
 *    FIELD_PREP(REG_FIELD_D, 0x40);
 *
 * Modify:
 *    FIELD_SET(reg, REG_FIELD_D, 0x40);
 *
 * Note:
 *    FIELD_GET and FIELD_PREP are R-value expressions.
 *    FIELD_SET is statement.
 *    "({" "})" braces were removed since they are non-ISO C.
 */


#define FIELD_GET(_mask, _reg)                                  \
    (                                                           \
        (typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask))  \
    )

#define FIELD_PREP(_mask, _val)                                 \
    (                                                           \
        ((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask)    \
    )

#define FIELD_SET(_reg, _mask, _val)      \
    {                                     \
        _reg &= ~_mask;                   \
        _reg |= FIELD_PREP(_mask, _val);  \
    }

#endif // ! BITS_H
