/**
 * @file lt_l3_process.c
 * @brief Layer 3 functions definitions
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "lt_l3_process.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_l2.h"
#include "libtropic_logging.h"
#include "lt_aesgcm.h"
#include "lt_crypto_common.h"
#include "lt_l1.h"
#include "lt_secure_memzero.h"

static lt_ret_t lt_l3_nonce_increase(uint8_t *nonce)
{
#ifdef LT_REDUNDANT_ARG_CHECK
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
    s3->session_status = LT_SECURE_SESSION_OFF;

    lt_secure_memzero(s3->encryption_IV, sizeof(s3->encryption_IV));
    lt_secure_memzero(s3->decryption_IV, sizeof(s3->decryption_IV));

    lt_ret_t ret = lt_crypto_ctx_deinit(s3->crypto_ctx);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_crypto_ctx_deinit() failed, ret=%d", ret);
    }

#if LT_SEPARATE_L3_BUFF
    lt_secure_memzero(s3->buff, s3->buff_len);
#else
    lt_secure_memzero(s3->buff, sizeof(s3->buff));
#endif
}

lt_ret_t lt_l3_encrypt_request(lt_l3_state_t *s3)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if (s3->session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)s3->buff;

    // p_frame->data is both input plaintext and output ciphertext buffer,
    // it is large enough to hold both plaintext and ciphertext + tag.
    int ret = lt_aesgcm_encrypt(s3->crypto_ctx, s3->encryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data,
                                p_frame->cmd_size, p_frame->data, p_frame->cmd_size + TR01_L3_TAG_SIZE);
    if (ret != LT_OK) {
        lt_l3_invalidate_host_session_data(s3);
        return ret;
    }

    return lt_l3_nonce_increase(s3->encryption_IV);
}

lt_ret_t lt_l3_decrypt_response(lt_l3_state_t *s3)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s3) {
        return LT_PARAM_ERR;
    }
#endif
    if (s3->session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)s3->buff;

    if (p_frame->cmd_size > TR01_L3_RES_CIPHERTEXT_MAX_SIZE) {
        lt_l3_invalidate_host_session_data(s3);
        return LT_L3_RES_SIZE_ERROR;
    }

    // This check makes sure the decryption function does not go past buffer bounds.
    if (TR01_L3_SIZE_SIZE + p_frame->cmd_size + TR01_L3_TAG_SIZE > s3->buff_len) {
        lt_l3_invalidate_host_session_data(s3);
        return LT_L3_BUFFER_TOO_SMALL;
    }

    lt_ret_t ret
        = lt_aesgcm_decrypt(s3->crypto_ctx, s3->decryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data,
                            p_frame->cmd_size + TR01_L3_TAG_SIZE, p_frame->data, p_frame->cmd_size);
    if (ret != LT_OK) {
        lt_l3_invalidate_host_session_data(s3);
        return ret;
    }

    ret = lt_l3_nonce_increase(s3->decryption_IV);
    if (LT_OK != ret) {
        return ret;
    }

    switch (p_frame->data[0]) {
        case TR01_L3_RESULT_FAIL:
            return LT_L3_FAIL;
        case TR01_L3_RESULT_UNAUTHORIZED:
            return LT_L3_UNAUTHORIZED;
        case TR01_L3_RESULT_INVALID_CMD:
            return LT_L3_INVALID_CMD;
        case TR01_L3_RESULT_OK:
            return LT_OK;
        case TR01_L3_RESULT_SLOT_EMPTY:
            return LT_L3_SLOT_EMPTY;
        case TR01_L3_RESULT_SLOT_INVALID:
            return LT_L3_SLOT_INVALID;
        case TR01_L3_RESULT_INVALID_KEY:
            return LT_L3_INVALID_KEY;
        case TR01_L3_RESULT_SLOT_NOT_EMPTY:
            return LT_L3_SLOT_NOT_EMPTY;
        case TR01_L3_RESULT_SLOT_EXPIRED:
            return LT_L3_SLOT_EXPIRED;
        case TR01_L3_RESULT_UPDATE_ERR:
            return LT_L3_UPDATE_ERR;
        case TR01_L3_RESULT_COUNTER_INVALID:
            return LT_L3_COUNTER_INVALID;
        case TR01_L3_RESULT_HARDWARE_FAIL:
            return LT_L3_HARDWARE_FAIL;
        default:
            return LT_L3_RESULT_UNKNOWN;
    }
}
