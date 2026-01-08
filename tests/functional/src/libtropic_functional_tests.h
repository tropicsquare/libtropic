#ifndef LT_LIBTROPIC_FUNC_TESTS_H
#define LT_LIBTROPIC_FUNC_TESTS_H

/**
 * @defgroup libtropic_funct_tests 3. Functional Tests
 * @brief Test all libtropic main API functions.
 * @{
 */

/**
 * @file libtropic_functional_tests.h
 * @brief Functions with functional tests used internally for testing behaviour of TROPIC01 chip
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Tests EDDSA_Sign command.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Generate random message with random size for signing.
 *  3. Sign message with each empty slot and check for fail.
 *  4. Store pre-generated private key to each slot.
 *  5. Read the public key from each slot.
 *  6. Sign the message with each slot.
 *  7. Verify the signature.
 *  8. Erase each slot.
 *  9. Sign message with each erased slot and check for fail.
 *  10. Do steps 2-9, but instead of storing the key, generate it.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_eddsa_sign(lt_handle_t *h);

/**
 * @brief Tests ECDSA_Sign command.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Generate random message with random size for signing.
 *  3. Sign message with each empty slot and check for fail.
 *  4. Store pre-generated private key to each slot.
 *  5. Read the public key from each slot.
 *  6. Sign the message with each slot.
 *  7. Verify the signature.
 *  8. Erase each slot.
 *  9. Sign message with each erased slot and check for fail.
 *  10. Do steps 2-9, but instead of storing the key, generate it.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_ecdsa_sign(lt_handle_t *h);

/**
 * @brief Test Pairing_Key_Read, Pairing_Key_Write and Pairing_Key_Invalidate on all slots.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0 and read it.
 *  2. Read pairing key slots 1,2,3 and check they are empty.
 *  3. Write pairing key slots 1,2,3.
 *  4. Read all pairing key slots and check for expected value.
 *  5. Start Secure Session with slots 1,2,3 and abort.
 *  6. Start Secure Session with pairing key slot 0.
 *  7. Write zeros to all pairing key slots and check for failure.
 *  8. Invalidate all pairing key slots.
 *  9. Read and write all pairing key slots and check for failure.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_ire_pairing_key_slots(lt_handle_t *h);

/**
 * @brief Test Ping L3 command with random data of random length <= TR01_PING_LEN_MAX.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Generate random data length from the range 0-4096.
 *  3. Generate random bytes.
 *  4. Send Ping command with the random data.
 *  5. Check if the same data were received.
 *  6. Repeat steps 2-5 PING_MAX_LOOPS times.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_ping(lt_handle_t *h);

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
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_r_mem(lt_handle_t *h);

/**
 * @brief Backs up R-Config, erases it and then restores it.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole R-Config and save it.
 *  3. Erase the R-Config and check it.
 *  4. Restore the R-Config and check it.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_erase_r_config(lt_handle_t *h);

/**
 * @brief Test Secure Session initialization using handshake request and abortion of the Session.
 *
 * This function verifies that:
 *  1. Secure Session can be estabilished and aborted.
 *  2. Secure Session can be estabilished multiple times without aborting a previous existing Session.
 *  3. Secure Session can be aborted multiple times, even though there is no session.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_handshake_req(lt_handle_t *h);

/**
 * @brief Test monotonic counter API - lt_mcounter_init, lt_mcounter_get, lt_mcounter_update.
 *
 * Test steps:
 * 1. Try to init each counter with random value twice, then try a few decrements.
 * 2. Set each of the counters to random small value, then decrement to zero. Try decrement one more
 *    time after reaching 0 -- should fail with UPDATE_ERROR.
 * 3. Try to set all counters with known value and check that no counter was assigned
 *    wrong value.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_mcounter(lt_handle_t *h);

/**
 * @brief Test parameter checking in public API functions.
 *
 * Test steps:
 *  Try to call all public API functions with invalid parameters and check for expected error codes.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_param_check(lt_handle_t *h);

/**
 * @brief Test Get_Info_Req command in Application mode with all possible OBJECT_ID values.
 *
 * Test steps:
 *  1. Get device Certificate Store.
 *  2. For each of the 4 certificates, check if its size is not zero and print it.
 *  3. Get Chip ID and print it to log.
 *  4. Get RISC-V FW version and print it to log.
 *  5. Get SPECT FW version and print it to log.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_get_info_req_app(lt_handle_t *h);

/**
 * @brief Test Get_Info_Req command in Maintenance mode with all possible OBJECT_ID values.
 *
 * Test steps:
 *  1. Reboot to Maintenance mode.
 *  2. Get device Certificate Store.
 *  3. For each of the 4 certificates, check if its size is not zero and print it.
 *  4. Get Chip ID and print it to log.
 *  5. Get RISC-V bootloader version and print it to log.
 *  6. Get SPECT bootloader version, print it to log and check it's dummy.
 *  7. Read all FW banks and based on the bootloader version (1.0.1 or 2.0.1), print it to log.
 *  8. Reboot back to Application mode.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_get_info_req_bootloader(lt_handle_t *h);

/**
 * @brief Reads contents of I-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole I-config.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_read_i_config(lt_handle_t *h);

/**
 * @brief Tests writing the I-Config.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Write the whole I-Config with random data.
 *  3. Read the whole I-Config and check that it was written.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_ire_write_i_config(lt_handle_t *h);

/**
 * @brief Reads contents of R-Config and prints it to the log.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Read the whole R-config.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_read_r_config(lt_handle_t *h);

/**
 * @brief Test Resend_Req L2 request in Application and Maintenance mode.
 *
 * Test steps:
 * 1. Reboot into Application mode.
 * 2. Send Get_Info_Req and receive response frame, store the frame.
 * 3. Request a resend of the last L2 frame and store it.
 * 4. Contents of the original and resended frames are compared. This will check whether
 *    the resend works as intended.
 * 5. Reboot into Maintenance mode and do steps 2-4 again.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_resend_req(lt_handle_t *h);

/**
 * @brief Test Sleep_Req L2 request.
 *
 * Test steps:
 * 1. Start Secure Session.
 * 2. Send Sleep Req.
 * 3. Verify the Secure Session was terminated.
 * 4. Wake the chip up.
 *
 * @note There is no cleanup, as the chip does not have to be woken up. It is woken up automatically
 *       by any L2 request.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_sleep_req(lt_handle_t *h);

/**
 * @brief Test L2 Startup Request.
 *
 * Test steps:
 *  1. Reboot to normal mode and check.
 *  2. Reboot to normal mode again and check.
 *  3. Reboot to bootloader mode and check.
 *  4. Reboot to bootloader mode again and check.
 *  5. Reboot to normal mode and check.
 *
 * After each reboot before mode check, the test will wait until the chip is ready.
 * Mode check is done by reading SPECT FW version. The bootloader returns highest bit set to '1'
 * for each FW version.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_startup_req(lt_handle_t *h);

/**
 * @brief Backs up R-Config, writes it and then restores it.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Backup the whole R-Config.
 *  3. Erase the R-config.
 *  4. Write the whole R-Config with random data and check it.
 *  5. Write the whole R-Config again and check for an error.
 *  6. Restore the R-Config and check it.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_write_r_config(lt_handle_t *h);

/**
 * @brief Test ECC_Key_Generate command, along with ECC_Key_Read and ECC_Key_Erase.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Check if all slots are empty.
 *  3. In each slot, generate private keys using P256 curve.
 *  4. Try generating again, also using Ed25519 curve and check for errors.
 *  5. Read the generated public key and check curve and origin.
 *  6. Erase the key slot.
 *  7. Repeat steps 2-6 for the Ed25519 curve.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_ecc_key_generate(lt_handle_t *h);

/**
 * @brief Test ECC_Key_Store command, along with ECC_Key_Read and ECC_Key_Erase.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Check if all slots are empty.
 *  3. In each slot, store invalid private key using P256 curve and check for error.
 *  4. In each slot, store pre-generated private key using P256 curve.
 *  5. Try storing again, also using Ed25519 curve and check for errors.
 *  6. Read the stored public key and check curve and origin.
 *  7. Erase the key slot.
 *  8. Repeat steps 2-6 for the Ed25519 curve with pre-generated keys (without storing invalid private key - that cannot
 * be checked in the case of Ed25519).
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_ecc_key_store(lt_handle_t *h);

/**
 * @brief Tests Random_Value_Get command.
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Generate random number 0-255 using lt_random_bytes(), which will be
 *     used in the Random_Value_Get command.
 *  3. Get random count (from step 2) of random bytes from TROPIC01.
 *  4. Dump the random bytes from TROPIC01 into the log.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_random_value_get(lt_handle_t *h);

/**
 * @brief Tests MAC_And_Destroy command using HMAC-SHA256 as the Key Derivation Function (KDF).
 *
 * Test steps:
 *  1. Start Secure Session with pairing key slot 0.
 *  2. Setup random PIN with randomly generated number of attempts.
 *  3. Do a random number of wrong attempts (less than the maximum number of attempts).
 *  4. Do an attempt with the correct PIN with the first understroyed slot and compare the cryptographic key with the
 * one from the setup phase.
 *  5. Restore all destroyed slots.
 *  6. Do an attempt with the correct PIN with all the used slots and compare the cryptographic keys with the one from
 * the setup phase.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_mac_and_destroy(lt_handle_t *h);

/**
 * @brief Tests Get_Log_Req command in Application and Maintenance mode.
 *
 * Test steps:
 *  1. Reboot into Application mode.
 *  2. Start Secure Session with pairing key slot 0.
 *  3. Read CFG_DEBUG from I and R config to check if FW logging is enabled.
 *  4. Read the FW log (assert LT_OK if FW logging enabled, LT_L2_RESP_DISABLED otherwise).
 *  5. Print the FW log into the test log if enabled or it's length is not zero.
 *  6. Reboot into Maintenance mode and repeat steps 2-5.
 *
 * @param h     Handle for communication with TROPIC01
 */
void lt_test_rev_get_log_req(lt_handle_t *h);

/** @} */  // end of libtropic_funct_tests group

#ifdef __cplusplus
}
#endif

#endif  // LT_LIBTROPIC_FUNC_TESTS_H