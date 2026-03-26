#include "app_cmd/pin_set.h"

#include <stdint.h>
#include <string.h>

#include "app_cmd/app_cmd_common.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "mbedtls/platform_util.h"
#include "psa/crypto.h"
#include "usb_devkit_messages.pb.h"

extern uint32_t __macandd_nv_start__;
macandd_data_t g_macandd_data = {0};

/**
 * @brief Simple XOR "encryption" function. Replace with another encryption algorithm if needed.
 *
 * @param data         32B of data to be encrypted
 * @param key          32B key used for encryption
 * @param destination  Buffer into which 32B of encrypted data will be placed
 */
static void encrypt(const uint8_t *data, const uint8_t *key, uint8_t *destination)
{
    for (uint8_t i = 0; i < 32; i++) {
        destination[i] = data[i] ^ key[i];
    }
}

/**
 * @brief PSA Crypto HMAC-SHA256 wrapper.
 *
 * @param key       Key data buffer
 * @param key_len   Length of data in key buffer
 * @param data      Data buffer
 * @param data_len  Length of data buffer
 * @param output    Output buffer for HMAC result
 * @return          psa_status_t
 */
static psa_status_t hmac_sha256(const uint8_t *key, const size_t key_len, const uint8_t *data,
                                const size_t data_len, uint8_t *output)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_status_t status;
    size_t output_len;

    // Set key attributes for HMAC.
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

    // Import key.
    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Compute HMAC.
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), data, data_len, output,
                             PSA_HASH_LENGTH(PSA_ALG_SHA_256), &output_len);

cleanup:
    if (key_id != 0) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&attributes);
    return status;
}

void pin_set(const PinSetCmd *cmd, AppResp *resp)
{
    if (cmd->attempts > TR01_MACANDD_ROUNDS_MAX) {
        resp->type.pin_set.res_code = PIN_SET_RESP_CODE_ATTEMPS_OUT_OF_RANGE;
        return;
    }

    // Assume error by default.
    resp->type.pin_set.res_code = PIN_SET_RESP_CODE_ERROR;

    // Variables used during the pin set process.
    // Using notation from this document:
    // https://github.com/tropicsquare/tropic01-rtl/blob/public/doc/theory/MAC-and-destroy-v2.pdf
    uint8_t t[PSA_HASH_LENGTH(PSA_ALG_SHA_256)], u[PSA_HASH_LENGTH(PSA_ALG_SHA_256)],
        ignored[TR01_MAC_AND_DESTROY_DATA_SIZE], v[PSA_HASH_LENGTH(PSA_ALG_SHA_256)],
        w[TR01_MAC_AND_DESTROY_DATA_SIZE], k_i[PSA_HASH_LENGTH(PSA_ALG_SHA_256)],
        kdf_key_zeros[256] = {0},
        pin_with_add_data[sizeof(cmd->new_pin) + sizeof(cmd->additional_data.bytes)];
    size_t pin_with_add_data_len;
    macandd_data_t macandd_data = {0};
    lt_ret_t lt_ret;
    psa_status_t psa_status;
    // HAL_StatusTypeDef hal_status;

    // 1. Generate a random 32B secret s.
    // The user gives us this secret, but it could be generated e.g. using TROPIC01 TRNG.
    // Notation: where the aforementioned document uses s, we use cmd->secret.

    // 2. Compute t = KDF(s, "0x00")).
    psa_status = hmac_sha256(cmd->secret, sizeof(cmd->secret), (uint8_t *)"0", 1, t);
    if (psa_status != PSA_SUCCESS) {
        goto cleanup;
    }

    // 3. Initialize depleted attempts to 0 and save tag t.
    macandd_data.depleted_attempts = TR01_MAC_AND_DESTROY_SLOT_0;
    memcpy(macandd_data.auth_tag, t, sizeof(macandd_data.auth_tag));

    // 4. Compute u = KDF(s, "0x01")).
    psa_status = hmac_sha256(cmd->secret, sizeof(cmd->secret), (uint8_t *)"1", 1, u);
    if (psa_status != PSA_SUCCESS) {
        goto cleanup;
    }

    // 5. Compute v = KDF(0^256, PIN || A).
    pin_with_add_data_len = strlen(cmd->new_pin);
    memcpy(pin_with_add_data, cmd->new_pin, pin_with_add_data_len);
    if (cmd->has_additional_data) {
        memcpy(pin_with_add_data + pin_with_add_data_len, cmd->additional_data.bytes,
               cmd->additional_data.size);
        pin_with_add_data_len += cmd->additional_data.size;
    }
    psa_status = hmac_sha256(kdf_key_zeros, sizeof(kdf_key_zeros), pin_with_add_data,
                             pin_with_add_data_len, v);
    if (psa_status != PSA_SUCCESS) {
        goto cleanup;
    }

    // 6. n (number of attempts to setup) MAC-and-destroy operations.
    for (lt_mac_and_destroy_slot_t i = 0; i < (lt_mac_and_destroy_slot_t)cmd->attempts; i++) {
        // 6.1 Execute MAC-and-destroy(i, u) and ignore output.
        lt_ret = lt_mac_and_destroy(&lt_handle, i, u, ignored);
        if (lt_ret != LT_OK) {
            goto libtropic_error;
        }
        // 6.2 Execute MAC-and-destroy(i, v) and store output to w.
        lt_ret = lt_mac_and_destroy(&lt_handle, i, v, w);
        if (lt_ret != LT_OK) {
            goto libtropic_error;
        }
        // 6.3 Execute MAC-and-destroy(i, u) and ignore output.
        lt_ret = lt_mac_and_destroy(&lt_handle, i, u, ignored);
        if (lt_ret != LT_OK) {
            goto libtropic_error;
        }
        // 6.4 Compute k_i = KDF(w, PIN || A).
        psa_status = hmac_sha256(w, sizeof(w), pin_with_add_data, pin_with_add_data_len, k_i);
        if (psa_status != PSA_SUCCESS) {
            goto cleanup;
        }
        // 6.5 Encrypt s using k_i as the key and save the ciphertext.
        // TODO: use AES
        encrypt(cmd->secret, k_i, macandd_data.ciphertexts[i]);
    }

    // 7. Compute k = KDF(s, "0x02").
    psa_status = hmac_sha256(cmd->secret, sizeof(cmd->secret), (uint8_t *)"2", 1,
                             resp->type.pin_set.crypto_key);
    if (psa_status != PSA_SUCCESS) {
        goto cleanup;
    }

    // 9. Store MAC-and-destroy non-confidential data in non-volatile memory.
    // hal_status = macandd_data_store(&macandd_data);
    // if (hal_status != HAL_OK) {
    //     resp->type.pin_set.res_code = PIN_SET_RESP_CODE_FLASH_WRITE_ERROR;
    // }
    memcpy(&g_macandd_data, &macandd_data, sizeof(macandd_data_t));
    resp->type.pin_set.res_code = PIN_SET_RESP_CODE_OK;
    goto cleanup;

libtropic_error:
    resp->has_libtropic_error_code = true;
    resp->libtropic_error_code = lt_ret;
    // 8. Return k and purge all other computed values from RAM.
    // We should also clear the command contents, but that is not done here.
cleanup:
    mbedtls_platform_zeroize(t, sizeof(t));
    mbedtls_platform_zeroize(u, sizeof(u));
    mbedtls_platform_zeroize(ignored, sizeof(ignored));
    mbedtls_platform_zeroize(v, sizeof(v));
    mbedtls_platform_zeroize(w, sizeof(w));
    mbedtls_platform_zeroize(k_i, sizeof(k_i));
    mbedtls_platform_zeroize(pin_with_add_data, sizeof(pin_with_add_data));
}