/**
 * @file lt_ex_hello_world_separate_API.c
 * @brief Establishes Secure Session and executes Ping L3 command using separated API.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_examples.h"
#include "libtropic_l2.h"
#include "libtropic_l3.h"
#include "libtropic_logging.h"

/** @brief Message to send with Ping L3 command. */
#define PING_MSG "This is Hello World message from TROPIC01!!"
/** @brief Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 44

int lt_ex_hello_world_separate_API(lt_handle_t *h)
{
    LT_LOG_INFO("=========================================================");
    LT_LOG_INFO("====  TROPIC01 Hello World with Separate API Example ====");
    LT_LOG_INFO("=========================================================");

    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Getting Certificate Store from TROPIC01");
    uint8_t cert1[LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE], cert2[LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE],
        cert3[LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE], cert4[LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE];
    struct lt_cert_store_t store
        = {.certs = {cert1, cert2, cert3, cert4},
           .buf_len = {LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE, LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE,
                       LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE, LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE}};
    ret = lt_get_info_cert_store(h, &store);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to get Certificate Store, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // Get only stpub, we don't verify certificate chain here
    LT_LOG_INFO("Getting stpub key from Certificate Store");
    uint8_t stpub[32];
    ret = lt_get_st_pub(&store, stpub, 32);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to get stpub key, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    //---------------------------------------------------------------------------------------//
    // Separated API calls for starting a secure session:
    session_state_t state = {0};

    // Inicialize session from a server side by creating state->ehpriv and state->ehpub,
    // l2 request is prepared into handle's buffer (h->l2_buff)
    LT_LOG_INFO("Executing lt_out__session_start()...");
    ret = lt_out__session_start(h, PAIRING_KEY_SLOT_INDEX_0, &state);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_out__session_start() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // handle's buffer (h->l2_buff) now contains data which must be transferred over tunnel to TROPIC01

    // Following l2 functions are called on remote host
    LT_LOG_INFO("Executing lt_l2_send()...");
    ret = lt_l2_send(&h->l2);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_l2_send() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("Executing lt_l2_receive()...");
    ret = lt_l2_receive(&h->l2);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_l2_receive() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // Handle's buffer (h->l2_buff) now contains data which must be transferred over tunnel back to the server

    // Once data are back on server's side, bytes are copied into h->l2_buff
    // Then following l2 function is called on server side
    // This function establishes gcm contexts for a session
    LT_LOG_INFO("Executing lt_in__session_start()...");
    ret = lt_in__session_start(h, stpub, PAIRING_KEY_SLOT_INDEX_0, sh0priv, sh0pub, &state);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_in__session_start failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    // Now we can use lt_ping() to send a message to TROPIC01 and receive a response, this is done with separate API
    // calls
    uint8_t recv_buf[PING_MSG_SIZE];
    LT_LOG_INFO("Executing lt_out__ping() with message:");
    LT_LOG_INFO("\t\"%s\"", PING_MSG);
    ret = lt_out__ping(h, (const uint8_t *)PING_MSG, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_out__ping failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Executing lt_l2_send_encrypted_cmd()...");
    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_l2_send_encrypted_cmd failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("Executing lt_l2_recv_encrypted_res()...");
    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, h->l3.buff_len);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_l2_recv_encrypted_res failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Executing lt_in__ping()...");
    ret = lt_in__ping(h, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_in__ping failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }

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
