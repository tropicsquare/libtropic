/**
 * @file lt_test_rev_ping.c
 * @brief Test Ping L3 command with random data of random length <= TR01_PING_LEN_MAX.
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

/** @brief How many pings will be sent. */
#define PING_MAX_LOOPS 200

void lt_test_rev_ping(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_ping()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t ping_msg_out[TR01_PING_LEN_MAX], ping_msg_in[TR01_PING_LEN_MAX];
    uint16_t ping_msg_len;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Will send %d Ping commands with random data of random length", PING_MAX_LOOPS);
    for (uint16_t i = 0; i < PING_MAX_LOOPS; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length <= %d...", (int)TR01_PING_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &ping_msg_len, sizeof(ping_msg_len)));
        ping_msg_len %= TR01_PING_LEN_MAX + 1;  // 0-4096

        LT_LOG_INFO("Generating %" PRIu16 " random bytes...", ping_msg_len);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, ping_msg_out, ping_msg_len));

        LT_LOG_INFO("Sending Ping command #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_ping(h, ping_msg_out, ping_msg_in, ping_msg_len));

        LT_LOG_INFO("Comparing sent and received message...");
        LT_TEST_ASSERT(0, memcmp(ping_msg_out, ping_msg_in, ping_msg_len));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
