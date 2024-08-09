#ifndef TS_AES_GCM_H
#define TS_AES_GCM_H

/**
* @file   ts_aesgcm.h
* @brief  AESGCM function declarations
* @author Tropic Square s.r.o.
*/
#if USE_TREZOR_CRYPTO
#include "aes.h"
#include "aesgcm.h"
#endif

/** AES-GCM context structure */
typedef struct ts_aes_gcm_ctx
{
#if USE_TREZOR_CRYPTO
    gcm_ctx ctx;
#elif USE_MBEDTLS

#endif
} ts_aes_gcm_ctx_t;

/**
 * @details This function initializes AES GCM context with keys
 *
 * @param ctx         AESGCM context structure
 * @param key         The key value
 * @param key_len     Length of key in bytes
 * @return            TS_OK if success, otherwise returns other error code.
 */
int ts_aesgcm_init_and_key(void *ctx,
                           const uint8_t *key,
                           uint32_t key_len);

/**
 * @details This function decrypts data. It expect initialized context with valid keys.
 *
 * @param ctx         AESGCM context structure
 * @param iv          The initialisation vector
 * @param iv_len      Length of initialization vector in bytes
 * @param aad         The header buffer
 * @param aad_len     Length of header buffer in bytes
 * @param msg         Message buffer
 * @param msg_len     Length of message in bytes
 * @param tag         The tag buffer
 * @param tag_len     Length of tag buffer in bytes
 * @return            TS_OK if success, otherwise returns other error code.
 */
int ts_aesgcm_encrypt(void *ctx,
                      const uint8_t *iv,
                      uint32_t iv_len,
                      const uint8_t *aad,
                      uint32_t aad_len,
                      uint8_t *msg,
                      uint32_t msg_len,
                      uint8_t *tag,
                      uint32_t tag_len);

/**
 * @details This function decrypts data. It expect initialized context with valid keys.
 *
 * @param ctx         AESGCM context structure
 * @param iv          The initialisation vector
 * @param iv_len      Length of initialization vector in bytes
 * @param aad         The header buffer
 * @param aad_len     Length of header buffer in bytes
 * @param msg         Message buffer
 * @param msg_len     Length of message in bytes
 * @param tag         The tag buffer
 * @param tag_len     Length of tag buffer in bytes
 * @return            TS_OK if success, otherwise returns other error code.
 */
int ts_aesgcm_decrypt(void *ctx,
                      const uint8_t *iv,
                      uint32_t iv_len,
                      const uint8_t *aad,
                      uint32_t aad_len,
                      uint8_t *msg,
                      uint32_t msg_len,
                      const uint8_t *tag,
                      uint32_t tag_len);

/**
 * @details This function clears AES GCM context
 *
 * @param ctx         AESGCM context structure
 * @return            TS_OK if success, otherwise returns other error code.
 */
int ts_aesgcm_end(void *ctx);

#endif
