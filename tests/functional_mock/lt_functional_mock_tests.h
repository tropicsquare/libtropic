#ifndef LT_FUNCTIONAL_MOCK_TESTS_H
#define LT_FUNCTIONAL_MOCK_TESTS_H

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
 * 
 * @return 0 on success, non-zero on failure.
 */
int lt_test_mock_attrs(lt_handle_t *h);

/**
 * @brief Test for handling invalid CRC in TROPIC01 responses.
 * 
 * Test steps:
 *  1. Mock a response with an invalid CRC for a dummy request (Get_Info is used).
 *  2. Send request.
 *  3. Verify that Libtropic correctly identifies the invalid CRC and returns an error.
 * 
 * @param h Handle for communication with TROPIC01
 * 
 * @return 0 on success, non-zero on failure.
 */
int lt_test_mock_invalid_in_crc(lt_handle_t *h);

#ifdef __cplusplus
}
#endif

#endif // LT_FUNCTIONAL_MOCK_TESTS_H