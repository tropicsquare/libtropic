/**
 * @file libtropic.c
 * @brief Implementation of libtropic API
 * @author Tropic Square s.r.o.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_port.h"
#include "lt_random.h"
#include "lt_l1_port_wrap.h"
#include "lt_l1.h"
#include "lt_l2.h"
#include "lt_l2_api_structs.h"
#include "lt_l3.h"
#include "lt_l3_api_structs.h"
#include "lt_x25519.h"
#include "lt_ed25519.h"
#include "lt_hkdf.h"
#include "lt_sha256.h"
#include "lt_aesgcm.h"
#include "libtropic.h"
#include "TROPIC01_configuration_objects.h"

lt_ret_t lt_init(lt_handle_t *h)
{
    if(!h) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret = lt_l1_init(h);
    if(ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_deinit(lt_handle_t *h)
{
    if(!h) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret = lt_l1_deinit(h);
    if(ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

/** @brief Block index for data bytes 0-511 of the object */
typedef enum {
LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127,
LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255,
LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383,
LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511,
} block_index_t;

/** @brief The X.509 device certificate read from I-Memory and signed by Tropic Square (max length of 512B) */
# define LT_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE 0
/** @brief The chip ID - the chip silicon revision and unique device ID (max length of 128B) */
# define LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID 1
/** @brief The RISCV current running FW version (4 Bytes) */
# define LT_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION 2
/** @brief The SPECT FW version (4 Bytes) */
# define LT_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION 4
/** @brief The FW header read from the selected bank id (shown as an index). Supported only in Start-up mode */
# define LT_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK 176

lt_ret_t lt_get_info_cert(lt_handle_t *h, uint8_t *cert, const uint16_t max_len)
{
    if (     max_len < LT_L2_GET_INFO_REQ_CERT_SIZE
         || !h
         || !cert
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer with request data
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    for(int8_t i=0; i<4; i++) {
        // Fill l2 request buffer
        p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
        p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
        p_l2_req->obj_id = LT_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE;

        // LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127    = 0, "i" is used
        // LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255  = 1, "i" is used
        // LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383  = 2, "i" is used
        // LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511  = 3, "i" is used
        p_l2_req->block_index = i;
        lt_ret_t ret = lt_l2_transfer(h);
        if(ret != LT_OK) {
            return ret;
        }

        // Check incomming l3 length
        if((LT_L2_GET_INFO_REQ_CERT_SIZE/4) != (p_l2_resp->rsp_len)) {
            return LT_FAIL;
        }

        memcpy(cert + i*128, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->data, 128);
    }

    return LT_OK;
}

lt_ret_t lt_cert_verify_and_parse(const uint8_t *cert, const uint16_t max_len, uint8_t *stpub)
{
    if(    !cert
        || !stpub
        || (max_len > LT_L2_GET_INFO_REQ_CERT_SIZE)
    ) {
        return LT_PARAM_ERR;
    }

    // TODO Verify

    /* TODO Improve this
    Currently DER certificate is searched for "OBJECT IDENTIFIER curveX25519 (1 3 101 110)",
    which is represented by four bytes: 0x65, 0x6e, 0x03 and 0x21 */
    for(int i = 0; i<(512-3); i++) {
        if(cert[i] == 0x65 && cert[i+1] == 0x6e && cert[i+2] == 0x03 && cert[i+3] == 0x21) {
            memcpy(stpub, cert + i + 5, 32);
            return LT_OK;
        }
    }

    return LT_FAIL;
}

lt_ret_t lt_get_info_chip_id(lt_handle_t *h, uint8_t *chip_id, const uint16_t max_len)
{
    if (    !h
         || !chip_id
         ||  max_len < LT_L2_GET_INFO_CHIP_ID_SIZE) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
    p_l2_req->obj_id = LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(128 != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(chip_id, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->data, 128);

    return LT_OK;
}

lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len)
{
    if (    !h
         || !ver
         ||  max_len < LT_L2_GET_INFO_RISCV_FW_SIZE
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
    p_l2_req->obj_id = LT_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_RISCV_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->data, LT_L2_GET_INFO_RISCV_FW_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len)
{
    if (    !h
         || !ver
         ||  max_len < LT_L2_GET_INFO_SPECT_FW_SIZE) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
    p_l2_req->obj_id = LT_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_SPECT_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->data, LT_L2_GET_INFO_SPECT_FW_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, uint8_t *header, const uint16_t max_len)
{
    if (    !h
         || !header
         ||  max_len < LT_L2_GET_INFO_FW_HEADER_SIZE
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
    p_l2_req->obj_id = LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(128 != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(header, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->data, 128); //TODO specify and fix size of header

    return LT_OK;
}

lt_ret_t lt_session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub)
{
    if (    !h
         || !shipriv
         || !shipub
         || (pkey_index > PAIRING_KEY_SLOT_INDEX_3)
    ) {
        return LT_PARAM_ERR;
    }
    memset(h, 0, sizeof(lt_handle_t));
    // Create ephemeral host keys
    uint8_t ehpriv[32];
    uint8_t ehpub[32];
    lt_ret_t ret = lt_random_bytes((uint32_t*)ehpriv, 8);
    if(ret != LT_OK) {
        return ret;
    }
    lt_X25519_scalarmult(ehpriv, ehpub);

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_req_t* p_req = (struct lt_l2_handshake_req_t*)h->l2_buff;
    // Setup a response pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_rsp_t* p_rsp = (struct lt_l2_handshake_rsp_t*)h->l2_buff;

    p_req->req_id = LT_L2_HANDSHAKE_REQ_ID;
    p_req->req_len = LT_L2_HANDSHAKE_REQ_LEN;
    memcpy(p_req->e_hpub, ehpub, 32);
    p_req->pkey_index = pkey_index;

    ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Noise_KK1_25519_AESGCM_SHA256\x00\x00\x00
    uint8_t protocol_name[32] = {'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6',0x00,0x00,0x00};
    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    // h = SHA_256(protocol_name)
     lt_crypto_sha256_ctx_t hctx = {0};
    lt_sha256_init(&hctx);
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, protocol_name, 32);
    lt_sha256_finish(&hctx, hash);

    // h = SHA256(h||SHiPUB)
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, hash, 32);
    lt_sha256_update(&hctx, shipub, 32);
    lt_sha256_finish(&hctx, hash);

    // h = SHA256(h||STPUB)
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, hash, 32);
    lt_sha256_update(&hctx, stpub, 32);
    lt_sha256_finish(&hctx, hash);

    // h = SHA256(h||EHPUB)
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, hash, 32);
    lt_sha256_update(&hctx, ehpub, 32);
    lt_sha256_finish(&hctx, hash);

    // h = SHA256(h||PKEY_INDEX)
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, hash, 32);
    lt_sha256_update(&hctx, (uint8_t*)&pkey_index, 1);
    lt_sha256_finish(&hctx, hash);

    // h = SHA256(h||ETPUB)
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, hash, 32);
    lt_sha256_update(&hctx, p_rsp->e_tpub, 32);
    lt_sha256_finish(&hctx, hash);

    // ck = protocol_name
    uint8_t output_1[33] = {0};
    uint8_t output_2[32] = {0};
    // ck = HKDF (ck, X25519(EHPRIV, ETPUB), 1)
    uint8_t shared_secret[32] = {0};
    lt_X25519(ehpriv, p_rsp->e_tpub, shared_secret);
    lt_hkdf(protocol_name, 32, shared_secret, 32, 1, output_1, output_2);
    // ck = HKDF (ck, X25519(SHiPRIV, ETPUB), 1)
    lt_X25519(shipriv, p_rsp->e_tpub, shared_secret);
    lt_hkdf(output_1, 32, shared_secret, 32, 1, output_1, output_2);
    // ck, kAUTH = HKDF (ck, X25519(EHPRIV, STPUB), 2)
    lt_X25519(ehpriv, stpub, shared_secret);
    uint8_t kauth[32] = {0};
    lt_hkdf(output_1, 32, shared_secret, 32, 2, output_1, kauth);
    // kCMD, kRES = HKDF (ck, emptystring, 2)
    uint8_t kcmd[32] = {0};
    uint8_t kres[32] = {0};
    lt_hkdf(output_1, 32, (uint8_t*)"", 0, 2, kcmd, kres);

    ret = lt_aesgcm_init_and_key(&h->decrypt, kauth, 32);
    if(ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    ret = lt_aesgcm_decrypt(&h->decrypt, h->IV, 12u, hash, 32, (uint8_t*)"", 0, p_rsp->t_auth, 16u);
    if(ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    ret = lt_aesgcm_init_and_key(&h->encrypt, kcmd, 32);
    if(ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    ret = lt_aesgcm_init_and_key(&h->decrypt, kres, 32);
    if(ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    h->session = SESSION_ON;
    //lt_l3_nonce_init(h);
    // TODO create goto and clean keys here before leaving if something fails?
    return LT_OK;
}

lt_ret_t lt_session_abort(lt_handle_t *h)
{
    if (    !h
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_session_abt_req_t* p_l2_req = (struct lt_l2_encrypted_session_abt_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_encrypted_session_abt_rsp_t* p_l2_resp = (struct lt_l2_encrypted_session_abt_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_ENCRYPTED_SESSION_ABT_REQ_ID;
    p_l2_req->req_len = 0;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(0 != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_reboot(lt_handle_t *h, const uint8_t startup_id)
{
    if (    !h
         || ((startup_id != LT_L2_STARTUP_ID_REBOOT) && (startup_id != LT_L2_STARTUP_ID_MAINTENANCE))
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_startup_req_t* p_l2_req = (struct lt_l2_startup_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_startup_rsp_t* p_l2_resp = (struct lt_l2_startup_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_STARTUP_REQ_ID;
    p_l2_req->req_len = LT_L2_STARTUP_REQ_LEN;
    p_l2_req->startup_id = startup_id;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(0 != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ping(lt_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len)
{
    if(    (len > PING_LEN_MAX)
        || !h
        || !msg_out
        || !msg_in
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ping_cmd_t* p_l3_cmd = (struct lt_l3_ping_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = len + 1;
    p_l3_cmd->cmd_id = LT_L3_PING_CMD;
    memcpy(p_l3_cmd->data, msg_out, len);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(len != (p_l3_res->res_size - 1))
    {
        return LT_FAIL;
    }

    memcpy(msg_in, p_l3_res->data, len);

    return LT_OK;
}

lt_ret_t lt_pairing_key_write(lt_handle_t *h, const uint8_t *pubkey, const uint8_t slot)
{
    if(    !h
        || !pubkey
        || (slot > 3)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_write_cmd_t * p_l3_cmd = (struct lt_l3_pairing_key_write_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_pairing_key_write_res_t* p_l3_res = (struct lt_l3_pairing_key_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_WRITE_CMD;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->key, pubkey, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_pairing_key_read(lt_handle_t *h, uint8_t *pubkey, const uint8_t slot)
{
    if(    !h
        || !pubkey
        || (slot > 3)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_read_cmd_t * p_l3_cmd = (struct lt_l3_pairing_key_read_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_READ_CMD;
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_PAIRING_KEY_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(pubkey, p_l3_res->key, 32);

    return LT_OK;
}

lt_ret_t lt_pairing_key_invalidate(lt_handle_t *h, const uint8_t slot)
{
    if(    !h
        || (slot > 3)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_pairing_key_invalidate_cmd_t * p_l3_cmd = (struct lt_l3_pairing_key_invalidate_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_pairing_key_invalidate_res_t* p_l3_res = (struct lt_l3_pairing_key_invalidate_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_INVALIDATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_INVALIDATE_CMD;
    // cmd data
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj)
{
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(!h) {
        return LT_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_cmd_t* p_l3_cmd = (struct lt_l3_r_config_write_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_res_t* p_l3_res = (struct lt_l3_r_config_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_WRITE_SIZE;
    p_l3_cmd->command = LT_L3_R_CONFIG_WRITE;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->value = obj;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj)
{
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(!h) {
        return LT_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_cmd_t* p_l3_cmd = (struct lt_l3_r_config_read_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_res_t* p_l3_res = (struct lt_l3_r_config_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_READ_SIZE;
    p_l3_cmd->command = LT_L3_R_CONFIG_READ;
    p_l3_cmd->address = (uint16_t)addr;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(8 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *obj = p_l3_res->value;

    return LT_OK;
}

lt_ret_t lt_r_config_erase(lt_handle_t *h)
{
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(!h) {
        return LT_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_cmd_t* p_l3_cmd = (struct lt_l3_r_config_erase_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_res_t* p_l3_res = (struct lt_l3_r_config_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_ERASE_SIZE;
    p_l3_cmd->command = LT_L3_R_CONFIG_ERASE;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index)
{
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(!h) {
        return LT_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_cmd_t* p_l3_cmd = (struct lt_l3_i_config_write_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_res_t* p_l3_res = (struct lt_l3_i_config_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_I_CONFIG_WRITE_SIZE;
    p_l3_cmd->command = LT_L3_I_CONFIG_WRITE;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->bit_index = bit_index;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj)
{
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(!h) {
        return LT_PARAM_ERR;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_cmd_t* p_l3_cmd = (struct lt_l3_i_config_read_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_res_t* p_l3_res = (struct lt_l3_i_config_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_I_CONFIG_READ_SIZE;
    p_l3_cmd->command = LT_L3_I_CONFIG_READ;
    p_l3_cmd->address = (uint16_t)addr;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(8 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *obj = p_l3_res->value;

    return LT_OK;
}

lt_ret_t lt_r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size)
{
     if(   !h
        || !data
        ||  size > R_MEM_DATA_SIZE_MAX
        || (udata_slot > R_MEM_DATA_SLOT_MAX)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_write_cmd_t * p_l3_cmd = (struct lt_l3_r_mem_data_write_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_r_mem_data_write_res_t* p_l3_res = (struct lt_l3_r_mem_data_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = size + 4;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_WRITE_CMD;
    p_l3_cmd->slot = udata_slot;
    memcpy(p_l3_cmd->data, data, size);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size)
{
    if(    !h
        || !data
        ||  size > 444
        || (udata_slot > 511)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_read_cmd_t * p_l3_cmd = (struct lt_l3_r_mem_data_read_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_r_mem_data_read_res_t* p_l3_res = (struct lt_l3_r_mem_data_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_MEM_DATA_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_READ_CMD;
    p_l3_cmd->slot = udata_slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(4 + size != (p_l3_res->res_size)) {
        return LT_FAIL;
    }
    memcpy(data, p_l3_res->data, size);

    return LT_OK;
}

lt_ret_t lt_r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot)
{
    if(    !h
        || (udata_slot > 511)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_r_mem_data_erase_cmd_t * p_l3_cmd = (struct lt_l3_r_mem_data_erase_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_r_mem_data_erase_res_t* p_l3_res = (struct lt_l3_r_mem_data_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_MEM_DATA_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_ERASE_CMD;
    p_l3_cmd->slot = udata_slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_random_get(lt_handle_t *h, uint8_t *buff, const uint16_t len)
{
    if(    (len > RANDOM_VALUE_GET_LEN_MAX)
        || !h
        || !buff
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_random_value_get_cmd_t* p_l3_cmd = (struct lt_l3_random_value_get_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_RANDOM_VALUE_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_RANDOM_VALUE_GET_CMD;
    p_l3_cmd->n_bytes = len;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 + len != (p_l3_res->res_size - 3)) {
        return LT_FAIL;
    }

    memcpy(buff, p_l3_res->random_data, p_l3_res->res_size);

    return LT_OK;
}

lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const ecc_curve_type_t curve)
{
    if(    !h
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || ((curve != CURVE_P256) && (curve != CURVE_ED25519))
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_generate_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_generate_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_GENERATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_ECC_KEY_GENERATE_CMD;
    p_l3_cmd->slot = (uint8_t)slot;
    p_l3_cmd->curve = (uint8_t)curve;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const ecc_curve_type_t curve, const uint8_t *key)
{
    if(    !h
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || !curve
        || !key
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_store_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_store_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ecc_key_store_res_t* p_l3_res = (struct lt_l3_ecc_key_store_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_STORE_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECC_KEY_STORE_CMD;
    p_l3_cmd->slot = slot;
    p_l3_cmd->curve = curve;
    memcpy(p_l3_cmd->key, key, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t slot, uint8_t *key, const uint8_t keylen, ecc_curve_type_t *curve, ecc_key_origin_t *origin)
{
    if(    !h
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || !key
        || !curve
        || !origin
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    if(keylen < 64) {
        return LT_PARAM_ERR;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_read_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_read_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ecc_key_read_res_t* p_l3_res = (struct lt_l3_ecc_key_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECC_KEY_READ_CMD;
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if((p_l3_res->curve == (uint8_t)CURVE_ED25519) && ((p_l3_res->res_size -1-1-1-13) != 32)) {
        return LT_FAIL;
    }
    if((p_l3_res->curve == (uint8_t)CURVE_P256) && ((p_l3_res->res_size -1-1-1-13) != 64)) {
        return LT_FAIL;
    }

    *curve = p_l3_res->curve;
    *origin = p_l3_res->origin;

    if((p_l3_res->curve == (uint8_t)CURVE_ED25519)) {
        memcpy(key, p_l3_res->pub_key, 32);
    }
    if((p_l3_res->curve == (uint8_t)CURVE_P256)) {
        memcpy(key, p_l3_res->pub_key, 64);
    }

    return LT_OK;
}

lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot)
{
    if( !h
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_ecc_key_erase_cmd_t* p_l3_buff = (struct lt_l3_ecc_key_erase_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ecc_key_erase_res_t* p_l3_res = (struct lt_l3_ecc_key_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_buff->cmd_size = LT_L3_ECC_KEY_ERASE_CMD_SIZE;
    p_l3_buff->cmd_id = LT_L3_ECC_KEY_ERASE_CMD;
    p_l3_buff->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(1 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len, uint8_t *rs, const uint8_t rs_len)
{
    if(    !h
        || !msg
        || !rs
        || (msg_len > LT_L3_ECDSA_SIGN_MSG_LEN_MAX) || (rs_len < 64)
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Prepare hash of a message
    uint8_t msg_hash[32] = {0};
     lt_crypto_sha256_ctx_t hctx = {0};
    lt_sha256_init(&hctx);
    lt_sha256_start(&hctx);
    lt_sha256_update(&hctx, (uint8_t*)msg, msg_len);
    lt_sha256_finish(&hctx, msg_hash);

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecdsa_sign_cmd_t* p_l3_cmd = (struct lt_l3_ecdsa_sign_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_ecdsa_sign_res_t* p_l3_res = (struct lt_l3_ecdsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECDSA_SIGN_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECDSA_SIGN;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg_hash, msg_hash, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(0x50 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(rs, p_l3_res->r, 32);
    memcpy(rs + 32, p_l3_res->s, 32);

    return LT_OK;
}

lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len, uint8_t *rs, const uint8_t rs_len)
{
    if(    !h
        || !msg
        || !rs
        || rs_len < 64
        || ((msg_len < LT_L3_EDDSA_SIGN_MSG_LEN_MIN) | (msg_len > LT_L3_EDDSA_SIGN_MSG_LEN_MAX))
        || slot < LT_L3_ECC_KEY_GENERATE_SLOT_MIN
        || slot > LT_L3_ECC_KEY_GENERATE_SLOT_MAX
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_eddsa_sign_cmd_t* p_l3_cmd = (struct lt_l3_eddsa_sign_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_EDDSA_SIGN_CMD_SIZE + msg_len;
    p_l3_cmd->cmd_id = LT_L3_EDDSA_SIGN_CMD;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg, msg, msg_len);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(0x50 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(rs, p_l3_res->r, 32);
    memcpy(rs + 32, p_l3_res->s, 32);

    return LT_OK;
}

lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    if (   !msg
        ||  msg_len < LT_L3_EDDSA_SIGN_MSG_LEN_MIN
        ||  msg_len > LT_L3_EDDSA_SIGN_MSG_LEN_MAX
        || !pubkey
        || !rs
    ) {
        return LT_PARAM_ERR;
    }

    if (lt_ed25519_sign_open(msg, msg_len, pubkey, rs) != 0) {
        return LT_FAIL;
    }

    return LT_OK;
}

/*

MCounter_Init

MCounter_Update

MCounter_Get

*/

lt_ret_t lt_serial_code_get(lt_handle_t *h, uint8_t *serial_code, const uint16_t size)
{
     if( !h
        || !serial_code
        || size > SERIAL_CODE_SIZE
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_serial_data_get_cmd_t* p_l3_buff = (struct lt_l3_serial_data_get_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result data
    struct lt_l3_serial_data_get_res_t* p_l3_res = (struct lt_l3_serial_data_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_buff->cmd_size = LT_L3_SERIAL_DATA_GET_SIZE;
    p_l3_buff->cmd_id = LT_L3_SERIAL_DATA_GET_CMD;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(0x24 != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(serial_code, p_l3_res->serial_code, SERIAL_CODE_SIZE);

    return LT_OK;
}

const char *lt_ret_verbose(lt_ret_t ret) {
    switch(ret) {
        // Chip MODES
        case LT_L1_CHIP_ALARM_MODE:
            return "LT_L1_CHIP_ALARM_MODE";
        case LT_L1_CHIP_STARTUP_MODE:
            return "LT_L1_CHIP_STARTUP_MODE";
        case LT_L1_CHIP_BUSY:
            return "LT_L1_CHIP_BUSY";

        // L1
        case LT_L1_SPI_ERROR:
            return "LT_L1_SPI_ERROR";
        case LT_L1_DATA_LEN_ERROR:
            return "LT_L1_DATA_LEN_ERROR";

        // L2
        case LT_L2_IN_CRC_ERR:
            return "LT_L2_IN_CRC_ERR";
        case LT_L2_REQ_CONT:
            return "LT_L2_REQ_CONT";
        case LT_L2_RES_CONT:
            return "LT_L2_RES_CONT";
        case LT_L2_HSK_ERR:
            return "LT_L2_HSK_ERR";
        case LT_L2_NO_SESSION:
            return "LT_L2_NO_SESSION";
        case LT_L2_TAG_ERR:
            return "LT_L2_TAG_ERR";
        case LT_L2_CRC_ERR:
            return "LT_L2_CRC_ERR";
        case LT_L2_GEN_ERR:
            return "LT_L2_GEN_ERR";
        case LT_L2_NO_RESP:
            return "LT_L2_NO_RESP";
        case LT_L2_UNKNOWN_REQ:
            return "LT_L2_UNKNOWN_REQ";
        case LT_L2_STATUS_NOT_RECOGNIZED:
            return "LT_L2_STATUS_NOT_RECOGNIZED";
        case LT_L2_DATA_LEN_ERROR:
            return "LT_L2_DATA_LEN_ERROR";

        // L3
        case LT_L3_OK:
            return "LT_L3_OK";
        case LT_L3_FAIL:
            return "LT_L3_FAIL";
        case LT_L3_UNAUTHORIZED:
            return "LT_L3_UNAUTHORIZED";
        case LT_L3_INVALID_CMD:
            return "LT_L3_INVALID_CMD";
        case LT_L3_DATA_LEN_ERROR:
            return "LT_L3_DATA_LEN_ERROR";

        // libtropic
        case LT_HOST_NO_SESSION:
            return "LT_HOST_NO_SESSION";
        case LT_OK:
            return "LT_OK";
        case LT_FAIL:
            return "LT_FAIL";
        case LT_PARAM_ERR:
            return "LT_PARAM_ERR";
        case LT_CRYPTO_ERR:
            return "LT_CRYPTO_ERR";

        // Default
        default:
            return "FATAL ERROR, unknown return value";
  }
}
