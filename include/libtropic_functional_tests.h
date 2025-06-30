#ifndef LT_LIBTROPIC_FUNC_TESTS_H
#define LT_LIBTROPIC_FUNC_TESTS_H

/**
 * @file libtropic_functional_tests.h
 * @brief Functions with functional tests used internally for testing behaviour of TROPIC01 chip
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
 * @brief Test function which writes pairing keys 1 2 and 3
 *
 * @return int
 */
int lt_test_write_pairing_keys(void);

/**
 * @brief Test function which reads and prints out CHIP ID
 *
 * @return int
 */
int lt_test_chip_id(void);

/**
 * @brief Test function which reads and prints out R-Config
 *
 * @return int
 */
int lt_test_dump_r_config(void);

/**
 * @brief Test function which reads and prints out I-Config
 *
 * @return int
 */
int lt_test_dump_i_config(void);

/**
 * @brief Test function which reads and prints whole certificate store
 *
 * @return int
 */
int lt_test_dump_cert_store(void);

/**
 * @brief Test function which does Ping command with all pairing keys
 *
 * @return int
 */
int lt_test_ping(void);

/**
 * @brief Test function which does Ping command with all pairing keys
 * against engineering sample 01
 *
 * @return int
 */
int lt_test_engineering_sample_01_ping(void);

/**
 * @brief Test function which writes r config
 *
 * @return int
 */
int lt_test_write_r_config(void);

/**
 * @brief Test function which tests all ecc EdDSA commands on all ecc keys lots
 *
 * @return int
 */
int lt_test_ecc_eddsa(void);

/**
 * @brief Test function which tests all ecc ECDSA commands on all ecc keys lots
 *
 * @return int
 */
int lt_test_ecc_ecdsa(void);

/**
 * @brief Test function which tests reading,writing and erasing of all r mem slots
 *
 * @return int
 */
int lt_test_r_mem(void);

/**
 * @brief Test function which erases R config
 *
 * @return int
 */
int lt_test_erase_r_config(void);

#endif
