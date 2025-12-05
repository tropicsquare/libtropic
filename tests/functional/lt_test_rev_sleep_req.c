/**
 * @file lt_test_rev_sleep_req.c
 * @brief Test Sleep_Req L2 request.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_test_common.h"

void lt_test_rev_sleep_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_sleep_req()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t msg_out[4] = {'T', 'E', 'S', 'T'};
    uint8_t msg_in[4];
    struct lt_chip_id_t chip_id;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG_INFO("Sending Sleep_Req...");
    LT_TEST_ASSERT(LT_OK, lt_sleep(h, TR01_L2_SLEEP_KIND_SLEEP));

    LT_LOG_INFO("Verifying we are not in Secure Session...");
    LT_TEST_ASSERT(LT_L2_NO_SESSION, lt_ping(h, msg_out, msg_in, sizeof(msg_out)));

    LT_LOG_INFO("Waking the chip up by sending dummy L2 request...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(h, &chip_id));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}