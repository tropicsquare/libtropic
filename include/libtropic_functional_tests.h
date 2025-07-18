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

extern void (*lt_test_cleanup_function)(void);

void lt_assert_fail_handler(void);

// Assertions -- special variant for functional tests.
// Will log as a system message and call native assert function.
// On error, they will also call a cleanup function, if not NULL. The pointer
// is usually assigned at the start of a functional test, if the test requires a cleanup
// on each error (and exit of course) to be truly reversible.
//
// Note that parameters are stored to _val_ and _exp_ for a case when there
// are function calls passed to the macros. Without the helper variables
// the functions will be called mutliple times -- in the first comparison
// (if statement), LT_LOG_ERROR and finally in the assert(). This can cause
// unexpected behaviour.
#define LT_TEST_ASSERT(expected, value)                                            \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (expected);                                                    \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
            lt_assert_fail_handler();                                              \
        };                                                                         \
        assert(_exp_ == _val_);                                                    \
    }

#define LT_TEST_ASSERT_COND(value, condition, expected_if_true, expected_if_false) \
    {                                                                              \
        int _val_ = (value);                                                       \
        int _exp_ = (condition ? expected_if_true : expected_if_false);            \
        if (_val_ == _exp_) {                                                      \
            LT_LOG_INFO("ASSERT PASSED!");                                         \
        }                                                                          \
        else {                                                                     \
            LT_LOG_ERROR("ASSERT FAILED! Got: '%d' Expected: '%d'", _val_, _exp_); \
            lt_assert_fail_handler();                                              \
        }                                                                          \
        assert(_exp_ == _val_);                                                    \
    }

// Used to instruct the test runner that the test finshed and may disconnect (useful in embedded ports).
#define LT_FINISH_TEST()               \
    {                                  \
        LT_LOG_INFO("TEST FINISHED!"); \
    }

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
 * @brief Test Pairing_Key_Read and Pairing_Key_Write on all slots.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0 and read it.
 *  2. Read pairing key slots 1,2,3 and check they are empty.
 *  3. Write pairing key slots 1,2,3.
 *  4. Read all pairing key slots and check for expected value.
 *  5. Write zeros to all pairing key slots and check for failure.
 */
void lt_test_ire_read_write_pairing_keys(void);

/**
 * @brief Test function which does Ping command with all pairing keys
 *
 * @return int
 */
int lt_test_rev_ping(void);

/**
 * @brief Test R_Mem_Data_* commands on all User Data slots
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Check if all slots are empty.
 *  3. Write all slots entirely with random data.
 *  4. Read all slots and check if they were written.
 *  5. Write 0x00 to all slots and check if it fails.
 *  6. Read all slots and check they were not written.
 *  7. Erase all slots and check that reading fails.
 *  8. Write all slots partially with random data of random length (0-443).
 *      - if the random length is 0, check that write fails.
 *  9. Read all slots and check if they were written.
 *      - if the random length is 0, check that read fails (slot empty).
 *  10. Erase all slots and check that reading fails.
 */
void lt_test_rev_r_mem(void);

/**
 * @brief Backs up R-Config, erases it and then restores it.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole R-Config and save it.
 *  3. Erase the R-Config and check it.
 *  4. Restore the R-Config and check it.
 */
void lt_test_rev_erase_r_config(void);

/**
 * @brief Test Secure Session initialization using handshake request and abortion of the Session.
 *
 * This function verifies that:
 *  1. Secure Session can be estabilished and aborted.
 *  2. Secure Session can be estabilished multiple times without aborting a previous existing Session.
 *  3. Secure Session can be aborted multiple times, even though there is no session.
 */
void lt_test_rev_handshake_req(void);

/**
 * @brief Read Certificate Store with 4 certificates and print it to log.
 *
 * Test steps:
 *  1. Get device Certificate Store.
 *  2. For each of the 4 certificates, check if its size is not zero and print it.
 */
void lt_test_rev_read_cert_store(void);

/**
 * @brief Test reading Chip ID and parse it.
 *
 * Test steps:
 *  1. Get device Chip ID.
 *  2. Parse it and print it.
 */
void lt_test_rev_read_chip_id(void);

/**
 * @brief Reads contents of I-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole I-config.
 */
void lt_test_rev_read_i_config(void);

/**
 * @brief Reads contents of R-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole R-config.
 */
void lt_test_rev_read_r_config(void);

/**
 * @brief Test Resend_Req L2 request.
 *
 * Test steps:
 * 1. Send Get_Info_Req and receive response frame, store the frame.
 * 2. Request a resend of the last L2 frame and store it.
 * 3. Contents of the original and resended frames are compared. This will check whether
 *    the resend works as intended.
 */
void lt_test_rev_resend_req(void);

/**
 * @brief Test function which writes r config
 *
 * @return int
 */
void lt_test_rev_write_r_config(void);

#endif
