#ifndef LT_FUNCTIONAL_MOCK_TESTS_H
#define LT_FUNCTIONAL_MOCK_TESTS_H

/**
 * @file lt_functional_mock_tests.h
 * @brief Declaration of functional mock test functions.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test for checking if TROPIC01 attributes are set correctly based on RISC-V FW version.
 *
 * Test steps:
 *  For each tested RISC-V FW version:
 *   1. Mock TROPIC01 responses on init to simulate different RISC-V FW versions.
 *   2. Initialize libtropic handle.
 *   3. Verify that attributes in the handle are set correctly according to the mocked FW version
 *   4. Deinitialize libtropic handle.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_attrs(lt_handle_t *h);

/**
 * @brief Test for checking if TROPIC01 silicon revision is parsed correctly from CHIP_ID.
 *
 * Test steps:
 *  For each known silicon revision (ABAB with CHIP_ID v0.0.0.1, ABAB, ACAB, BDBB):
 *   1. Mock TROPIC01 responses on init to return a CHIP_ID with the given silicon revision.
 *   2. Initialize libtropic handle.
 *   3. Verify that the silicon revision in the handle is set correctly.
 *   4. Deinitialize libtropic handle.
 *  For an unknown silicon revision:
 *   5. Mock TROPIC01 responses on init to return a CHIP_ID with an unknown silicon revision.
 *   6. Verify that libtropic handle initialization fails with LT_SILICON_REV_UNKNOWN.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_silicon_rev(lt_handle_t *h);

/**
 * @brief Test for handling invalid CRC in requests sent to TROPIC01.
 *
 * @note Get_Info Request is used for testing on L2, and Ping is used on L3.
 *
 * Test steps:
 *  1. Mock init sequence and initialize Libtropic handle.
 *  2. Test LT_L2_CRC_ERR retry mechanism on L2 by returning STATUS=CRC_ERR until all retry
 *     attempts are depleted and verify LT_L2_CRC_ERR is returned.
 *  3. Test LT_L2_CRC_ERR retry mechanism on L2 with a random number of STATUS=CRC_ERR frames
 *     followed by STATUS=OK and verify LT_OK is returned.
 *  4. Start mocked Secure Session.
 *  5. Test LT_L2_CRC_ERR retry mechanism on L3 in lt_l2_send_encrypted_cmd by returning
 *     STATUS=CRC_ERR until all retry attempts are depleted and verify LT_L2_CRC_ERR is returned.
 *  6. Test LT_L2_CRC_ERR retry mechanism on L3 in lt_l2_send_encrypted_cmd with a random number
 *     of STATUS=CRC_ERR frames followed by STATUS=OK and verify LT_OK is returned.
 *  7. Test LT_L2_CRC_ERR retry mechanism on L3 in lt_l2_recv_encrypted_res by returning
 *     STATUS=CRC_ERR until all retry attempts are depleted and verify LT_L2_CRC_ERR is returned.
 *  8. Test LT_L2_CRC_ERR retry mechanism on L3 in lt_l2_recv_encrypted_res with a random number
 *     of STATUS=CRC_ERR frames followed by STATUS=OK and verify LT_OK is returned.
 *  9. Terminate Secure Session and deinitialize the Libtropic handle.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_invalid_crc(lt_handle_t *h);

/**
 * @brief Test for handling invalid CRC in TROPIC01 responses.
 *
 * @note Get_Info Request is used for testing on L2, and Ping is used on L3.
 *
 * Test steps:
 *  1. Mock init sequence and initialize Libtropic handle.
 *  2. Test LT_L2_IN_CRC_ERR retry mechanism on L2: Deplete all retry attempts and check if
 *     LT_L2_IN_CRC_ERR was returned.
 *  3. Test LT_L2_IN_CRC_ERR retry mechanism on L2:  Send random number of corrupted frames (with
 *     incorrect CRC) and then a correct one and check if LT_OK is returned.
 *  4. Start mocked Secure Session.
 *  5. Test LT_L2_IN_CRC_ERR retry mechanism on L3 in lt_l2_send_encrypted_cmd: send corrupted CRCs in
 *     the responses to commands. Deplete all retry attempts and check if LT_L2_IN_CRC_ERR was
 *     returned. Use lt_ping as a dummy command.
 *  6. Test LT_L2_IN_CRC_ERR retry mechanism on L3 in lt_l2_send_encrypted_cmd: send random number of
 *     corrupted frames (with incorrect CRC) in the responses to commands and then a correct one and
 *     check if LT_OK is returned. Use lt_ping as a dummy command.
 *  7. Test LT_L2_IN_CRC_ERR retry mechanism on L3 in lt_l2_recv_encrypted_res: send corrupted CRCs in
 *     Results. Deplete all retry attempts and check if LT_L2_IN_CRC_ERR was returned. Use lt_ping as a
 *     dummy command.
 *  8. Test LT_L2_IN_CRC_ERR retry mechanism on L3 in lt_l2_recv_encrypted_res: send random number of
 *     corrupted Results (with incorrect CRC) then a correct one and check if LT_OK is returned. Use
 *     lt_ping as a dummy command.
 *  9. Terminate the session and deinitialize the Libtropic handle.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_invalid_in_crc(lt_handle_t *h);

/**
 * @brief Test for handling HARDWARE_FAIL return code.
 *
 * Test steps:
 * 1. Mock Secure Session initialization.
 * 2. For each of Pairing_Key_Write, Pairing_Key_Invalidate, R_Config_Write, I_Config_Write,
 * R_Mem_Data_Write: a. Mock L3 Result with RESULT=HARDWARE_FAIL. b. Call Libtropic function
 * corresponding to the L3 Command and verify that Libtropic returns LT_L3_HARDWARE_FAIL.
 * 3. Mock Secure Session deinitialization.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_hardware_fail(lt_handle_t *h);

/**
 * @brief Test for checking that lt_init suceeds if Application FW cannot be booted.
 *
 * Test steps:
 *  1. Mock responses to simulate a scenario where Application FW cannot be booted, i.e. TROPIC01 stays
 *     in Start-up Mode.
 *  2. Call lt_init and verify that it succeeds.
 *
 * @param h Handle for communication with TROPIC01
 */
void lt_test_mock_invalid_app_fw_init(lt_handle_t *h);

#ifdef __cplusplus
}
#endif

#endif  // LT_FUNCTIONAL_MOCK_TESTS_H