/**
 * @file lt_ex_hello_world.c
 * @brief Establishes Secure Session and executes Ping L3 command.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"

/** @brief Message to send with Ping L3 command. */
#define PING_MSG "This is Hello World message from TROPIC01!!"
/** @brief Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 44

int lt_ex_hello_world(lt_handle_t *h)
{
    LT_LOG_INFO("======================================");
    LT_LOG_INFO("==== TROPIC01 Hello World Example ====");
    LT_LOG_INFO("======================================");

    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                     lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    uint8_t recv_buf[PING_MSG_SIZE];
    LT_LOG_INFO("Sending Ping command with message:");
    LT_LOG_INFO("\t\"%s\"", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Ping command failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Message received from TROPIC01:");
    LT_LOG_INFO("\t\"%s\"", recv_buf);
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
