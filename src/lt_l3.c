/**
 * @file lt_l3.c
 * @brief Layer 3 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "lt_l1.h"
#include "lt_l2.h"
#include "lt_l3.h"
#include "lt_aesgcm.h"

lt_ret_t lt_l3_nonce_init(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    memset(&h->IV,0,12);
    return LT_OK;
}

lt_ret_t lt_l3_nonce_increase(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    uint32_t nonce = (h->IV[3] << 24) | (h->IV[2] << 16) | (h->IV[1] << 8) | (h->IV[0]);

    nonce ++;
    h->IV[3] = nonce >> 24;
    h->IV[2] = (nonce & 0x00FF0000) >> 16;
    h->IV[1] = (nonce & 0x0000FF00) >> 8;
    h->IV[0] = (nonce & 0x000000FF);

    return LT_OK;
}

lt_ret_t lt_l3_cmd(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)h->l3_buff;

    int ret = lt_aesgcm_encrypt(&h->encrypt, h->IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_encrypted_cmd(h);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_aesgcm_decrypt(&h->decrypt, h->IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case L3_RESULT_FAIL:
            return LT_L3_FAIL;
        case L3_RESULT_UNAUTHORIZED:
            lt_l3_nonce_increase(h);
            return LT_L3_UNAUTHORIZED;
        case L3_RESULT_INVALID_CMD:
            return LT_L3_INVALID_CMD;
        case L3_RESULT_OK:
            lt_l3_nonce_increase(h);
            return LT_OK;
        case L3_PAIRING_KEY_EMPTY:
            lt_l3_nonce_increase(h);
            return LT_L3_PAIRING_KEY_EMPTY;
        case L3_PAIRING_KEY_INVALID:
            lt_l3_nonce_increase(h);
            return LT_L3_PAIRING_KEY_INVALID;
        default:
            return LT_FAIL;
    }
}
