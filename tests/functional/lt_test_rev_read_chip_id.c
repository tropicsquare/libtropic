/**
 * @file lt_test_rev_read_chip_id.c
 * @brief Test reading Chip ID and parse it.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

void lt_test_rev_read_chip_id(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_read_chip_id()");
    LT_LOG_INFO("----------------------------------------------");

    struct lt_chip_id_t chip_id = {0};

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Reading Chip ID:");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(h, &chip_id));
    LT_TEST_ASSERT(LT_OK, lt_print_chip_id(&chip_id, printf));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
