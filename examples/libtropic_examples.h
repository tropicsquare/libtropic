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

// Default factory pairing keys
extern uint8_t pkey_index_0;
extern uint8_t sh0priv[];
extern uint8_t sh0pub[];
// Keys with acces to write attestation key in slot 1
extern uint8_t pkey_index_1;
extern uint8_t sh1priv[];
extern uint8_t sh1pub[];
// Keys with access only to read serial number
extern uint8_t pkey_index_2;
extern uint8_t sh2priv[];
extern uint8_t sh2pub[];
// Keys for application
extern uint8_t pkey_index_3;
extern uint8_t sh3priv[];
extern uint8_t sh3pub[];

#define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#define LOG_OUT_RESULT(f_, ...) printf("  result: "f_, ##__VA_ARGS__)


#define LOG_OUT_INFO(f_, ...) printf("\t[INFO] "f_, ##__VA_ARGS__)
#define LOG_OUT_VALUE(f_, ...) printf("\t\t- "f_, ##__VA_ARGS__)
#define LOG_OUT_LINE(f_, ...) printf("\t-------------------------------------------------------------------------------------------------------------\r\n"f_, ##__VA_ARGS__)

#define LOG_OUT_SESSION(f_, ...) printf(f_, ##__VA_ARGS__)

#define LT_ASSERT(func, expected)   \
                    if(func!=expected) \
                    { \
                        printf("\t\t\tERROR\r\n"); return -1; \
                    } else {printf(" %s\r\n", "(ok)");};

#define LT_PRINT_RES(func)   \
                    {printf("%d\r\n", func);}

int lt_ex_hello_world(void);
int lt_ex_hardware_wallet(void);
void lt_ex_test_reversible(void);
void lt_ex_test_ireversible(void);

#endif
