/**
 * @file lt_test_mock_invalid_pn.c
 * @brief Test printing of invalid CHIP_ID
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "lt_functional_mock_tests.h"
#include "lt_test_common.h"

void lt_test_mock_invalid_pn(lt_handle_t *h)
{
    LT_UNUSED(h);

    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_invalid_pn()");
    LT_LOG_INFO("----------------------------------------------");

    lt_chip_id_t chip_id = {.chip_id_ver = {0x01, 0x00, 0x00, 0x00},
                            .silicon_rev = {'A', 'C', 'A', 'B'}};

    // First byte of part_num_data indicates length of the P/N data, the rest (max 15 bytes) is the P/N
    // itself.
    memset(&chip_id.part_num_data[1], 'X', sizeof(chip_id.part_num_data) - 1);

    LT_LOG_INFO("Calling lt_print_chip_id with maximum valid P/N length (part_num_data[0]=15)");
    chip_id.part_num_data[0] = 15;
    LT_TEST_ASSERT(LT_OK, lt_print_chip_id(&chip_id, chip_id_printf_wrapper));

    LT_LOG_INFO("Calling lt_print_chip_id with P/N length one over maximum (part_num_data[0]=16)");
    chip_id.part_num_data[0] = 16;
    LT_TEST_ASSERT(LT_FAIL, lt_print_chip_id(&chip_id, chip_id_printf_wrapper));

    LT_LOG_INFO("Calling lt_print_chip_id with invalid P/N length (part_num_data[0]=0xff)");
    chip_id.part_num_data[0] = 0xff;
    LT_TEST_ASSERT(LT_FAIL, lt_print_chip_id(&chip_id, chip_id_printf_wrapper));
}
