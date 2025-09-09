/**
 * @file lt_l3_process.c
 * @brief Layer 3 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_l3_process.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_l2.h"
#include "lt_aesgcm.h"
#include "lt_l1.h"

LT_STATIC lt_ret_t lt_l3_nonce_increase(uint8_t *nonce)
{
#ifdef LIBT_DEBUG
    if (!nonce) {
        return LT_PARAM_ERR;
    }
#endif
    uint32_t nonce_int = (nonce[3] << 24) | (nonce[2] << 16) | (nonce[1] << 8) | (nonce[0]);

    if (nonce_int == UINT32_MAX) {
        return LT_NONCE_OVERFLOW;
    }

    nonce_int++;

    nonce[3] = nonce_int >> 24;
    nonce[2] = (nonce_int & 0x00FF0000) >> 16;
    nonce[1] = (nonce_int & 0x0000FF00) >> 8;
    nonce[0] = (nonce_int & 0x000000FF);

    return LT_OK;
}

void lt_l3_invalidate_host_session_data(lt_l3_state_t *s3)
{
    s3->session = SESSION_OFF;
    memset(s3->encryption_IV, 0, sizeof(s3->encryption_IV));
    memset(s3->decryption_IV, 0, sizeof(s3->decryption_IV));
    memset(s3->encrypt, 0, sizeof(s3->encrypt));
    memset(s3->decrypt, 0, sizeof(s3->decrypt));
#if LT_SEPARATE_L3_BUFF
    memset(s3->buff, 0, s3->buff_len);
#else
    memset(s3->buff, 0, sizeof(s3->buff));
#endif
}

lt_ret_t lt_l3_encrypt_request(lt_l3_state_t *s3)
{
#ifdef LIBT_DEBUG
    if (!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if (s3->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)s3->buff;

    int ret = lt_aesgcm_encrypt(&s3->encrypt, s3->encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data,
                                p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        lt_l3_invalidate_host_session_data(s3);
        return ret;
    }

    return lt_l3_nonce_increase(s3->encryption_IV);
}

lt_ret_t lt_l3_decrypt_response(lt_l3_state_t *s3)
{
#ifdef LIBT_DEBUG
    if (!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if (s3->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)s3->buff;

    lt_ret_t ret = lt_aesgcm_decrypt(&s3->decrypt, s3->decryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data,
                                     p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE);
    if (ret != LT_OK) {
        lt_l3_invalidate_host_session_data(s3);
        return ret;
    }

    ret = lt_l3_nonce_increase(s3->decryption_IV);
    if (LT_OK != ret) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case L3_RESULT_FAIL:
            return LT_L3_FAIL;
        case L3_RESULT_UNAUTHORIZED:
            return LT_L3_UNAUTHORIZED;
        case L3_RESULT_INVALID_CMD:
            return LT_L3_INVALID_CMD;
        case L3_RESULT_OK:
            return LT_OK;
        case L3_PAIRING_KEY_EMPTY:
            return LT_L3_PAIRING_KEY_EMPTY;
        case L3_PAIRING_KEY_INVALID:
            return LT_L3_PAIRING_KEY_INVALID;
        case L3_ECC_INVALID_KEY:
            return LT_L3_ECC_INVALID_KEY;
        case L3_R_MEM_DATA_WRITE_WRITE_FAIL:
            return LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL;
        case L3_R_MEM_DATA_WRITE_SLOT_EXPIRED:
            return LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED;
        case L3_MCOUNTER_UPDATE_ERROR:
            return LT_L3_MCOUNTER_UPDATE_UPDATE_ERR;
        case L3_MCOUNTER_COUNTER_INVALID:
            return LT_L3_COUNTER_INVALID;
        default:
            return LT_FAIL;
    }
}
