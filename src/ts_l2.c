#include <string.h>

#include "ts_common.h"
#include "ts_crc16.h"
#include "ts_l1.h"
#include "ts_l2.h"

/**
* @file ts_l2.c
* @brief Layer 2 interfaces
* @author Tropic Square s.r.o.
*/

/** Safety number - limit number of loops during l3 chunks reception. TROPIC01 divides data into 128B
 *  chunks, length of L3 buffer is (2 + 4096 + 16).
 *  Divided by typical chunk length: (2 + 4096 + 16) / 128 => 32,
 *  with a few added loops it is set to 42
 */
#define MAX_LOOPS 42

ts_ret_t ts_l2_frame_check(const uint8_t *frame)
{
    if(!frame) {
        return TS_PARAM_ERR;
    }

    // Take status, len and crc values from incomming frame
    uint8_t status = frame[1];
    uint8_t len = frame[2];
    uint16_t frame_crc = frame[len + 4] | frame[len + 3] << 8;

    switch (status) {

        // Valid frames, or crc errors in INCOMMING frames are handled here:
        case L2_STATUS_REQUEST_OK:
        case L2_STATUS_RESULT_OK:
            if (frame_crc != crc16(frame+1, len + 2)) {
                return TS_L2_IN_CRC_ERR;
            }
            return TS_OK;

        // L2 statuses returned by Tropic chip are handled here
        case L2_STATUS_REQUEST_CONT:
            return TS_L2_REQ_CONT;
        case L2_STATUS_RESULT_CONT:
            return TS_L2_RES_CONT;
        case L2_STATUS_HSK_ERR:
            return TS_L2_HSK_ERR;
        case L2_STATUS_NO_SESSION:
            return TS_L2_NO_SESSION;
        case L2_STATUS_TAG_ERR:
            return TS_L2_TAG_ERR;
        case L2_STATUS_CRC_ERR:
            return TS_L2_CRC_ERR;
        case L2_STATUS_GEN_ERR:
            return TS_L2_GEN_ERR;
        case L2_STATUS_NO_RESP:
            return TS_L2_NO_RESP;
        case L2_STATUS_UNKNOWN_ERR:
            return TS_L2_UNKNOWN_REQ;
        default:
            return TS_FAIL;
    }
}

ts_ret_t ts_l2_transfer(ts_handle_t *h)
{
    if(!h) {
        return TS_PARAM_ERR;
    }

    add_crc(h->l2_buff_req);

    uint8_t len = h->l2_buff_req[1];

    int ret = ts_l1_write(h, len + 4, TS_L1_TIMEOUT_MS_DEFAULT);
    if(ret != TS_OK) {
        return ret;
    }

    ret = ts_l1_read(h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT);
    if(ret != TS_OK) {
        return ret;
    }

    ret = ts_l2_frame_check(h->l2_buff_resp);
    if(ret != TS_OK) {
        return ret;
    }

    return TS_OK;
}

ts_ret_t ts_l2_encrypted_cmd(ts_handle_t *h)
{
    int ret = TS_FAIL;

    /* Setup a pointer to l2 buffers, which are placed in handle */
    struct l2_encrypted_req_t *req = (struct l2_encrypted_req_t*)h->l2_buff_req;
    struct l2_encrypted_rsp_t *resp = (struct l2_encrypted_rsp_t*)h->l2_buff_resp;

    /* Sending part */

    struct l3_frame_t * p_frame = (struct l3_frame_t*)h->l3_buff;
    /* Calculate number of chunks to send. At least one chunk needs to be sent, therefore + 1 */
    uint16_t chunk_num      = ((L3_PACKET_SIZE_SIZE + p_frame->packet_size + L3_TAG_SIZE) / L2_CHUNK_MAX_DATA_SIZE) + 1;
    /* Calculate the length of the last*/
    uint16_t chunk_last_len = ((L3_PACKET_SIZE_SIZE + p_frame->packet_size + L3_TAG_SIZE) % L2_CHUNK_MAX_DATA_SIZE);

    /* Split encrypted buffer into chunks and proceed them into l2 transfers: */
    for (int i=0; i<chunk_num; i++) {

        req->req_id = L2_ENCRYPTED_CMD_REQ_ID;
        /* Update length based on whether actually processed chunk is the last one or not */
        if(i == (chunk_num - 1)) {
            req->req_len = chunk_last_len;
        } else {
            req->req_len = L2_CHUNK_MAX_DATA_SIZE;
        }
        memcpy(req->body, (uint8_t*)&h->l3_buff + i*L2_CHUNK_MAX_DATA_SIZE, req->req_len);

        add_crc(req);

        /* Send l2 request cointaining a chunk from l3 buff */
        ret = ts_l1_write(h, 2 + req->req_len + 2, TS_L1_TIMEOUT_MS_DEFAULT);
        if(ret != TS_OK) {
            return ret;
        }

        /* Read a response on this l2 request */
        ret = ts_l1_read(h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT);
        if(ret != TS_OK) {
            return ret;
        }

        /* Check status byte of this frame */
        ret = ts_l2_frame_check((uint8_t*)resp);
        if(ret != TS_OK && ret != TS_L2_REQ_CONT) {
            return ret;
        }
    }

    /* Receiving part */

    /* Position into l3 buffer where processed l2 chunk will be copied into */
    uint16_t offset = 0;
    /* Tropic can respond with various lengths of chunks, this loop should be limited */
    uint16_t loops = 0;

    do {
        /* Get one l2 frame of a chip's response */
        ret = ts_l1_read(h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT);
        if(ret != TS_OK) {
            return ret;
        }
        /* Prevent overflow of l3 buffer */
        if (offset + resp->resp_len > L3_FRAME_MAX_SIZE) {
            return TS_L2_DATA_LEN_ERROR;
        }

        /* Check status byte of this frame */
        ret = ts_l2_frame_check((uint8_t*)resp);
        switch (ret) {
            case TS_L2_RES_CONT:
                /* Copy content of l2 into certain offset of l3 buffer*/
                memcpy((uint8_t*)&h->l3_buff + offset, (struct l2_encrypted_rsp_t*)resp->body, resp->resp_len);
                offset += resp->resp_len;
                loops++;
                break;
            case TS_OK:
                /* This was last l2 frame of l3 packet, copy it and return */
                memcpy((uint8_t*)&h->l3_buff + offset, (struct l2_encrypted_rsp_t*)resp->body, resp->resp_len);
                return TS_OK;
            default:
                /* Any other L2 packet's status is not expected */
                return ret;
        }
    } while (loops < MAX_LOOPS);

    return TS_FAIL;
}
