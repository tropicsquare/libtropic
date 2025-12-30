/**
 * @file lt_test_rev_handshake_req.c
 * @brief Test handshake request and Secure Session abortion request.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_test_common.h"

void lt_test_rev_handshake_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_handshake_req()");
    LT_LOG_INFO("----------------------------------------------");

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Part 1/3: Start and abort Secure Session.");
    LT_LOG_INFO("Starting Secure Session using lt_verify_chip_and_start_secure_session()...");
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG_INFO("Aborting Secure Session using lt_session_abort()...");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Part 2/3: Start Secure Session multiple times without aborting.");
    for (int i = 0; i < 3; i++) {
        LT_LOG_INFO("Starting Secure Session (attempt %d)...", i);
        LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                      TR01_PAIRING_KEY_SLOT_INDEX_0));
    }

    LT_LOG_INFO("Part 3/3: Abort Secure Session multiple times.");
    for (int i = 0; i < 3; i++) {
        LT_LOG_INFO("Aborting Secure Session using lt_session_abort()...");
        LT_TEST_ASSERT(LT_OK, lt_session_abort(h));
    }
}