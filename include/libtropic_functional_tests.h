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
 * @brief Test Pairing_Key_Read and Pairing_Key_Write on all slots.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0 and read it.
 *  2. Read pairing key slots 1,2,3 and check they are empty.
 *  3. Write pairing key slots 1,2,3.
 *  4. Read all pairing key slots and check for expected value.
 *  5. Write zeros to all pairing key slots and check for failure.
 */
void lt_test_read_write_pairing_keys_irreversible(void);

/**
 * @brief Test reading Chip ID and parse it.
 *
 * Test steps:
 *  1. Get device Chip ID.
 *  2. Parse it and print it.
 */
void lt_test_read_chip_id_reversible(void);

/**
 * @brief Reads contents of R-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole R-config.
 */
void lt_test_read_r_config_reversible(void);

/**
 * @brief Reads contents of I-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole I-config.
 */
void lt_test_read_i_config_reversible(void);

/**
 * @brief Read Certificate Store with 4 certificates and print it to log.
 *
 * Test steps:
 *  1. Get device Certificate Store.
 *  2. For each of the 4 certificates, check if its size is not zero and print it.
 */
void lt_test_read_cert_store_reversible(void);

/**
 * @brief Test function which does Ping command with all pairing keys
 *
 * @return int
 */
int lt_test_ping(void);

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
void lt_test_erase_r_config_reversible(void);

/**
 * @brief Test Secure Session initialization using handshake request and abortion of the Session.
 *
 * This function verifies that:
 *  1. Secure Session can be estabilished and aborted.
 *  2. Secure Session can be estabilished multiple times without aborting a previous existing Session.
 *  3. Secure Session can be aborted multiple times, even though there is no session.
 */
void lt_test_handshake_req_reversible(void);

/**
 * @brief Test Resend_Req L2 request.
 *
 * Test steps:
 * 1. Send Get_Info_Req and receive response frame, store the frame.
 * 2. Request a resend of the last L2 frame and store it.
 * 3. Contents of the original and resended frames are compared. This will check whether
 *    the resend works as intended.
 */
void lt_test_resend_req_reversible(void);

#endif
