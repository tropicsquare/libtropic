/**
* @file libtropic.c
* @brief Implementation of libtropic API
* @author Tropic Square s.r.o.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ts_common.h"
#include "ts_random.h"
#include "ts_l1.h"
#include "ts_l2.h"
#include "ts_l2_api.h"
#include "ts_l3.h"
#include "ts_l3_api.h"
#include "ts_x25519.h"
#include "ts_hkdf.h"
#include "ts_sha256.h"
#include "ts_aesgcm.h"
#include "libtropic.h"

ts_ret_t ts_init(ts_handle_t *h)
{
    if(!h) {
        return TS_PARAM_ERR;
    }

    ts_ret_t ret = ts_l1_init(h);
    if(ret != TS_OK) {
        return ret;
    }

    return TS_OK;
}

ts_ret_t ts_deinit(ts_handle_t *h)
{
    if(!h) {
        return TS_PARAM_ERR;
    }

    ts_ret_t ret = ts_l1_deinit(h);
    if(ret != TS_OK) {
        return ret;
    }

    return TS_OK;
}

ts_ret_t ts_handshake(ts_handle_t *h, const uint8_t *stpub, const uint8_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub)
{
    if (!h || !shipriv || !shipub || (pkey_index > TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_3)) {
        return TS_PARAM_ERR;
    }

    // Create ephemeral host keys
    uint8_t ehpriv[32];
    uint8_t ehpub[32];
    ts_ret_t ret = ts_random_bytes(ehpriv, 32);
    if(ret != TS_OK) {
        return ret;
    }
    ts_X25519_scalarmult(ehpriv, ehpub);

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct l2_handshake_req_t* p_req = (struct l2_handshake_req_t*)h->l2_buff;
    // Setup a response pointer to l2 buffer, which is placed in handle
    struct l2_handshake_rsp_t* p_rsp = (struct l2_handshake_rsp_t*)h->l2_buff;

    p_req->req_id = TS_L2_HANDSHAKE_REQ_ID;
    p_req->req_len = TS_L2_HANDSHAKE_REQ_LEN;
    memcpy(p_req->e_hpub, ehpub, 32);
    p_req->pkey_index = pkey_index;

    ret = ts_l2_transfer(h);
    if(ret != TS_OK) {
        return ret;
    }

    // Noise_KK1_25519_AESGCM_SHA256\x00\x00\x00
    uint8_t protocol_name[32] = {'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6',0x00,0x00,0x00};
    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    // h = SHA_256(protocol_name)
    ts_sha256_ctx_t hctx = {0};

    ts_sha256_init(&hctx);
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, protocol_name, 32);
    ts_sha256_finish(&hctx, hash);

    // h = SHA256(h||SHiPUB)
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, hash, 32);
    ts_sha256_update(&hctx, shipub, 32);
    ts_sha256_finish(&hctx, hash);

    // h = SHA256(h||STPUB)
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, hash, 32);
    ts_sha256_update(&hctx, stpub, 32);
    ts_sha256_finish(&hctx, hash);

    // h = SHA256(h||EHPUB)
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, hash, 32);
    ts_sha256_update(&hctx, ehpub, 32);
    ts_sha256_finish(&hctx, hash);

    // h = SHA256(h||PKEY_INDEX)
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, hash, 32);
    ts_sha256_update(&hctx, &pkey_index, 1);
    ts_sha256_finish(&hctx, hash);

    // h = SHA256(h||ETPUB)
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, hash, 32);
    ts_sha256_update(&hctx, p_rsp->e_tpub, 32);
    ts_sha256_finish(&hctx, hash);

    // ck = protocol_name
    uint8_t output_1[33] = {0};
    uint8_t output_2[32] = {0};
    // ck = HKDF (ck, X25519(EHPRIV, ETPUB), 1)
    uint8_t shared_secret[32] = {0};
    ts_X25519(ehpriv, p_rsp->e_tpub, shared_secret);
    ts_hkdf(protocol_name, 32, shared_secret, 32, 1, output_1, output_2);
    // ck = HKDF (ck, X25519(SHiPRIV, ETPUB), 1)
    ts_X25519(shipriv, p_rsp->e_tpub, shared_secret);
    ts_hkdf(output_1, 32, shared_secret, 32, 1, output_1, output_2);
    // ck, kAUTH = HKDF (ck, X25519(EHPRIV, STPUB), 2)
    ts_X25519(ehpriv, stpub, shared_secret);
    uint8_t kauth[32] = {0};
    ts_hkdf(output_1, 32, shared_secret, 32, 2, output_1, kauth);
    // kCMD, kRES = HKDF (ck, emptystring, 2)
    uint8_t kcmd[32] = {0};
    uint8_t kres[32] = {0};
    ts_hkdf(output_1, 32, (uint8_t*)"", 0, 2, kcmd, kres);

    ret = ts_aesgcm_init_and_key(&h->decrypt, kauth, 32);
    if(ret != RETURN_GOOD) {
        return ret;
    }

    ret = ts_aesgcm_decrypt(&h->decrypt, h->IV, 12u, hash, 32, (uint8_t*)"", 0, p_rsp->t_auth, 16u);
    if(ret != RETURN_GOOD) {
        return ret;
    }

    ret = ts_aesgcm_init_and_key(&h->encrypt, kcmd, 32);
    if(ret != RETURN_GOOD) {
        return ret;
    }

    ret = ts_aesgcm_init_and_key(&h->decrypt, kres, 32);
    if(ret != RETURN_GOOD) {
        return ret;
    }

    h->session = SESSION_ON;

    // TODO create goto and clean keys here before leaving if something fails
    return TS_OK;
}

ts_ret_t ts_ping(ts_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if((len > PING_LEN_MAX) || !h || !msg_out || !msg_in) {
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_ping_cmd_t* p_l3_cmd = (struct ts_l3_ping_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_ping_res_t* p_l3_res = (struct ts_l3_ping_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = len + 1;
    p_l3_cmd->command = TS_L3_PING_CMD;
    memcpy(p_l3_cmd->data, msg_out, len);

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    if(len != (p_l3_res->packet_size - 1))
    {
        return TS_FAIL;
    }

    memcpy(msg_in, (uint8_t*)&p_l3_res->data, p_l3_res->packet_size);

    return TS_OK;
}

ts_ret_t ts_random_get(ts_handle_t *h, uint8_t *buff, const uint16_t len)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if((len > RANDOM_VALUE_GET_LEN_MAX) || !h || !buff) {
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_random_value_get_cmd_t* p_l3_cmd = (struct ts_l3_random_value_get_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_random_value_get_res_t* p_l3_res = (struct ts_l3_random_value_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = TS_L3_RANDOM_VALUE_GET_CMD_SIZE;
    p_l3_cmd->command = TS_L3_RANDOM_VALUE_GET_CMD;
    p_l3_cmd->n_bytes = len;

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    if(len != (p_l3_res->packet_size - 1 - 3))
    {
        return TS_FAIL;
    }

    memcpy(buff, (uint8_t*)&p_l3_res->random_data, p_l3_res->packet_size);

    return TS_OK;
}

ts_ret_t ts_ecc_key_generate(ts_handle_t *h, const uint8_t slot, const uint8_t curve)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if(slot > TS_L3_ECC_KEY_GENERATE_SLOT_MAX || !h || ((curve != TS_L3_ECC_KEY_GENERATE_CURVE_P256) && (curve != TS_L3_ECC_KEY_GENERATE_CURVE_ED25519))  ){
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_ecc_key_generate_cmd_t* p_l3_cmd = (struct ts_l3_ecc_key_generate_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_ecc_key_generate_res_t* p_l3_res = (struct ts_l3_ecc_key_generate_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = TS_L3_ECC_KEY_GENERATE_CMD_SIZE;
    p_l3_cmd->command = TS_L3_ECC_KEY_GENERATE_CMD;
    p_l3_cmd->slot = slot;
    p_l3_cmd->curve = curve;

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    if(0 != (p_l3_res->packet_size - 1))
    {
        return TS_FAIL;
    }

    return TS_OK;
}

ts_ret_t ts_ecc_key_read(ts_handle_t *h, const uint8_t slot, uint8_t *key, const int8_t keylen, uint8_t *curve, uint8_t *origin)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if(!h || slot > TS_L3_ECC_KEY_GENERATE_SLOT_MAX || !key || !curve || !origin) {
        return TS_PARAM_ERR;
    }
    if(keylen < 64) {
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_ecc_key_read_cmd_t* p_l3_cmd = (struct ts_l3_ecc_key_read_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_ecc_key_read_res_t* p_l3_res = (struct ts_l3_ecc_key_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = TS_L3_ECC_KEY_READ_CMD_SIZE;
    p_l3_cmd->command= TS_L3_ECC_KEY_READ_CMD;
    p_l3_cmd->slot = slot;

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    // Check incomming l3 length
    if((p_l3_res->curve == TS_L3_ECC_KEY_READ_CURVE_ED25519) && ((p_l3_res->packet_size -1-1-1-13) != 32)) {
        return TS_FAIL;
    }
    if((p_l3_res->curve == TS_L3_ECC_KEY_READ_CURVE_P256) && ((p_l3_res->packet_size -1-1-1-13) != 64)) {
        return TS_FAIL;
    }

    *curve = p_l3_res->curve;
    *origin = p_l3_res->origin;
    memcpy(key, (uint8_t*)&p_l3_res->pub_key, p_l3_res->packet_size);

    return TS_OK;
}

ts_ret_t ts_eddsa_sign(ts_handle_t *h, const uint8_t slot, const uint8_t *msg, const int16_t msg_len, uint8_t *rs, const int8_t rs_len)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if(!h || !msg || !rs || rs_len < 64 || ((msg_len < TS_L3_EDDSA_SIGN_MSG_LEN_MIN) | (msg_len > TS_L3_EDDSA_SIGN_MSG_LEN_MAX))) {
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_eddsa_sign_cmd_t* p_l3_cmd = (struct ts_l3_eddsa_sign_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_eddsa_sign_res_t* p_l3_res = (struct ts_l3_eddsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = TS_L3_EDDSA_SIGN_CMD_SIZE + msg_len;
    p_l3_cmd->command = TS_L3_EDDSA_SIGN_CMD;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg, msg, msg_len + TS_L3_EDDSA_SIGN_CMD_SIZE);

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    memcpy(rs, (uint8_t*)&p_l3_res->r, 32);
    memcpy(rs + 32, (uint8_t*)&p_l3_res->s, 32);

    return TS_OK;
}

ts_ret_t ts_ecdsa_sign(ts_handle_t *h, const uint8_t slot, const uint8_t *msg_hash, const int16_t msg_hash_len, uint8_t *rs, const int8_t rs_len)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if(!h || !msg_hash || !rs || (msg_hash_len < TS_L3_ECDSA_SIGN_MSG_HASH_LEN) || (rs_len < 64)) {
        return TS_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains Ping command data
    struct ts_l3_ecdsa_sign_cmd_t* p_l3_cmd = (struct ts_l3_ecdsa_sign_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer when it contains Ping result data.
    struct ts_l3_ecdsa_sign_res_t* p_l3_res = (struct ts_l3_ecdsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->packet_size = TS_L3_ECDSA_SIGN_CMD_SIZE;
    p_l3_cmd->command= TS_L3_ECDSA_SIGN;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg_hash, msg_hash, TS_L3_ECDSA_SIGN_MSG_HASH_LEN);

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    memcpy(rs, (uint8_t*)&p_l3_res->r, 32);
    memcpy(rs + 32, (uint8_t*)&p_l3_res->s, 32);

    return TS_OK;
}

ts_ret_t ts_ecc_key_erase(ts_handle_t *h, const uint8_t slot)
{
    if(h->session != SESSION_ON) {
        return TS_HOST_NO_SESSION;
    }
    if(!h || slot > TS_L3_ECC_KEY_GENERATE_SLOT_MAX) {
        return TS_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct ts_l3_ecc_key_erase_cmd_t* p_l3_buff = (struct ts_l3_ecc_key_erase_cmd_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_buff->packet_size = TS_L3_ECC_KEY_ERASE_CMD_SIZE;
    p_l3_buff->command = TS_L3_ECC_KEY_ERASE_CMD;
    p_l3_buff->slot = slot;

    ts_ret_t ret = ts_l3_cmd(h);
    if(ret != TS_OK) {
        return ret;
    }

    return TS_OK;
}

ts_ret_t ts_get_info_cert(ts_handle_t *h, uint8_t *cert, const int16_t max_len)
{
    if (max_len < TS_L2_GET_INFO_REQ_CERT_SIZE || !h || !cert) {
        return TS_FAIL;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct l2_get_info_req_t* p_l2_buff = (struct l2_get_info_req_t*)&h->l2_buff;

    for(int8_t i=0; i<4; i++) {
        // Fill l2 request buffer
        p_l2_buff->req_id = TS_L2_GET_INFO_REQ_ID;
        p_l2_buff->req_len = TS_L2_GET_INFO_REQ_LEN;
        p_l2_buff->obj_id = TS_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE;

        // TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127    = 0, "i" is used
        // TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255  = 1, "i" is used
        // TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383  = 2, "i" is used
        // TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511  = 3, "i" is used
        p_l2_buff->block_index = i;
        ts_ret_t ret = ts_l2_transfer(h);
        if(ret != TS_OK) {
            return ret;
        }
        memcpy(cert + i*128, ((struct l2_get_info_rsp_t*)h->l2_buff)->data, 128);
    }

    return TS_OK;
}

ts_ret_t ts_cert_verify_and_parse(const uint8_t *cert, const int16_t max_len, uint8_t *stpub)
{
    if(!cert || !stpub || (max_len > 512)) {
        return TS_PARAM_ERR;
    }

    // TODO Verify

    // TODO parse correctly, not like this:
    #ifdef HOST_KEY_FPGA
    memcpy(stpub, cert+197, 32);
    #else
    memcpy(stpub, cert+188, 32);
    #endif

    return TS_OK;
}


// TODO
//ts_ret_t ts_get_info_chip_id(ts_handle_t *h, uint8_t chip_id, uint16_t max_len)
//{
//    return TS_OK;
//}
//ts_ret_t ts_get_info_riscv_fw_ver(ts_handle_t *h, uint8_t ver, uint16_t max_len)
//{
//    return TS_OK;
//}
//ts_ret_t ts_get_info_spect_fw_ver(ts_handle_t *h, uint8_t ver, uint16_t max_len)
//{
//    return TS_OK;
//}
//ts_ret_t ts_get_info_fw_bank(ts_handle_t *h, uint8_t fw_bank, uint16_t max_len)
//{
//    return TS_OK;
//}
//
