/**
 * @file lt_ex_hello_world_separate_API.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "lt_l2.h"
#include "lt_l3.h"
#include "string.h"

/**
 * @name Hello World with separate calls
 *
 * @details This example shows how to use separated API calls with TROPIC01. Separate calls are named lt_out__* and
 * lt_in__*and they provide splitting of the commands/results, which might be used for example for communication over a
 * tunnel. Let's say we want to speak with TROPIC01 from a server, then lt_out__* part is done on the server, then
 * encrypted payload is transferred over tunnel to the point where SPI is wired to TROPIC01. L2 communication is
 * executed, encrypted result is transferred back to the server, where lt_in__* function is used to decrypt the
 * response.
 *
 * To have a better understanding have a look into lt_ex_hello_world.c, both examples shows similar procedure.
 *
 * This might be used for example in production, where we want to establish a secure channel between HSM and TROPIC01 on
 * PCB.
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
 */

/**
 * @brief Session with H0 pairing keys
 *
 * @param h           Device's handle
 * @return            0 if success, otherwise -1
 */
static int session_H0(void)
{
    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    lt_init(&h);

    LT_LOG("%s", "Establish session with H0");

    // First we need to get certificate store from TROPIC01
    LT_LOG("%s", "lt_get_info_cert_store()");

    uint8_t cert1[700] = {0};
    uint8_t cert2[700] = {0};
    uint8_t cert3[700] = {0};
    uint8_t cert4[700] = {0};

    struct lt_cert_store_t store
        = {.cert_len = {0, 0, 0, 0}, .buf_len = {700, 700, 700, 700}, .certs = {cert1, cert2, cert3, cert4}};

    LT_ASSERT(LT_OK, lt_get_info_cert_store(&h, &store));

    // Then we need to get stpub out of it
    // We don't verify certificate chain here. This is intended in separate example
    LT_LOG("%s", "lt_get_st_pub() ");
    uint8_t stpub[32] = {0};
    LT_ASSERT(LT_OK, lt_get_st_pub(&store, stpub, 32));

    //---------------------------------------------------------------------------------------//
    // Separated API calls for starting a secure session:
    session_state_t state;

    // Inicialize session from a server side  by creating state->ehpriv and state->ehpub,
    // l2 request is prepared into handle's buffer (h->l2_buff)
    LT_LOG("%s", "lt_out__session_start() ");
    LT_ASSERT(LT_OK, lt_out__session_start(&h, PAIRING_KEY_SLOT_INDEX_0, &state));

    // handle's buffer (h->l2_buff) now contains data which must be transferred over tunnel to TROPIC01

    // Following l2 functions are called on remote host
    LT_LOG("%s", "lt_l2_send() ");
    LT_ASSERT(LT_OK, lt_l2_send(&h.l2));
    LT_LOG("%s", "lt_l2_receive() ");
    LT_ASSERT(LT_OK, lt_l2_receive(&h.l2));

    // Handle's buffer (h->l2_buff) now contains data which must be transferred over tunnel back to the server

    // Once data are back on server's side, bytes are copied into h->l2_buff
    // Then following l2 function is called on server side
    // This function establishes gcm contexts for a session
    LT_LOG("%s", "lt_in__session_start() ");
    LT_ASSERT(LT_OK, lt_in__session_start(&h, stpub, PAIRING_KEY_SLOT_INDEX_0, sh0priv, sh0pub, &state));

    // Now we can use lt_ping() to send a message to TROPIC01 and receive a response, this is done with separate API
    // calls
    uint8_t in[100] = {0};
    uint8_t out[100] = {0};
    memcpy(out, "This is Hello World message from TROPIC01!!", 43);

    LT_LOG("%s", "lt_out__ping() ");
    LT_ASSERT(LT_OK, lt_out__ping(&h, out, 43));

    LT_LOG("%s", "lt_l2_send_encrypted_cmd() ");
    LT_ASSERT(LT_OK, lt_l2_send_encrypted_cmd(&h.l2, h.l3.buff, 4000));
    LT_LOG("%s", "lt_l2_recv_encrypted_res() ");
    LT_ASSERT(LT_OK, lt_l2_recv_encrypted_res(&h.l2, h.l3.buff, 4000));

    LT_LOG("%s", "lt_in__ping() ");
    LT_ASSERT(LT_OK, lt_in__ping(&h, in, 43));

    LT_LOG("\t\tMessage: %s", in);

    lt_deinit(&h);

    return 0;
}

int lt_ex_hello_world_separate_API(void)
{
    LT_LOG("");
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 Hello World with separate API                       ===");
    LT_LOG("\t=======================================================================");

    LT_LOG_LINE();
    LT_LOG("\t Session with H0 keys:");
    if (session_H0() == -1) {
        LT_LOG("Error during session_H0()");
    }

    LT_LOG_LINE();

    LT_LOG("\t End of execution, no errors.");

    return 0;
}
