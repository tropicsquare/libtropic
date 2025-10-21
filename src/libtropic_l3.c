#include "libtropic_l3.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_l2.h"
#include "libtropic_port.h"
#include "lt_aesgcm.h"
#include "lt_ed25519.h"
#include "lt_hkdf.h"
#include "lt_l1.h"
#include "lt_l1_port_wrap.h"
#include "lt_l2_api_structs.h"
#include "lt_l3_api_structs.h"
#include "lt_l3_process.h"
#include "lt_random.h"
#include "lt_sha256.h"
#include "lt_x25519.h"

lt_ret_t lt_out__session_start(lt_handle_t *h, const lt_pkey_index_t pkey_index, lt_host_eph_keys_t *host_eph_keys)
{
    if (!h || (pkey_index > TR01_PAIRING_KEY_SLOT_INDEX_3) || !host_eph_keys) {
        return LT_PARAM_ERR;
    }

    // In case we reuse handle and use separate l3 buffer, we need to ensure that IV's are zeroed,
    // because on session start we expect IV's to be 0. It does not hurt to zero them anyway on session start.
    memset(h->l3.encryption_IV, 0, sizeof(h->l3.encryption_IV));
    memset(h->l3.decryption_IV, 0, sizeof(h->l3.decryption_IV));

    // Create ephemeral host keys
    lt_ret_t ret = lt_random_bytes(h, host_eph_keys->ehpriv, sizeof(host_eph_keys->ehpriv));
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_X25519_scalarmult(host_eph_keys->ehpriv, host_eph_keys->ehpub);
    if (ret != LT_OK) {
        return ret;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_req_t *p_req = (struct lt_l2_handshake_req_t *)h->l2.buff;

    p_req->req_id = TR01_L2_HANDSHAKE_REQ_ID;
    p_req->req_len = TR01_L2_HANDSHAKE_REQ_LEN;
    memcpy(p_req->e_hpub, host_eph_keys->ehpub, TR01_EHPUB_LEN);

    p_req->pkey_index = (uint8_t)pkey_index;

    return LT_OK;
}

lt_ret_t lt_in__session_start(lt_handle_t *h, const uint8_t *stpub, const lt_pkey_index_t pkey_index,
                              const uint8_t *shipriv, const uint8_t *shipub, lt_host_eph_keys_t *host_eph_keys)
{
    if (!h || !stpub || (pkey_index > TR01_PAIRING_KEY_SLOT_INDEX_3) || !shipriv || !shipub || !host_eph_keys) {
        return LT_PARAM_ERR;
    }

    // Setup a response pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_rsp_t *p_rsp = (struct lt_l2_handshake_rsp_t *)h->l2.buff;

    // Noise_KK1_25519_AESGCM_SHA256\x00\x00\x00
    uint8_t protocol_name[32] = {'N', 'o', 'i', 's', 'e', '_', 'K', 'K', '1', '_', '2', '5', '5', '1',  '9',  '_',
                                 'A', 'E', 'S', 'G', 'C', 'M', '_', 'S', 'H', 'A', '2', '5', '6', 0x00, 0x00, 0x00};
    uint8_t hash[LT_SHA256_DIGEST_LENGTH] = {0};
    LT_CRYPTO_SHA256_CTX_T sha256_ctx;
    lt_ret_t ret;

    // h = SHA_256(protocol_name)
    ret = lt_sha256_init(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, protocol_name, sizeof(protocol_name));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // h = SHA256(h||SHiPUB)
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, hash, sizeof(hash));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, shipub, TR01_SHIPUB_LEN);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // h = SHA256(h||STPUB)
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, hash, sizeof(hash));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, stpub, TR01_STPUB_LEN);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // h = SHA256(h||EHPUB)
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, hash, sizeof(hash));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, host_eph_keys->ehpub, TR01_EHPUB_LEN);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // h = SHA256(h||PKEY_INDEX)
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, hash, sizeof(hash));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, (uint8_t *)&pkey_index, 1);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // h = SHA256(h||ETPUB)
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, hash, sizeof(hash));
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, p_rsp->e_tpub, TR01_ETPUB_LEN);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, hash);
    if (ret != LT_OK) {
        return ret;
    }

    // ck = protocol_name
    uint8_t output_1[33] = {0};  // Temp storage for ck, kcmd.
    uint8_t output_2[32] = {0};  // Temp storage for kauth.
    // ck = HKDF (ck, X25519(EHPRIV, ETPUB), 1)
    uint8_t shared_secret[TR01_X25519_KEY_LEN] = {0};
    ret = lt_X25519(host_eph_keys->ehpriv, p_rsp->e_tpub, shared_secret);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_hkdf(protocol_name, sizeof(protocol_name), shared_secret, sizeof(shared_secret), 1, output_1, output_2);
    if (ret != LT_OK) {
        return ret;
    }
    // ck = HKDF (ck, X25519(SHiPRIV, ETPUB), 1)
    ret = lt_X25519(shipriv, p_rsp->e_tpub, shared_secret);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_hkdf(output_1, sizeof(output_1), shared_secret, sizeof(output_2), 1, output_1, output_2);
    if (ret != LT_OK) {
        return ret;
    }
    // ck, kAUTH = HKDF (ck, X25519(EHPRIV, STPUB), 2)
    ret = lt_X25519(host_eph_keys->ehpriv, stpub, shared_secret);
    if (ret != LT_OK) {
        return ret;
    }
    uint8_t kauth[TR01_AES256_KEY_LEN] = {0};  // AES256 key used for handshake authentication.
    ret = lt_hkdf(output_1, sizeof(output_1), shared_secret, sizeof(shared_secret), 2, output_1, kauth);
    if (ret != LT_OK) {
        return ret;
    }
    // kCMD, kRES = HKDF (ck, emptystring, 2)
    uint8_t kcmd[TR01_AES256_KEY_LEN] = {0};  // AES256 key used for L3 command packet encryption/decryption.
    uint8_t kres[TR01_AES256_KEY_LEN] = {0};  // AES256 key used for L3 result packet encryption/decryption.
    ret = lt_hkdf(output_1, sizeof(output_1), (uint8_t *)"", 0, 2, kcmd, kres);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_aesgcm_init_and_key(&h->l3.aesgcm_decrypt_ctx, kauth, sizeof(kauth));
    if (ret != LT_OK) {
        goto exit;
    }

    ret = lt_aesgcm_decrypt(&h->l3.aesgcm_decrypt_ctx, h->l3.decryption_IV, sizeof(h->l3.decryption_IV), hash,
                            sizeof(hash), (uint8_t *)"", 0, p_rsp->t_tauth, sizeof(p_rsp->t_tauth));
    if (ret != LT_OK) {
        goto exit;
    }

    ret = lt_aesgcm_init_and_key(&h->l3.aesgcm_encrypt_ctx, kcmd, sizeof(kcmd));
    if (ret != LT_OK) {
        goto exit;
    }

    ret = lt_aesgcm_init_and_key(&h->l3.aesgcm_decrypt_ctx, kres, sizeof(kres));
    if (ret != LT_OK) {
        goto exit;
    }

    h->l3.session_status = LT_SECURE_SESSION_ON;

    return LT_OK;

// If something went wrong during session keys establishment, better clean up AES GCM contexts
exit:
    memset(&h->l3.aesgcm_encrypt_ctx, 0, sizeof(h->l3.aesgcm_encrypt_ctx));
    memset(&h->l3.aesgcm_decrypt_ctx, 0, sizeof(h->l3.aesgcm_decrypt_ctx));

    return ret;
}

lt_ret_t lt_out__ping(lt_handle_t *h, const uint8_t *msg_out, const uint16_t msg_len)
{
    if (!h || !msg_out || (msg_len > TR01_PING_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ping_cmd_t *p_l3_cmd = (struct lt_l3_ping_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = msg_len + TR01_L3_PING_CMD_SIZE_MIN;
    p_l3_cmd->cmd_id = TR01_L3_PING_CMD_ID;
    memcpy(p_l3_cmd->data_in, msg_out, msg_len);

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ping(lt_handle_t *h, uint8_t *msg_in, const uint16_t msg_len)
{
    if (!h || !msg_in || (msg_len > TR01_PING_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ping_res_t *p_l3_res = (struct lt_l3_ping_res_t *)h->l3.buff;

    // Check incomming l3 length
    if ((TR01_L3_PING_CMD_SIZE_MIN + msg_len) != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(msg_in, p_l3_res->data_out, msg_len);

    return LT_OK;
}

lt_ret_t lt_out__pairing_key_write(lt_handle_t *h, const uint8_t *pairing_pub, const uint8_t slot)
{
    if (!h || !pairing_pub || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_write_cmd_t *p_l3_cmd = (struct lt_l3_pairing_key_write_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_PAIRING_KEY_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_PAIRING_KEY_WRITE_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->s_hipub, pairing_pub, sizeof(p_l3_cmd->s_hipub));

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__pairing_key_write(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_write_res_t *p_l3_res = (struct lt_l3_pairing_key_write_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_PAIRING_KEY_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__pairing_key_read(lt_handle_t *h, const uint8_t slot)
{
    if (!h || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_read_cmd_t *p_l3_cmd = (struct lt_l3_pairing_key_read_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_PAIRING_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_PAIRING_KEY_READ_CMD_ID;
    p_l3_cmd->slot = slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__pairing_key_read(lt_handle_t *h, uint8_t *pubkey)
{
    if (!h || !pubkey) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_read_res_t *p_l3_res = (struct lt_l3_pairing_key_read_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_PAIRING_KEY_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(pubkey, p_l3_res->s_hipub, TR01_SHIPUB_LEN);

    return LT_OK;
}

lt_ret_t lt_out__pairing_key_invalidate(lt_handle_t *h, const uint8_t slot)
{
    if (!h || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_invalidate_cmd_t *p_l3_cmd = (struct lt_l3_pairing_key_invalidate_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_PAIRING_KEY_INVALIDATE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_PAIRING_KEY_INVALIDATE_CMD_ID;
    // cmd data
    p_l3_cmd->slot = slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__pairing_key_invalidate(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_invalidate_res_t *p_l3_res = (struct lt_l3_pairing_key_invalidate_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_PAIRING_KEY_INVALIDATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

static bool conf_addr_valid(enum lt_config_obj_addr_t addr)
{
    bool valid = false;

    switch (addr) {
        case TR01_CFG_START_UP_ADDR:
        case TR01_CFG_SENSORS_ADDR:
        case TR01_CFG_DEBUG_ADDR:
        case TR01_CFG_GPO_ADDR:
        case TR01_CFG_SLEEP_MODE_ADDR:
        case TR01_CFG_UAP_PAIRING_KEY_WRITE_ADDR:
        case TR01_CFG_UAP_PAIRING_KEY_READ_ADDR:
        case TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR:
        case TR01_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR:
        case TR01_CFG_UAP_R_CONFIG_READ_ADDR:
        case TR01_CFG_UAP_I_CONFIG_WRITE_ADDR:
        case TR01_CFG_UAP_I_CONFIG_READ_ADDR:
        case TR01_CFG_UAP_PING_ADDR:
        case TR01_CFG_UAP_R_MEM_DATA_WRITE_ADDR:
        case TR01_CFG_UAP_R_MEM_DATA_READ_ADDR:
        case TR01_CFG_UAP_R_MEM_DATA_ERASE_ADDR:
        case TR01_CFG_UAP_RANDOM_VALUE_GET_ADDR:
        case TR01_CFG_UAP_ECC_KEY_GENERATE_ADDR:
        case TR01_CFG_UAP_ECC_KEY_STORE_ADDR:
        case TR01_CFG_UAP_ECC_KEY_READ_ADDR:
        case TR01_CFG_UAP_ECC_KEY_ERASE_ADDR:
        case TR01_CFG_UAP_ECDSA_SIGN_ADDR:
        case TR01_CFG_UAP_EDDSA_SIGN_ADDR:
        case TR01_CFG_UAP_MCOUNTER_INIT_ADDR:
        case TR01_CFG_UAP_MCOUNTER_GET_ADDR:
        case TR01_CFG_UAP_MCOUNTER_UPDATE_ADDR:
        case TR01_CFG_UAP_MAC_AND_DESTROY_ADDR:
            valid = true;
    }
    return valid;
}

lt_ret_t lt_out__r_config_write(lt_handle_t *h, const enum lt_config_obj_addr_t addr, const uint32_t obj)
{
    if (!h || !conf_addr_valid(addr)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_cmd_t *p_l3_cmd = (struct lt_l3_r_config_write_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_R_CONFIG_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_R_CONFIG_WRITE_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->value = obj;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_config_write(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_res_t *p_l3_res = (struct lt_l3_r_config_write_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_R_CONFIG_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__r_config_read(lt_handle_t *h, const enum lt_config_obj_addr_t addr)
{
    if (!h || !conf_addr_valid(addr)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_cmd_t *p_l3_cmd = (struct lt_l3_r_config_read_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_R_CONFIG_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_R_CONFIG_READ_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_config_read(lt_handle_t *h, uint32_t *obj)
{
    if (!h || !obj) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_res_t *p_l3_res = (struct lt_l3_r_config_read_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_R_CONFIG_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *obj = p_l3_res->value;

    return LT_OK;
}

lt_ret_t lt_out__r_config_erase(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_cmd_t *p_l3_cmd = (struct lt_l3_r_config_erase_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_R_CONFIG_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_R_CONFIG_ERASE_CMD_ID;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_config_erase(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_res_t *p_l3_res = (struct lt_l3_r_config_erase_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_R_CONFIG_ERASE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__i_config_write(lt_handle_t *h, const enum lt_config_obj_addr_t addr, const uint8_t bit_index)
{
    if (!h || !conf_addr_valid(addr) || (bit_index > 31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_cmd_t *p_l3_cmd = (struct lt_l3_i_config_write_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_I_CONFIG_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_I_CONFIG_WRITE_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->bit_index = bit_index;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__i_config_write(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_res_t *p_l3_res = (struct lt_l3_i_config_write_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_I_CONFIG_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__i_config_read(lt_handle_t *h, const enum lt_config_obj_addr_t addr)
{
    if (!h || !conf_addr_valid(addr)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_cmd_t *p_l3_cmd = (struct lt_l3_i_config_read_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_I_CONFIG_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_I_CONFIG_READ_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__i_config_read(lt_handle_t *h, uint32_t *obj)
{
    if (!h || !obj) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_res_t *p_l3_res = (struct lt_l3_i_config_read_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_I_CONFIG_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *obj = p_l3_res->value;

    return LT_OK;
}

lt_ret_t lt_out__r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, const uint8_t *data,
                                  const uint16_t data_size)
{
    if (!h || !data || data_size < TR01_R_MEM_DATA_SIZE_MIN || data_size > TR01_R_MEM_DATA_SIZE_MAX
        || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_write_cmd_t *p_l3_cmd = (struct lt_l3_r_mem_data_write_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = data_size + 4;
    p_l3_cmd->cmd_id = TR01_L3_R_MEM_DATA_WRITE_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;
    memcpy(p_l3_cmd->data, data, data_size);

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_mem_data_write(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_write_res_t *p_l3_res = (struct lt_l3_r_mem_data_write_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_R_MEM_DATA_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot)
{
    if (!h || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_read_cmd_t *p_l3_cmd = (struct lt_l3_r_mem_data_read_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_R_MEM_DATA_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_R_MEM_DATA_READ_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_mem_data_read(lt_handle_t *h, uint8_t *data, const uint16_t data_max_size, uint16_t *data_read_size)
{
    if (!h || !data || !data_read_size) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_read_res_t *p_l3_res = (struct lt_l3_r_mem_data_read_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if ((p_l3_res->res_size < TR01_L3_R_MEM_DATA_READ_RES_SIZE_MIN)
        || p_l3_res->res_size > TR01_L3_R_MEM_DATA_READ_RES_SIZE_MAX) {
        return LT_FAIL;
    }

    // Get read data size
    // TODO: If FW implements fail error code on R_Mem_Data_Read from empty slot, this can be removed.
    *data_read_size = p_l3_res->res_size - sizeof(p_l3_res->result) - sizeof(p_l3_res->padding);

    // Check if slot is not empty
    if (*data_read_size == 0) {
        return LT_L3_R_MEM_DATA_READ_SLOT_EMPTY;
    }

    // Check if the output buffer for the read data is big enough
    if (data_max_size < *data_read_size) {
        return LT_PARAM_ERR;
    }

    memcpy(data, p_l3_res->data, *data_read_size);

    return LT_OK;
}

lt_ret_t lt_out__r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot)
{
    if (!h || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_erase_cmd_t *p_l3_cmd = (struct lt_l3_r_mem_data_erase_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_R_MEM_DATA_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_R_MEM_DATA_ERASE_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__r_mem_data_erase(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_erase_res_t *p_l3_res = (struct lt_l3_r_mem_data_erase_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_R_MEM_DATA_ERASE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__random_value_get(lt_handle_t *h, const uint16_t rnd_bytes_cnt)
{
    if ((rnd_bytes_cnt > TR01_RANDOM_VALUE_GET_LEN_MAX) || !h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_random_value_get_cmd_t *p_l3_cmd = (struct lt_l3_random_value_get_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_RANDOM_VALUE_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_RANDOM_VALUE_GET_CMD_ID;
    p_l3_cmd->n_bytes = rnd_bytes_cnt;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__random_value_get(lt_handle_t *h, uint8_t *rnd_bytes, const uint16_t rnd_bytes_cnt)
{
    if (!h || !rnd_bytes || (rnd_bytes_cnt > TR01_RANDOM_VALUE_GET_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_random_value_get_res_t *p_l3_res = (struct lt_l3_random_value_get_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incoming L3 length. The size is always equal to the number of requested random bytes + 4,
    // where '4' is padding (3 bytes) + result status (1 byte).
    if (TR01_L3_RANDOM_VALUE_GET_RES_SIZE_MIN + rnd_bytes_cnt != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    // Here we copy only random bytes, excluding padding and result status, hence using len from the
    // parameter. Note: p_l3_res->res_size could be used as well if we subtract TR01_L3_RANDOM_VALUE_GET_RES_SIZE_MIN.
    memcpy(rnd_bytes, p_l3_res->random_data, rnd_bytes_cnt);

    return LT_OK;
}

lt_ret_t lt_out__ecc_key_generate(lt_handle_t *h, const lt_ecc_slot_t slot, const lt_ecc_curve_type_t curve)
{
    if (!h || (slot > TR01_ECC_SLOT_31) || ((curve != TR01_CURVE_P256) && (curve != TR01_CURVE_ED25519))) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_generate_cmd_t *p_l3_cmd = (struct lt_l3_ecc_key_generate_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_ECC_KEY_GENERATE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_ECC_KEY_GENERATE_CMD_ID;
    p_l3_cmd->slot = (uint8_t)slot;
    p_l3_cmd->curve = (uint8_t)curve;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_key_generate(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_generate_res_t *p_l3_res = (struct lt_l3_ecc_key_generate_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_ECC_KEY_GENERATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__ecc_key_store(lt_handle_t *h, const lt_ecc_slot_t slot, const lt_ecc_curve_type_t curve,
                               const uint8_t *key)
{
    if (!h || (slot > TR01_ECC_SLOT_31) || ((curve != TR01_CURVE_P256) && (curve != TR01_CURVE_ED25519)) || !key) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_store_cmd_t *p_l3_cmd = (struct lt_l3_ecc_key_store_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_ECC_KEY_STORE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_ECC_KEY_STORE_CMD_ID;
    p_l3_cmd->slot = slot;
    p_l3_cmd->curve = curve;
    memcpy(p_l3_cmd->k, key, TR01_CURVE_PRIVKEY_LEN);

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_key_store(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_store_res_t *p_l3_res = (struct lt_l3_ecc_key_store_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_ECC_KEY_STORE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

// lt_ret_t lt_ecc_key_read(lt_handle_t *h, const lt_ecc_slot_t slot, uint8_t *key, const uint8_t keylen,
// lt_ecc_curve_type_t *curve, lt_ecc_key_origin_t *origin)
lt_ret_t lt_out__ecc_key_read(lt_handle_t *h, const lt_ecc_slot_t slot)
{
    if (!h || (slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_read_cmd_t *p_l3_cmd = (struct lt_l3_ecc_key_read_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_ECC_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_ECC_KEY_READ_CMD_ID;
    p_l3_cmd->slot = slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_key_read(lt_handle_t *h, uint8_t *key, const uint8_t key_max_size, lt_ecc_curve_type_t *curve,
                             lt_ecc_key_origin_t *origin)
{
    if (!h || !key || !curve || !origin) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_read_res_t *p_l3_res = (struct lt_l3_ecc_key_read_res_t *)h->l3.buff;

    *curve = p_l3_res->curve;
    *origin = p_l3_res->origin;

    size_t pubkey_size_in_result = p_l3_res->res_size - sizeof(p_l3_res->result) - sizeof(p_l3_res->curve)
                                   - sizeof(p_l3_res->origin) - sizeof(p_l3_res->padding);
    if (p_l3_res->curve == (uint8_t)TR01_CURVE_ED25519) {
        // Check incoming L3 length
        if (pubkey_size_in_result != TR01_CURVE_ED25519_PUBKEY_LEN) {
            return LT_FAIL;
        }

        // Check if the output buffer for the key is big enough
        if (key_max_size < TR01_CURVE_ED25519_PUBKEY_LEN) {
            return LT_PARAM_ERR;
        }

        memcpy(key, p_l3_res->pub_key, TR01_CURVE_ED25519_PUBKEY_LEN);
    }
    else if (p_l3_res->curve == (uint8_t)TR01_CURVE_P256) {
        // Check incoming L3 length
        if (pubkey_size_in_result != TR01_CURVE_P256_PUBKEY_LEN) {
            return LT_FAIL;
        }

        // Check if the output buffer for the key is big enough
        if (key_max_size < TR01_CURVE_P256_PUBKEY_LEN) {
            return LT_PARAM_ERR;
        }

        memcpy(key, p_l3_res->pub_key, TR01_CURVE_P256_PUBKEY_LEN);
    }
    else {
        // Unknown curve type
        return LT_FAIL;
    }

    return LT_OK;
}

// lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const lt_ecc_slot_t slot)

lt_ret_t lt_out__ecc_key_erase(lt_handle_t *h, const lt_ecc_slot_t slot)
{
    if (!h || (slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_ecc_key_erase_cmd_t *p_l3_cmd = (struct lt_l3_ecc_key_erase_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_ECC_KEY_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_ECC_KEY_ERASE_CMD_ID;
    p_l3_cmd->slot = slot;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_key_erase(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_erase_res_t *p_l3_res = (struct lt_l3_ecc_key_erase_res_t *)h->l3.buff;

    // Check incomming l3 length
    if (TR01_L3_ECC_KEY_ERASE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__ecc_ecdsa_sign(lt_handle_t *h, const lt_ecc_slot_t slot, const uint8_t *msg, const uint32_t msg_len)
{
    if (!h || (slot > TR01_ECC_SLOT_31) || !msg) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Prepare hash of a message
    uint8_t msg_hash[32] = {0};
    LT_CRYPTO_SHA256_CTX_T sha256_ctx;
    lt_ret_t ret;

    ret = lt_sha256_init(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_start(&sha256_ctx);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_update(&sha256_ctx, (uint8_t *)msg, msg_len);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_sha256_finish(&sha256_ctx, msg_hash);
    if (ret != LT_OK) {
        return ret;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecdsa_sign_cmd_t *p_l3_cmd = (struct lt_l3_ecdsa_sign_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_ECDSA_SIGN_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_ECDSA_SIGN_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg_hash, msg_hash, sizeof(p_l3_cmd->msg_hash));

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_ecdsa_sign(lt_handle_t *h, uint8_t *rs)
{
    if (!h || !rs) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecdsa_sign_res_t *p_l3_res = (struct lt_l3_ecdsa_sign_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_ECDSA_SIGN_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(rs, p_l3_res->r, sizeof(p_l3_res->r));
    memcpy(rs + sizeof(p_l3_res->r), p_l3_res->s, sizeof(p_l3_res->s));

    return LT_OK;
}

lt_ret_t lt_out__ecc_eddsa_sign(lt_handle_t *h, const lt_ecc_slot_t ecc_slot, const uint8_t *msg,
                                const uint16_t msg_len)
{
    if (!h || !msg || (msg_len > TR01_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX) || (ecc_slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_eddsa_sign_cmd_t *p_l3_cmd = (struct lt_l3_eddsa_sign_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size
        = TR01_L3_EDDSA_SIGN_CMD_SIZE_MIN + msg_len
          - 1;  // -1 Because the TR01_L3_EDDSA_SIGN_CMD_SIZE_MIN already includes minimal message size 1B
    p_l3_cmd->cmd_id = TR01_L3_EDDSA_SIGN_CMD_ID;
    p_l3_cmd->slot = ecc_slot;
    memcpy(p_l3_cmd->msg, msg, msg_len);

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__ecc_eddsa_sign(lt_handle_t *h, uint8_t *rs)
{
    if (!h || !rs) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_eddsa_sign_res_t *p_l3_res = (struct lt_l3_eddsa_sign_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_EDDSA_SIGN_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(rs, p_l3_res->r, sizeof(p_l3_res->r));
    memcpy(rs + sizeof(p_l3_res->r), p_l3_res->s, sizeof(p_l3_res->s));

    return LT_OK;
}

lt_ret_t lt_out__mcounter_init(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index,
                               const uint32_t mcounter_value)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_init_cmd_t *p_l3_cmd = (struct lt_l3_mcounter_init_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_MCOUNTER_INIT_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_MCOUNTER_INIT_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;
    p_l3_cmd->mcounter_val = mcounter_value;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__mcounter_init(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_init_res_t *p_l3_res = (struct lt_l3_mcounter_init_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_MCOUNTER_INIT_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__mcounter_update(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_update_cmd_t *p_l3_cmd = (struct lt_l3_mcounter_update_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_MCOUNTER_UPDATE_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_MCOUNTER_UPDATE_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__mcounter_update(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_update_res_t *p_l3_res = (struct lt_l3_mcounter_update_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_MCOUNTER_UPDATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_out__mcounter_get(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_get_cmd_t *p_l3_cmd = (struct lt_l3_mcounter_get_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_MCOUNTER_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_MCOUNTER_GET_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__mcounter_get(lt_handle_t *h, uint32_t *mcounter_value)
{
    if (!h || !mcounter_value) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_get_res_t *p_l3_res = (struct lt_l3_mcounter_get_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_MCOUNTER_GET_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *mcounter_value = p_l3_res->mcounter_val;

    return LT_OK;
}

lt_ret_t lt_out__mac_and_destroy(lt_handle_t *h, lt_mac_and_destroy_slot_t slot, const uint8_t *data_out)
{
    if (!h || !data_out || slot > TR01_MAC_AND_DESTROY_SLOT_127) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mac_and_destroy_cmd_t *p_l3_cmd = (struct lt_l3_mac_and_destroy_cmd_t *)h->l3.buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = TR01_L3_MAC_AND_DESTROY_CMD_SIZE;
    p_l3_cmd->cmd_id = TR01_L3_MAC_AND_DESTROY_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->data_in, data_out, TR01_MAC_AND_DESTROY_DATA_SIZE);

    return lt_l3_encrypt_request(&h->l3);
}

lt_ret_t lt_in__mac_and_destroy(lt_handle_t *h, uint8_t *data_in)
{
    if (!h || !data_in) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_mac_and_destroy_res_t *p_l3_res = (struct lt_l3_mac_and_destroy_res_t *)h->l3.buff;

    lt_ret_t ret = lt_l3_decrypt_response(&h->l3);
    if (ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if (TR01_L3_MAC_AND_DESTROY_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(data_in, p_l3_res->data_out, TR01_MAC_AND_DESTROY_DATA_SIZE);

    return LT_OK;
}
