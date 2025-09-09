#include "libtropic_l2.h"

#include <string.h>

#include "libtropic_common.h"
#include "lt_crc16.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"

/**
 * @file libtropic_l2.c
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

lt_ret_t lt_l2_send(lt_l2_state_t *s2)
{
    if (!s2) {
        return LT_PARAM_ERR;
    }

    add_crc(s2->buff);

    uint8_t len = s2->buff[1];

    return lt_l1_write(s2, len + 4, LT_L1_TIMEOUT_MS_DEFAULT);
}

lt_ret_t lt_l2_resend_response(lt_l2_state_t *s2)
{
    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_resend_req_t *p_l2_req = (struct lt_l2_resend_req_t *)s2->buff;
    p_l2_req->req_id = LT_L2_RESEND_REQ_ID;
    p_l2_req->req_len = LT_L2_RESEND_REQ_LEN;

    lt_ret_t ret = lt_l2_send(s2);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l1_read(s2, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
    if (ret != LT_OK) {
        return ret;
    }

    return lt_l2_frame_check(s2->buff);
}

lt_ret_t lt_l2_receive(lt_l2_state_t *s2)
{
    if (!s2) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret = lt_l1_read(s2, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_frame_check(s2->buff);

    if ((ret == LT_L2_CRC_ERR) || (ret == LT_L2_GEN_ERR)) {
        // There was an error when checking received data.
        // Let's consider that length byte is correct, but CRC is not.
        // We try three times to resend the last response.
        for (int i = 0; i < 3; i++) {
            ret = lt_l2_resend_response(s2);
            if (ret == LT_OK) {
                break;
            }
        }
    }

    // Rest of errors are reported directly to upper layers, without trying to resend response.
    return ret;
}

lt_ret_t lt_l2_send_encrypted_cmd(lt_l2_state_t *s2, uint8_t *buff, uint16_t max_len)
{
    if (!s2
        // Max len must be definitively smaller than size of l3 buffer
        || max_len > L3_PACKET_MAX_SIZE || !buff) {
        return LT_PARAM_ERR;
    }

    int ret = LT_FAIL;

    // There is l3 payload in passed buffer.
    // First check how much data are to be send and if it actually fits into that buffer,
    // there must be a space for 2B of size value, ?B of command (ID + data) and 16B of TAG.
    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)buff;
    uint16_t packet_size = (L3_CMD_SIZE_SIZE + p_frame->cmd_size + L3_TAG_SIZE);
    // Prevent sending more data then is the size of compiled l3 buffer
    if (packet_size > max_len) {
        return LT_L3_DATA_LEN_ERROR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_cmd_req_t *req = (struct lt_l2_encrypted_cmd_req_t *)s2->buff;

    // Calculate number of chunks to send.
    // First, get the number of full chunks.
    uint16_t full_chunk_num = (packet_size / L2_CHUNK_MAX_DATA_SIZE);
    // Second, if packet_size is not divisible by the maximum chunk size, one additional smaller chunk will be created,
    // which we add to the total count.
    uint16_t chunk_num = (packet_size % L2_CHUNK_MAX_DATA_SIZE) == 0 ? full_chunk_num : full_chunk_num + 1;
    // Calculate the length of the last chunk
    uint16_t last_chunk_len = packet_size - ((chunk_num - 1) * L2_CHUNK_MAX_DATA_SIZE);

    // Split encrypted buffer into chunks and proceed them into l2 transfers:
    for (int i = 0; i < chunk_num; i++) {
        req->req_id = LT_L2_ENCRYPTED_CMD_REQ_ID;
        // If the currently processed chunk is the last one, get its length (may be shorter than L2_CHUNK_MAX_DATA_SIZE)
        if (i == (chunk_num - 1)) {
            req->req_len = last_chunk_len;
        }
        else {
            req->req_len = L2_CHUNK_MAX_DATA_SIZE;
        }
        memcpy(req->l3_chunk, buff + i * L2_CHUNK_MAX_DATA_SIZE, req->req_len);

        add_crc(req);

        // Send l2 request cointaining a chunk from l3 buff
        ret = lt_l1_write(s2, 2 + req->req_len + 2, LT_L1_TIMEOUT_MS_DEFAULT);
        if (ret != LT_OK) {
            return ret;
        }

        // Read a response on this l2 request
        ret = lt_l1_read(s2, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
        if (ret != LT_OK) {
            return ret;
        }

        // Check status byte of this frame
        ret = lt_l2_frame_check(s2->buff);
        if (ret != LT_OK && ret != LT_L2_REQ_CONT) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t lt_l2_recv_encrypted_res(lt_l2_state_t *s2, uint8_t *buff, uint16_t max_len)
{
    if (!s2
        // Max len must be definitively smaller than size of l3 buffer
        || max_len > L3_PACKET_MAX_SIZE || !buff) {
        return LT_PARAM_ERR;
    }

    int ret = LT_FAIL;
    // Setup a response pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_cmd_rsp_t *resp = (struct lt_l2_encrypted_cmd_rsp_t *)s2->buff;

    // Position into l3 buffer where processed l2 chunk will be copied into
    uint16_t offset = 0;
    // Tropic can respond with various lengths of chunks, this loop should be limited
    uint16_t loops = 0;

    do {
        /* Get one l2 frame of a device's response */
        ret = lt_l1_read(s2, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT);
        if (ret != LT_OK) {
            return ret;
        }

        // Prevent receiving more data then is compiled size of l3 buffer
        if (offset + resp->rsp_len > max_len) {
            return LT_L3_DATA_LEN_ERROR;
        }

        // Check status byte of this frame
        ret = lt_l2_frame_check(s2->buff);
        switch (ret) {
            case LT_L2_RES_CONT:
                // Copy content of l2 into certain offset of l3 buffer
                memcpy(buff + offset, (struct l2_encrypted_rsp_t *)resp->l3_chunk, resp->rsp_len);
                offset += resp->rsp_len;
                loops++;
                break;
            case LT_OK:
                // This was last l2 frame of l3 packet, copy it and return
                memcpy(buff + offset, (struct l2_encrypted_rsp_t *)resp->l3_chunk, resp->rsp_len);
                return LT_OK;
            default:
                // Any other L2 packet's status is not expected
                return ret;
        }
    } while (loops < MAX_LOOPS);

    return LT_FAIL;
}
