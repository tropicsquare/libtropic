/**
 * @file lt_test_rev_sleep_req.c
 * @brief Test Sleep_Req L2 request.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

void lt_test_rev_sleep_req(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_sleep_req()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h;
    uint8_t msg_out[4] = {'T', 'E', 'S', 'T'};
    uint8_t msg_in[4];
    struct lt_chip_id_t chip_id;

#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG_INFO("Sending Sleep_Req...");
    LT_TEST_ASSERT(LT_OK, lt_sleep(&h, LT_L2_SLEEP_KIND_SLEEP));

    LT_LOG_INFO("Verifying we are not in Secure Session...");
    LT_TEST_ASSERT(LT_L2_NO_SESSION, lt_ping(&h, msg_out, msg_in, sizeof(msg_out)));

    LT_LOG_INFO("Waking the chip up by sending dummy L2 request...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(&h, &chip_id));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(&h));
}