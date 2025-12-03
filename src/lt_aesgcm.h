#ifndef LT_AES_GCM_H
#define LT_AES_GCM_H

/**
 * @file   lt_aesgcm.h
 * @brief  AES-GCM function declarations
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "libtropic_common.h"

/**
 * @brief Initializes AES-GCM encryption context with the given key.
 *
 * @param ctx         Crypto context structure
 * @param key         Key to initialize with
 * @param key_len     Length of the key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_encrypt_init(void *ctx, const uint8_t *key, const uint32_t key_len)
    __attribute__((warn_unused_result));

/**
 * @brief Initializes AES-GCM decryption context with the given key.
 *
 * @param ctx         Crypto context structure
 * @param key         Key to initialize with
 * @param key_len     Length of the key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_decrypt_init(void *ctx, const uint8_t *key, const uint32_t key_len)
    __attribute__((warn_unused_result));

/**
 * @brief Encrypts data and expects initialized context with valid keys.
 *
 * @param ctx               AES-GCM context structure
 * @param iv                Initialization vector
 * @param iv_len            Length of the initialization vector
 * @param add               Additional data
 * @param add_len           Length of additional data
 * @param plaintext         Input plaintext buffer
 * @param plaintext_len     Length of the plaintext in bytes
 * @param ciphertext        Output ciphertext buffer (data + tag will be stored here)
 * @param ciphertext_len    Length of the ciphertext buffer
 * @return                  LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_encrypt(void *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, const uint8_t *plaintext, const uint32_t plaintext_len,
                           uint8_t *ciphertext, const uint32_t ciphertext_len) __attribute__((warn_unused_result));

/**
 * @brief Decrypts data and expects initialized context with valid keys.
 *
 * @param ctx               AES-GCM context structure
 * @param iv                The initialisation vector
 * @param iv_len            Length of the initialization vector
 * @param add               Additional data
 * @param add_len           Length of additional data
 * @param ciphertext        Input ciphertext buffer (data + tag)
 * @param ciphertext_len    Length of the ciphertext buffer
 * @param plaintext         Buffer to store plaintext
 * @param plaintext_len     Length of the plaintext buffer
 * @return                  LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_decrypt(void *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, const uint8_t *ciphertext, const uint32_t ciphertext_len,
                           uint8_t *plaintext, const uint32_t plaintext_len) __attribute__((warn_unused_result));

/**
 * @brief Deinitializes AES-GCM encryption context.
 *
 * @param ctx  Crypto context structure
 * @return     LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_encrypt_deinit(void *ctx) __attribute__((warn_unused_result));

/**
 * @brief Deinitializes AES-GCM decryption context.
 *
 * @param ctx  Crypto context structure
 * @return     LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_aesgcm_decrypt_deinit(void *ctx) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_AES_GCM_H
