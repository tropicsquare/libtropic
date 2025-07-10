#ifndef LT_LIBTROPIC_EXAMPLES_H
#define LT_LIBTROPIC_EXAMPLES_H

/**
 * @file libtropic_examples.h
 * @brief Functions with examples of usage of TROPIC01 chip
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

// Default factory pairing keys
extern uint8_t sh0priv[];
extern uint8_t sh0pub[];
// Keys with acces to write attestation key in slot 1
extern uint8_t sh1priv[];
extern uint8_t sh1pub[];
// Keys with access only to read serial number
extern uint8_t sh2priv[];
extern uint8_t sh2pub[];
// Keys for application
extern uint8_t sh3priv[];
extern uint8_t sh3pub[];

/**
 * @brief Example function, Hello World
 *
 *  Verifies chip's certificate, establishes secure channel and executes Ping l3 command.
 *  TODO explain more
 * @return int
 */
int lt_ex_hello_world(void);

/**
 * @brief Example function, Hello World with separate API
 *
 * Verifies chip's certificate, establishes secure channel and executes Ping l3 command.
 * TODO explain more
 * @return int
 */

/**
 * @brief Establishes Secure Session and executes Ping L3 command using separated API.
 *
 * This example shows how to use separated API calls with TROPIC01. Separate calls are named lt_out__* and
 * lt_in__* and they provide splitting of the commands/results, which might be used for example for communication over a
 * tunnel. Let's say we want to speak with TROPIC01 from a server, then lt_out__* part is done on the server, then
 * encrypted payload is transferred over tunnel to the point where SPI is wired to TROPIC01. L2 communication is
 * executed, encrypted result is transferred back to the server, where lt_in__* function is used to decrypt the
 * response.
 *
 * To have a better understanding have a look into lt_ex_hello_world.c, both examples shows similar procedure.
 *
 * This might be used for example in production, where we want to establish a secure channel between HSM and TROPIC01 on
 * PCB.
 * @return -1 on fail, 0 otherwise
 */
int lt_ex_hello_world_separate_API(void);

/**
 * @brief Example function, Hardware Wallet
 * @details Pairing keys SH1-3 are set and SH0 is invalidated. TODO explain more
 * WARNING: This example ireversively writes into chip!
 *
 * @return int
 */
int lt_ex_hardware_wallet(void);

/**
 * @brief Example function, shows how to update TROPIC01's internal firmwares
 */
void lt_ex_fw_update(void);

/**
 * @brief Example usage of TROPIC01 flagship feature - 'Mac And Destroy' PIN verification engine.
 *
 * Value MACANDD_ROUNDS represents a number of possible PIN gueses, this value also affects size of lt_macandd_nvm_t
 * struct. Technically TROPIC01 is capable to have this set to 128, therefore provide 128 Mac And Destroy tries, which
 * would require roughly 128*32 bytes in non volatile memory for storing data related to M&D tries.
 *
 * In this example TROPIC01's R memory is used as a storage for data during power cycle.
 * For a sake of simplicity, only one R memory slot is used as a storage, which means 444B of storage are available.
 *
 * Therefore MACANDD_ROUNDS is here limited to 12 -> biggest possible number of tries which fits into 444B one R
 * memory slot.
 *
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 *
 * @return 0 on success, -1 otherwise
 */
int lt_ex_macandd(void);

#endif
