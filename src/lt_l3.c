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
    memset(&h->encryption_IV,0,12);
    memset(&h->decryption_IV,0,12);
    return LT_OK;
}

lt_ret_t lt_l3_nonce_increase(uint8_t nonce[12])
{
#ifdef LIBT_DEBUG
    if(!h) {
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

lt_ret_t lt_l3_encrypt_request(lt_handle_t *h)
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

    int ret = lt_aesgcm_encrypt(&h->encrypt, h->encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    lt_l3_nonce_increase(h->encryption_IV);

    return LT_OK;
}

lt_ret_t lt_l3_decrypt_response(lt_handle_t *h)
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

    lt_ret_t ret = lt_aesgcm_decrypt(&h->decrypt, h->decryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case L3_RESULT_FAIL:
            return LT_L3_FAIL;
        case L3_RESULT_UNAUTHORIZED:
            lt_l3_nonce_increase(h->decryption_IV);
            return LT_L3_UNAUTHORIZED;
        case L3_RESULT_INVALID_CMD:
            return LT_L3_INVALID_CMD;
        case L3_RESULT_OK:
            lt_l3_nonce_increase(h->decryption_IV);
            return LT_OK;
        case L3_PAIRING_KEY_EMPTY:
            lt_l3_nonce_increase(h->decryption_IV);
            return LT_L3_PAIRING_KEY_EMPTY;
        case L3_PAIRING_KEY_INVALID:
            lt_l3_nonce_increase(h->decryption_IV);
            return LT_L3_PAIRING_KEY_INVALID;
        case L3_ECC_INVALID_KEY:
            lt_l3_nonce_increase(h->decryption_IV);
            return LT_L3_ECC_INVALID_KEY;
        default:
            return LT_FAIL;
    }

}

lt_ret_t lt_l3_cmd(lt_handle_t *h)
{
    lt_ret_t ret = LT_OK;
    ret = lt_l3_encrypt_request(h);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_encrypted_cmd(h);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l3_decrypt_response(h);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
