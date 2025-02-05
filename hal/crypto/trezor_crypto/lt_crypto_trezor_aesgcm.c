/**
 * @file lt_crypto_trezor_aesgcm.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifdef LT_USE_TREZOR_CRYPTO
#include "aes/aes.h"
#include "aes/aesgcm.h"
#include "lt_aesgcm.h"
#include "libtropic_common.h"

int lt_aesgcm_init_and_key(void *ctx,
                    const uint8_t *key,
                    uint32_t key_len
                    )
{
    gcm_ctx *_ctx = (gcm_ctx*) ctx;

    int ret = gcm_init_and_key(key, key_len, _ctx);
    if(ret != RETURN_GOOD) {
        return LT_FAIL;
    }
    return LT_OK;
}

int lt_aesgcm_encrypt( void *ctx,
                    const uint8_t *iv,
                    uint32_t iv_len,
                    const uint8_t *aad,
                    uint32_t aad_len,
                    uint8_t *msg,
                    uint32_t msg_len,
                    uint8_t *tag,
                    uint32_t tag_len)
{
    gcm_ctx *_ctx = (gcm_ctx*) ctx;

    int ret = gcm_encrypt_message(iv, iv_len, aad, aad_len, msg, msg_len, tag, tag_len, _ctx);
    if(ret != RETURN_GOOD) {
        return LT_FAIL;
    }

    return LT_OK;;
}



int lt_aesgcm_decrypt(void *ctx,
                    const uint8_t *iv,
                    uint32_t iv_len,
                    const uint8_t *aad,
                    uint32_t aad_len,
                    uint8_t *msg,
                    uint32_t msg_len,
                    const uint8_t *tag,
                    uint32_t tag_len)
{
    gcm_ctx *_ctx = (gcm_ctx*) ctx;

    int ret = gcm_decrypt_message(iv, iv_len, aad, aad_len, msg, msg_len, tag, tag_len, _ctx);
    if(ret != RETURN_GOOD) {
        return LT_FAIL;
    }

    return LT_OK;
}

int lt_aesgcm_end(void *ctx)
{
    gcm_ctx *_ctx = (gcm_ctx*) ctx;
    int ret = gcm_end(_ctx);
    if(ret != RETURN_GOOD) {
        return LT_FAIL;
    }
    return LT_OK;
}

#endif
