#include <string.h>

#include "libtropic_common.h"
#include "lt_crc16.h"
#include "lt_l1.h"
#include "lt_l2.h"
#include "lt_l2_frame_check.h"
#include "lt_l2_api_structs.h"

/**
 * @file lt_l2.c
 * @brief Layer 2 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

/** Safety number - limit number of loops during l3 chunks reception. TROPIC01 divides data into 128B
 *  chunks, length of L3 buffer is (2 + 4096 + 16).
 *  Divided by typical chunk length: (2 + 4096 + 16) / 128 => 32,
 *  with a few added loops it is set to 42
 */
#define MAX_LOOPS 42

lt_ret_t lt_l2_transfer(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    add_crc(h->l2_buff);

    uint8_t len = h->l2_buff[1];

    int ret = lt_l1_write(h, len + 4, LT_L1_TIMEOUT_MS_DEFAULT);
    if(ret != LT_OK) {
        return ret;
    }

    ret = lt_l1_read(h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
    if(ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_frame_check(h->l2_buff);
    // We can ask TROPIC01 to resend the last response. It makes sense to do it if
    // lt_l2_frame_check() returned CRC error or some generic error.
    if((ret == LT_L2_CRC_ERR) || (ret == LT_L2_GEN_ERR)) {
        // Payload's CRC is not OK, let's try to resend it three times
        for(int i=0; i<3; i++) {

            // Setup a request pointer to l2 buffer, which is placed in handle
            struct lt_l2_resend_req_t* p_l2_req = (struct lt_l2_resend_req_t*)&h->l2_buff;
            p_l2_req->req_id = LT_L2_RESEND_REQ_ID;
            p_l2_req->req_len = LT_L2_RESEND_REQ_LEN;

            int ret = lt_l1_write(h, len + 4, LT_L1_TIMEOUT_MS_DEFAULT);
            if(ret != LT_OK) {
                return ret;
            }

            ret = lt_l1_read(h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
            if(ret != LT_OK) {
                return ret;
            }

            ret = lt_l2_frame_check(h->l2_buff);
            if(ret == LT_OK) {
                 // Payload is ok, returning
                return LT_OK;
            }
        }
    }

    // Rest of errors are reported directly to upper layers, without trying to resend response.
    return ret;
}

lt_ret_t lt_l2_encrypted_cmd(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    int ret = LT_FAIL;

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_cmd_req_t *req = (struct lt_l2_encrypted_cmd_req_t*)h->l2_buff;
    // Setup a response pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_cmd_rsp_t *resp = (struct lt_l2_encrypted_cmd_rsp_t*)h->l2_buff;

    // SENDING PART

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)h->l3_buff;
    // Calculate number of chunks to send. At least one chunk needs to be sent, therefore + 1
    uint16_t chunk_num      = ((L3_CMD_SIZE_SIZE + p_frame->cmd_size + L3_TAG_SIZE) / L2_CHUNK_MAX_DATA_SIZE) + 1;
    // Calculate the length of the last
    uint16_t chunk_last_len = ((L3_RES_SIZE_SIZE + p_frame->cmd_size + L3_TAG_SIZE) % L2_CHUNK_MAX_DATA_SIZE);

    // Split encrypted buffer into chunks and proceed them into l2 transfers:
    for (int i=0; i<chunk_num; i++) {

        req->req_id = LT_L2_ENCRYPTED_CMD_REQ_ID;
        // Update length based on whether actually processed chunk is the last one or not
        if(i == (chunk_num - 1)) {
            req->req_len = chunk_last_len;
        } else {
            req->req_len = L2_CHUNK_MAX_DATA_SIZE;
        }
        memcpy(req->l3_chunk, (uint8_t*)&h->l3_buff + i*L2_CHUNK_MAX_DATA_SIZE, req->req_len);

        add_crc(req);

        // Send l2 request cointaining a chunk from l3 buff
        ret = lt_l1_write(h, 2 + req->req_len + 2, LT_L1_TIMEOUT_MS_DEFAULT);
        if(ret != LT_OK) {
            return ret;
        }

        // Read a response on this l2 request
        ret = lt_l1_read(h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
        if(ret != LT_OK) {
            return ret;
        }

        // Check status byte of this frame
        ret = lt_l2_frame_check((uint8_t*)resp);
        if(ret != LT_OK && ret != LT_L2_REQ_CONT) {
            return ret;
        }
    }

    // RECEIVING PART

    // Position into l3 buffer where processed l2 chunk will be copied into
    uint16_t offset = 0;
    // Tropic can respond with various lengths of chunks, this loop should be limited
    uint16_t loops = 0;

    do {
        /* Get one l2 frame of a device's response */
        ret = lt_l1_read(h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
        if(ret != LT_OK) {
            return ret;
        }
        // Prevent overflow of l3 buffer
        if (offset + resp->rsp_len > L3_FRAME_MAX_SIZE) {
            return LT_L3_DATA_LEN_ERROR;
        }

        // Check status byte of this frame
        ret = lt_l2_frame_check((uint8_t*)resp);
        switch (ret) {
            case LT_L2_RES_CONT:
                // Copy content of l2 into certain offset of l3 buffer
                memcpy((uint8_t*)&h->l3_buff + offset, (struct l2_encrypted_rsp_t*)resp->l3_chunk, resp->rsp_len);
                offset += resp->rsp_len;
                loops++;
                break;
            case LT_OK:
                // This was last l2 frame of l3 packet, copy it and return
                memcpy((uint8_t*)&h->l3_buff + offset, (struct l2_encrypted_rsp_t*)resp->l3_chunk, resp->rsp_len);
                return LT_OK;
            default:
                // Any other L2 packet's status is not expected
                return ret;
        }
    } while (loops < MAX_LOOPS);

    return LT_FAIL;
}
