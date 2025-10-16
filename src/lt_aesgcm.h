#ifndef LT_AES_GCM_H
#define LT_AES_GCM_H

/**
 * @file   lt_aesgcm.h
 * @brief  AES-GCM function declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "libtropic_common.h"
#include "lt_crypto_macros.h"

/**
 * @brief Initializes AES-GCM context with the given key.
 *
 * @param ctx         AES-GCM context structure
 * @param key         The key value
 * @param key_len     Length of key in bytes
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_init_and_key(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *key, const uint32_t key_len)
    __attribute__((warn_unused_result));

/**
 * @brief Encrypts data and expects initialized context with valid keys.
 *
 * @param ctx         AES-GCM context structure
 * @param iv          Initialization vector
 * @param iv_len      Length of the initialization vector
 * @param add         Additional data
 * @param add_len     Length of additional data
 * @param msg         Message buffer
 * @param msg_len     Length of message in bytes
 * @param tag         Tag buffer
 * @param tag_len     Length of the tag buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_encrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, uint8_t *tag,
                           const uint32_t tag_len) __attribute__((warn_unused_result));

/**
 * @brief Decrypts data and expects initialized context with valid keys.
 *
 * @param ctx         AES-GCM context structure
 * @param iv          The initialisation vector
 * @param iv_len      Length of the initialization vector
 * @param add         Additional data
 * @param add_len     Length of additional data
 * @param msg         Message buffer
 * @param msg_len     Length of message in bytes
 * @param tag         Tag buffer
 * @param tag_len     Length of the tag buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_decrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, const uint8_t *tag,
                           const uint32_t tag_len) __attribute__((warn_unused_result));

/**
 * @brief Clears AES-GCM context.
 *
 * @param ctx         AES-GCM context structure
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_end(LT_CRYPTO_AES_GCM_CTX_T *ctx) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_AES_GCM_H
