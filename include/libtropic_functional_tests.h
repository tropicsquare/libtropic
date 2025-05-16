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

// Only info-level loggers and decorators.
// This has no effect, test runner just simply copies these lines to the log.
#define LT_LOG(f_, ...) LT_LOG_INFO(f_, ##__VA_ARGS__)
#define LT_LOG_RESULT(f_, ...) LT_LOG_INFO("  result: "f_, ##__VA_ARGS__)
#define LT_LOG_VALUE(f_, ...) LT_LOG_INFO("\t\t- "f_, ##__VA_ARGS__)
#define LT_LOG_LINE(f_, ...) LT_LOG_INFO("\t-------------------------------------------------------------------------------------------------------------"f_, ##__VA_ARGS__)

// Loggers with selectable message type.
#define LT_LOG_INFO(f_, ...) printf("%d\t;INFO;"f_"\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_WARN(f_, ...) printf("%d\t;WARNING;"f_"\r\n", __LINE__, ##__VA_ARGS__)
#define LT_LOG_ERROR(f_, ...) printf("%d\t;ERROR;"f_"\r\n", __LINE__, ##__VA_ARGS__)
// Logger for system messages -- e.g. finishing a test.
#define LT_LOG_SYSTEM(f_, ...) printf("%d\t;SYSTEM;"f_"\r\n", __LINE__, ##__VA_ARGS__)

// Assertions. Will log as a system message.
#define LT_ASSERT(expected, value)                              \
                    if(value == expected)                       \
                    {                                           \
                        LT_LOG_SYSTEM("ASSERT_OK");             \
                    } else {                                    \
                        LT_LOG_SYSTEM("ASSERT_FAIL %d", value);           \
                    };

#define LT_ASSERT_COND(value, condition, expected_if_true, expected_if_false)                      \
                    if (value == (condition ? expected_if_true : expected_if_false)) {             \
                        LT_LOG_SYSTEM("ASSERT_OK");                                                \
                    } else {                                                                       \
                        LT_LOG_SYSTEM("ASSERT_FAIL");                                              \
                    }
// Used to stop the test. Will log as a system message.
#define LT_FINISH_TEST() LT_LOG_SYSTEM("TEST_FINISH")

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
