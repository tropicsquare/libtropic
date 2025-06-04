/**
 * @file lt_l3_transfer.c
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
#include "lt_l3_process.h"
#include "lt_aesgcm.h"

lt_ret_t lt_l3_nonce_increase(uint8_t nonce[12])
{
#ifdef LIBT_DEBUG
    if(!nonce) {
        return LT_PARAM_ERR;
    }
#endif
    uint32_t nonce_int = (nonce[3] << 24) | (nonce[2] << 16) | (nonce[1] << 8) | (nonce[0]);

    nonce_int++;
    nonce[3] = nonce_int >> 24;
    nonce[2] = (nonce_int & 0x00FF0000) >> 16;
    nonce[1] = (nonce_int & 0x0000FF00) >> 8;
    nonce[0] = (nonce_int & 0x000000FF);

    return LT_OK;
}

lt_ret_t lt_l3_encrypt_request(lt_l3_state_t *s3)//, uint8_t buff, uint16_t len)
{
#ifdef LIBT_DEBUG
    if(!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if(s3->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)s3->buff;

    int ret = lt_aesgcm_encrypt(&s3->encrypt, s3->encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    lt_l3_nonce_increase(s3->encryption_IV);

    return LT_OK;
}

lt_ret_t lt_l3_decrypt_response(lt_l3_state_t *s3)
{
#ifdef LIBT_DEBUG
    if(!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if(s3->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)s3->buff;

    lt_ret_t ret = lt_aesgcm_decrypt(&s3->decrypt, s3->decryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case L3_RESULT_FAIL:
            return LT_L3_FAIL;
        case L3_RESULT_UNAUTHORIZED:
            lt_l3_nonce_increase(s3->decryption_IV);
            return LT_L3_UNAUTHORIZED;
        case L3_RESULT_INVALID_CMD:
            return LT_L3_INVALID_CMD;
        case L3_RESULT_OK:
            lt_l3_nonce_increase(s3->decryption_IV);
            return LT_OK;
        case L3_PAIRING_KEY_EMPTY:
            lt_l3_nonce_increase(s3->decryption_IV);
            return LT_L3_PAIRING_KEY_EMPTY;
        case L3_PAIRING_KEY_INVALID:
            lt_l3_nonce_increase(s3->decryption_IV);
            return LT_L3_PAIRING_KEY_INVALID;
        case L3_ECC_INVALID_KEY:
            lt_l3_nonce_increase(s3->decryption_IV);
            return LT_L3_ECC_INVALID_KEY;
        default:
            return LT_FAIL;
    }

}
//-------------

lt_ret_t lt_l3_cmd(lt_handle_t *h)
{
    lt_ret_t ret = LT_OK;
    ret = lt_l3_encrypt_request(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, 4000);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, 4000);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
