/**
 * @file lt_test_rev_random_value_get.c
 * @brief Tests Random_Value_Get command.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"
#include "string.h"

#define RANDOM_VALUE_GET_LOOPS 300

void lt_test_rev_random_value_get(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_random_value_get()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t random_data[TR01_RANDOM_VALUE_GET_LEN_MAX];
    uint16_t random_data_len;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Random_Value_Get will be executed %d times", RANDOM_VALUE_GET_LOOPS);
    for (uint16_t i = 0; i < RANDOM_VALUE_GET_LOOPS; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length <= %d (with lt_random_bytes())...",
                    (int)TR01_RANDOM_VALUE_GET_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &random_data_len, sizeof(random_data_len)));
        random_data_len %= TR01_RANDOM_VALUE_GET_LEN_MAX + 1;  // 0-255

        LT_LOG_INFO("Getting %" PRIu16 " random numbers from TROPIC01...", random_data_len);
        LT_TEST_ASSERT(LT_OK, lt_random_value_get(h, random_data, random_data_len));
        LT_LOG_INFO("Random data from TROPIC01:");
        hexdump_8byte(random_data, random_data_len);
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}