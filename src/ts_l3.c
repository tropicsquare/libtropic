/**
* @file ts_l3.c
* @brief This file contains interfaces related to layer 3.
* @author Tropic Square s.r.o.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "ts_l1.h"
#include "ts_l2.h"
#include "ts_l3.h"
#include "ts_aesgcm.h"

ts_ret_t ts_l3_nonce_init(ts_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return TS_PARAM_ERR;
    }
#endif
    memset(&h->IV,0,12);
    return TS_OK;
}

ts_ret_t ts_l3_nonce_increase(ts_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return TS_PARAM_ERR;
    }
#endif
    uint32_t nonce = (h->IV[3] << 24) | (h->IV[2] << 16) | (h->IV[1] << 8) | (h->IV[0]);

    nonce ++;
    h->IV[3] = nonce >> 24;
    h->IV[2] = (nonce & 0x00FF0000) >> 16;
    h->IV[1] = (nonce & 0x0000FF00) >> 8;
    h->IV[0] = (nonce & 0x000000FF);

    return TS_OK;
}

ts_ret_t ts_l3_cmd(ts_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return TS_PARAM_ERR;
    }
#endif
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }

    struct l3_frame_t * p_frame = (struct l3_frame_t*)h->l3_buff;

    int ret = ts_aesgcm_encrypt(&h->encrypt, h->IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE);
    if (ret != TS_OK) {
        return ret;
    }

    ret = ts_l2_encrypted_cmd(h);
    if (ret != TS_OK) {
        return ret;
    }

    ret = ts_aesgcm_decrypt(&h->decrypt, h->IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE);
    if (ret != TS_OK) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case L3_RESULT_FAIL:
            return TS_L3_FAIL;
        case L3_RESULT_UNAUTHORIZED:
            return TS_L3_UNAUTHORIZED;
        case L3_RESULT_INVALID_CMD:
            return TS_L3_INVALID_CMD;
        case L3_RESULT_OK:
            ts_l3_nonce_increase(h);
            return TS_OK;
        default:
            return TS_FAIL;
    }
}
