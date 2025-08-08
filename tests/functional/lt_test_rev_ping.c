/**
 * @file lt_test_rev_ping.c
 * @brief Test Ping L3 command with random data of random length <= PING_LEN_MAX.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "string.h"

/** @brief How many pings will be sent. */
#define PING_MAX_LOOPS 200

void lt_test_rev_ping(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_ping()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t ping_msg_out[PING_LEN_MAX], ping_msg_in[PING_LEN_MAX];
    uint32_t random_data[PING_LEN_MAX / sizeof(uint32_t)], random_data_size;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Will send %d Ping commands with random data of random length", PING_MAX_LOOPS);
    for (uint16_t i = 0; i < PING_MAX_LOOPS; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length <= %d...", (int)PING_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(&random_data_size, 1));
        random_data_size %= PING_LEN_MAX + 1;  // 0-4096

        LT_LOG_INFO("Generating %" PRIu32 " random bytes...", random_data_size);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(random_data, sizeof(random_data) / sizeof(uint32_t)));
        memcpy(ping_msg_out, random_data, random_data_size);

        LT_LOG_INFO("Sending Ping command #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_ping(h, ping_msg_out, ping_msg_in, random_data_size));

        LT_LOG_INFO("Comparing sent and received message...");
        LT_TEST_ASSERT(0, memcmp(ping_msg_out, ping_msg_in, random_data_size));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
