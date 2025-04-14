/**
 * @file lt_test_ping.c
 * @brief Test function which tries Ping command with max length of message with all 4 pairing slots
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"

// Ping message of max length
uint8_t ping_msg[PING_LEN_MAX];

/**
 * @brief  
 *
 * @return int
 */
int lt_test_ping(void)
{
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- lt_test_ping() -------------------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");

    lt_handle_t h = {0};

    // Ping message definition
    for (uint8_t i = 0; i < (PING_LEN_MAX / 32); i++) {
        memcpy(ping_msg+32*i, "This is ping message to be sent", 32);
    }

    uint8_t in[PING_LEN_MAX] = {0};

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    // Ping with SH0
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH1
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_1);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh1priv, sh1pub, pkey_index_1));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH2
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_2);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh2priv, sh2pub, pkey_index_2));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH3
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_3);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh3priv, sh3pub, pkey_index_3));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}


