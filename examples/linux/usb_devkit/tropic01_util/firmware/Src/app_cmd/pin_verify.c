#include "app_cmd/pin_verify.h"

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

/**
 * @brief Simple XOR "decryption" function. Replace with another decryption algorithm if needed.
 *
 * @param data         32B of data to be decrypted
 * @param key          32B key used for decryption
 * @param destination  Buffer into which 32B of decrypted data will be placed
 */
static void decrypt(const uint8_t *data, const uint8_t *key, uint8_t *destination)
{
    for (uint8_t i = 0; i < 32; i++) {
        destination[i] = data[i] ^ key[i];
    }
}

void pin_verify(const PinVerifyCmd *cmd, AppResp *resp)
{
    // Assume error by default.
    resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_ERROR;

    // Variables used during the pin verification process.
    // Using notation from this document:
    // https://github.com/tropicsquare/tropic01-rtl/blob/public/doc/theory/MAC-and-destroy-v2.pdf
    uint8_t s[TR01_MAC_AND_DESTROY_DATA_SIZE], t[PSA_HASH_LENGTH(PSA_ALG_SHA_256)],
        u[PSA_HASH_LENGTH(PSA_ALG_SHA_256)], ignored[TR01_MAC_AND_DESTROY_DATA_SIZE],
        v[PSA_HASH_LENGTH(PSA_ALG_SHA_256)], w[TR01_MAC_AND_DESTROY_DATA_SIZE],
        k_i[PSA_HASH_LENGTH(PSA_ALG_SHA_256)],
        kdf_key_zeros[32] = {0},
        pin_with_add_data[sizeof(cmd->pin) + sizeof(cmd->additional_data.bytes)];
    size_t pin_with_add_data_len;
    macandd_data_t macandd_data = {0};
    lt_ret_t lt_ret;
    HAL_StatusTypeDef hal_status;

    // 1. Load MAC-and-destroy non-confidential data from non-volatile memory.
    // TODO: read from flash
    memcpy(&macandd_data, &g_macandd_data, sizeof(macandd_data_t));

    // 2. Check depleted attempts.
    if (macandd_data.depleted_attempts >= TR01_MACANDD_ROUNDS_MAX) {
        resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_DEPLETED_ATTEMPS_OUT_OF_RANGE;
        return;
    }

    // 3. Increment depleted attempts.
    macandd_data.depleted_attempts++;

    // 4. Compute v = KDF(0^256, PIN || A).
    pin_with_add_data_len = strlen(cmd->pin);
    memcpy(pin_with_add_data, cmd->pin, pin_with_add_data_len);
    if (cmd->has_additional_data) {
        memcpy(pin_with_add_data + pin_with_add_data_len, cmd->additional_data.bytes,
               cmd->additional_data.size);
        pin_with_add_data_len += cmd->additional_data.size;
    }
    hal_status = hmac_sha256(kdf_key_zeros, sizeof(kdf_key_zeros), pin_with_add_data,
                             pin_with_add_data_len, v);
    if (hal_status != HAL_OK) {
        resp->type.pin_verify.res_code = PIN_SET_RESP_CODE_KDF_ERROR;
        goto cleanup;
    }

    // 5. Execute MAC-and-destroy(i, v) and store output w.
    // i = depleted attemps - 1, because we start from M&D slot 0.
    lt_ret = lt_mac_and_destroy(&lt_handle, macandd_data.depleted_attempts - 1, v, w);
    if (lt_ret != LT_OK) {
        goto libtropic_error;
    }

    // Do step 3. (store depleted attempts back to NVM after increment) here, because only now has the
    // attempt been really depleted (M&D slot destroyed).

    // hal_status = macandd_data_store(&macandd_data);
    // if (hal_status != HAL_OK) {
    //     resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_FLASH_WRITE_ERROR;
    //     goto cleanup;
    // }
    memcpy(&g_macandd_data, &macandd_data, sizeof(macandd_data_t));

    // 6. Compute k_i = KDF(w, PIN || A).
    hal_status = hmac_sha256(w, sizeof(w), pin_with_add_data, pin_with_add_data_len, k_i);
    if (hal_status != HAL_OK) {
        resp->type.pin_verify.res_code = PIN_SET_RESP_CODE_KDF_ERROR;
        goto cleanup;
    }

    // 7., 8. Decrypt c_i using k_i as the key to obtain the plaintext s.
    // We are using simple XOR here. In production, replace it with some decryption standard!
    decrypt(macandd_data.ciphertexts[macandd_data.depleted_attempts - 1], k_i, s);

    // 9. Compute t = KDF(s, "0x00").
    hal_status = hmac_sha256(s, sizeof(s), (uint8_t *)"0", 1, t);
    if (hal_status != HAL_OK) {
        resp->type.pin_verify.res_code = PIN_SET_RESP_CODE_KDF_ERROR;
        goto cleanup;
    }

    // 10. Compare t to the stored tag.
    if (memcmp(t, macandd_data.auth_tag, sizeof(t)) != 0) {
        resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_WRONG_PIN;
        goto cleanup;
    }

    // 11. Compute u = KDF(s, "0x01").
    hal_status = hmac_sha256(s, sizeof(s), (uint8_t *)"1", 1, u);
    if (hal_status != HAL_OK) {
        resp->type.pin_verify.res_code = PIN_SET_RESP_CODE_KDF_ERROR;
        goto cleanup;
    }

    // 12. For j in {1..i} execute MAC-and-destroy(j, u) and ignore output.
    for (lt_mac_and_destroy_slot_t j = 0; j < macandd_data.depleted_attempts; j++) {
        lt_ret = lt_mac_and_destroy(&lt_handle, j, u, ignored);
        if (lt_ret != LT_OK) {
            goto libtropic_error;
        }
    }

    // 13. Reset depleted attempts and store into non-volatile memory.
    macandd_data.depleted_attempts = 0;
    // hal_status = macandd_data_store(&macandd_data);
    // if (hal_status != HAL_OK) {
    //     resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_FLASH_WRITE_ERROR;
    //     goto cleanup;
    // }
    memcpy(&g_macandd_data, &macandd_data, sizeof(macandd_data_t));

    // 14. Compute k = KDF(s, "0x02").
    hal_status = hmac_sha256(s, sizeof(s), (uint8_t *)"2", 1, resp->type.pin_verify.crypto_key);
    if (hal_status != HAL_OK) {
        resp->type.pin_verify.res_code = PIN_SET_RESP_CODE_KDF_ERROR;
        goto cleanup;
    }

    resp->type.pin_verify.res_code = PIN_VERIFY_RESP_CODE_OK;
    goto cleanup;

libtropic_error:
    resp->has_libtropic_res_code = true;
    resp->libtropic_res_code = lt_ret;

    // 15. Return k and purge all other computed values from RAM.
    // We should also clear the command contents, but that is not done here.
cleanup:
    mbedtls_platform_zeroize(s, sizeof(s));
    mbedtls_platform_zeroize(t, sizeof(t));
    mbedtls_platform_zeroize(u, sizeof(u));
    mbedtls_platform_zeroize(ignored, sizeof(ignored));
    mbedtls_platform_zeroize(v, sizeof(v));
    mbedtls_platform_zeroize(w, sizeof(w));
    mbedtls_platform_zeroize(k_i, sizeof(k_i));
    mbedtls_platform_zeroize(pin_with_add_data, sizeof(pin_with_add_data));
}