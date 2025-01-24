#ifndef HELLO_WORLD_EXAMPLE
#define HELLO_WORLD_EXAMPLE

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
/**
 * @brief  This is example of using TROPIC01 in a generic hardware wallet project
 *
 * @return int        Returns 0 if all commands were executed successfuly, otherwise returns 1
 */
int tropic01_hello_world_example(void);

#endif
