/**
 * @file lt_test_mock_silicon_rev.c
 * @brief Test for checking if TROPIC01 silicon revision is parsed correctly from CHIP_ID.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_functional_mock_tests.h"
#include "lt_mock_helpers.h"
#include "lt_test_common.h"

/**
 * @brief Mocks lt_init() with the given CHIP_ID and checks that the expected silicon revision was
 * parsed.
 *
 * @param h            Handle for communication with TROPIC01
 * @param chip_id      CHIP_ID to mock
 * @param expected_rev Silicon revision expected to be parsed from the CHIP_ID
 */
static void test_known_silicon_rev(lt_handle_t *h, const struct lt_chip_id_t *chip_id,
                                   const lt_tr01_silicon_rev_t expected_rev)
{
    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK, mock_init_communication_chip_id(
                              h, chip_id, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Checking if silicon revision was parsed correctly");
    LT_TEST_ASSERT(expected_rev, h->tr01_attrs.silicon_rev);

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}

/**
 * @brief Mocks lt_init() with the given CHIP_ID and checks that lt_init() fails with
 * LT_SILICON_REV_UNKNOWN.
 *
 * @param h       Handle for communication with TROPIC01
 * @param chip_id CHIP_ID with an unknown silicon revision to mock
 */
static void test_unknown_silicon_rev(lt_handle_t *h, const struct lt_chip_id_t *chip_id)
{
    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK, mock_init_communication_chip_id(
                              h, chip_id, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    // lt_init() cleans up on failure, so lt_deinit() is not called.
    LT_LOG_INFO("Initializing handle, expecting failure");
    LT_TEST_ASSERT(LT_SILICON_REV_UNKNOWN, lt_init(h));
}

void lt_test_mock_silicon_rev(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_silicon_rev()");
    LT_LOG_INFO("----------------------------------------------");

    // CHIP_ID v0.0.0.1 was used only in ABAB chips and has no silicon revision field.
    LT_LOG_INFO("Testing with mocked silicon revision: ABAB (CHIP_ID v0.0.0.1)");
    const struct lt_chip_id_t chip_id_abab_v0001 = {.chip_id_ver = {0x00, 0x00, 0x00, 0x01}};
    test_known_silicon_rev(h, &chip_id_abab_v0001, LT_TR01_ABAB);

    LT_LOG_INFO("Testing with mocked silicon revision: ACAB");
    const struct lt_chip_id_t chip_id_acab = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                                              .silicon_rev = {'A', 'C', 'A', 'B'}};
    test_known_silicon_rev(h, &chip_id_acab, LT_TR01_ACAB);

    LT_LOG_INFO("Testing with mocked silicon revision: BDBB");
    const struct lt_chip_id_t chip_id_bdbb = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                                              .silicon_rev = {'B', 'D', 'B', 'B'}};
    test_known_silicon_rev(h, &chip_id_bdbb, LT_TR01_BDBB);

    LT_LOG_INFO("Testing with mocked silicon revision: unknown");
    const struct lt_chip_id_t chip_id_unknown = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                                                 .silicon_rev = {'X', 'X', 'X', 'X'}};
    test_unknown_silicon_rev(h, &chip_id_unknown);
}
