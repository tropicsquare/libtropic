#ifndef HW_WALLET
#define HW_WALLET

/**
 * @file TROPIC01_hw_wallet.h
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <stdio.h>


#define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#define LOG_OUT_INFO(f_, ...) printf("\t[INFO] "f_, ##__VA_ARGS__)
#define LOG_OUT_VALUE(f_, ...) printf("\t\t- "f_, ##__VA_ARGS__)
#define LOG_OUT_LINE(f_, ...) printf("\t-------------------------------------------------------------------------------------------------------------\r\n"f_, ##__VA_ARGS__)

#define LOG_OUT_SESSION(f_, ...) printf("\t\t- "f_, ##__VA_ARGS__)

#define LT_ASSERT(func, expected)   \
                    if(func!=expected) \
                    { \
                        printf("\t\t\tERROR\r\n"); return -1; \
                    } else {printf(" %s\r\n", "(ok)");};


int tropic01_hw_wallet_example(void);

#endif
