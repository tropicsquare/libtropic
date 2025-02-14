/**
 * @file libtropic.c
 * @brief Implementation of libtropic API
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
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

lt_ret_t lt_update_mode(lt_handle_t *h)
{
    if (   !h
    ) {
        return LT_PARAM_ERR;
    }

    // Transfer just one byte to read CHIP_STATUS byte
    lt_l1_spi_csn_low(h);

    if (lt_l1_spi_transfer(h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT) != LT_OK) {
        lt_l1_spi_csn_high(h);
        return LT_L1_SPI_ERROR;
    }

    lt_l1_spi_csn_high(h);

    // Buffer in handle now contains CHIP_STATUS byte,
    // Save info about chip mode into 'mode' variable in handle
    if(h->l2_buff[0] & CHIP_MODE_STARTUP_bit) {
        h->mode = 1;
    } else {
        h->mode = 0;
    }

    return LT_OK;
}

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
        p_l2_req->object_id = LT_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE;

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

        memcpy(cert + i*128, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->object, 128);
    }

    return LT_OK;
}

lt_ret_t lt_cert_verify_and_parse(const uint8_t *cert, const uint16_t max_len, uint8_t *stpub)
{
    if(    !cert
        || !stpub
        || (max_len < LT_L2_GET_INFO_REQ_CERT_SIZE)
    ) {
        return LT_PARAM_ERR;
    }

    // TODO Verify

    /* TODO Improve this
    Currently DER certificate is searched for "OBJECT IDENTIFIER curveX25519 (1 3 101 110)",
    which is represented by four bytes: 0x65, 0x6e, 0x03 and 0x21 */
    for(int i = 0; i<(476); i++) {
        if(cert[i] == 0x65 && cert[i+1] == 0x6e && cert[i+2] == 0x03 && cert[i+3] == 0x21) {
            memcpy(stpub, cert + i + 4 + 1, 32); // +1 because pubkey starts one byte after object identifier
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
    p_l2_req->object_id = LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_CHIP_ID_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(chip_id, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->object, LT_L2_GET_INFO_CHIP_ID_SIZE);

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
    p_l2_req->object_id = LT_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_RISCV_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->object, LT_L2_GET_INFO_RISCV_FW_SIZE);

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
    p_l2_req->object_id = LT_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION;
    p_l2_req->block_index = LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_SPECT_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->object, LT_L2_GET_INFO_SPECT_FW_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, const bank_id_t bank_id, uint8_t *header, const uint16_t max_len)
{
    if (    !h
         || !header
         ||  max_len < LT_L2_GET_INFO_FW_HEADER_SIZE
         ||  ((bank_id != FW_BANK_FW1) && (bank_id != FW_BANK_FW2) && (bank_id != FW_BANK_SPECT1) && (bank_id != FW_BANK_SPECT2))
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t* p_l2_req = (struct lt_l2_get_info_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_INFO_REQ_LEN;
    p_l2_req->object_id = LT_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK;
    p_l2_req->block_index = bank_id;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_GET_INFO_FW_HEADER_SIZE != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    memcpy(header, ((struct lt_l2_get_info_rsp_t*)h->l2_buff)->object, LT_L2_GET_INFO_FW_HEADER_SIZE); //TODO specify and fix size of header

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

    //TODO check length here

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

    ret = lt_aesgcm_decrypt(&h->decrypt, h->IV, 12u, hash, 32, (uint8_t*)"", 0, p_rsp->t_tauth, 16u);
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

    p_l2_req->req_id = LT_L2_ENCRYPTED_SESSION_ABT_ID;
    p_l2_req->req_len = LT_L2_ENCRYPTED_SESSION_ABT_LEN;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_ENCRYPTED_SESSION_ABT_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_sleep(lt_handle_t *h, const uint8_t sleep_kind)
{
    if (    !h
         || ((sleep_kind != LT_L2_SLEEP_KIND_SLEEP) && (sleep_kind != LT_L2_SLEEP_KIND_DEEP_SLEEP))
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_startup_req_t* p_l2_req = (struct lt_l2_startup_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_startup_rsp_t* p_l2_resp = (struct lt_l2_startup_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_SLEEP_REQ_ID;
    p_l2_req->req_len = LT_L2_SLEEP_REQ_LEN;
    p_l2_req->startup_id = sleep_kind;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L2_SLEEP_RSP_LEN != (p_l2_resp->rsp_len)) {
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
    if(LT_L2_STARTUP_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    lt_l1_delay(h, LT_TROPIC01_REBOOT_DELAY_MS);

    return LT_OK;
}

lt_ret_t lt_mutable_fw_erase(lt_handle_t *h, bank_id_t bank_id)
{
    if (    !h
         ||  ((bank_id != FW_BANK_FW1) && (bank_id != FW_BANK_FW2) && (bank_id != FW_BANK_SPECT1) && (bank_id != FW_BANK_SPECT2))
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_erase_req_t* p_l2_req = (struct lt_l2_mutable_fw_erase_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_erase_rsp_t* p_l2_resp = (struct lt_l2_mutable_fw_erase_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_MUTABLE_FW_ERASE_REQ_ID;
    p_l2_req->req_len = LT_L2_MUTABLE_FW_ERASE_REQ_LEN;
    p_l2_req->bank_id = bank_id;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    if(LT_L2_MUTABLE_FW_ERASE_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *fw_data, const uint16_t fw_data_size, bank_id_t bank_id)
{
    if (    !h
         || !fw_data
         ||  fw_data_size > 25600
         ||  ((bank_id != FW_BANK_FW1) && (bank_id != FW_BANK_FW2) && (bank_id != FW_BANK_SPECT1) && (bank_id != FW_BANK_SPECT2))
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_update_req_t* p_l2_req = (struct lt_l2_mutable_fw_update_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_update_rsp_t* p_l2_resp = (struct lt_l2_mutable_fw_update_rsp_t*)&h->l2_buff;

    uint16_t loops = fw_data_size / 128;
    uint16_t rest = fw_data_size % 128;

    for (int16_t i = 0; i < loops; i++) {
        p_l2_req->req_id = LT_L2_MUTABLE_FW_UPDATE_REQ_ID;
        p_l2_req->req_len = LT_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN + 128;
        p_l2_req->bank_id = bank_id;
        p_l2_req->offset = i*128;
        memcpy(p_l2_req->data, fw_data + (i*128), 128);

        lt_ret_t ret = lt_l2_transfer(h);
        if(ret != LT_OK) {
            return ret;
        }

        if(LT_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
            return LT_FAIL;
        }
    }

    if (rest != 0) {
        p_l2_req->req_id = LT_L2_MUTABLE_FW_UPDATE_REQ_ID;
        p_l2_req->req_len = LT_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN + rest;
        p_l2_req->bank_id = bank_id;
        p_l2_req->offset = loops*128;
        memcpy(p_l2_req->data, fw_data + (loops*128), rest);

        lt_ret_t ret = lt_l2_transfer(h);
        if(ret != LT_OK) {
            return ret;
        }

        if(LT_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
            return LT_FAIL;
        }
    }

    return LT_OK;
}

lt_ret_t lt_get_log(lt_handle_t *h, uint8_t *log_msg, uint16_t msg_len_max)
{
    if (    !h
         || !log_msg
         || msg_len_max > GET_LOG_MAX_MSG_LEN
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_log_req_t* p_l2_req = (struct lt_l2_get_log_req_t*)&h->l2_buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_log_rsp_t* p_l2_resp = (struct lt_l2_get_log_rsp_t*)&h->l2_buff;

    p_l2_req->req_id = LT_L2_GET_LOG_REQ_ID;
    p_l2_req->req_len = LT_L2_GET_LOG_REQ_LEN;

    lt_ret_t ret = lt_l2_transfer(h);
    if(ret != LT_OK) {
        return ret;
    }

    // No check for incomming l3 length because we don't know in advance how big message will be,
    // the max possible length is 255 (uint8_t) and that fits the safe size GET_LOG_MAX_MSG_LEN of log_msg buffer

    memcpy(log_msg, p_l2_resp->log_msg, p_l2_resp->rsp_len);

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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = len + LT_L3_PING_CMD_SIZE_MIN;
    p_l3_cmd->cmd_id = LT_L3_PING_CMD_ID;
    memcpy(p_l3_cmd->data_in, msg_out, len);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(len != (p_l3_res->res_size - LT_L3_PING_CMD_SIZE_MIN))
    {
        return LT_FAIL;
    }

    memcpy(msg_in, p_l3_res->data_out, len);

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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_write_res_t* p_l3_res = (struct lt_l3_pairing_key_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_WRITE_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->s_hipub, pubkey, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_PAIRING_KEY_WRITE_RES_SIZE != (p_l3_res->res_size)) {
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_READ_CMD_ID;
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_PAIRING_KEY_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(pubkey, p_l3_res->s_hipub, 32);

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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_pairing_key_invalidate_res_t* p_l3_res = (struct lt_l3_pairing_key_invalidate_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_PAIRING_KEY_INVALIDATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_PAIRING_KEY_INVALIDATE_CMD_ID;
    // cmd data
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_PAIRING_KEY_INVALIDATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

static bool conf_obj_valid(enum CONFIGURATION_OBJECTS_REGS addr)
{
    bool valid = false;

    switch(addr) {
        case CONFIGURATION_OBJECTS_CFG_START_UP_ADDR:
        case CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR:
        case CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR:
        case CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR:
            valid = true;
        }
    return valid;
}

lt_ret_t lt_r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj)
{
    if(    !h
        || !conf_obj_valid(addr)
        // TODO how to check object?
        ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_cmd_t* p_l3_cmd = (struct lt_l3_r_config_write_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_write_res_t* p_l3_res = (struct lt_l3_r_config_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_CONFIG_WRITE_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->value = obj;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_CONFIG_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj)
{
    if(    !h
        || !conf_obj_valid(addr)
        || !obj
        ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_cmd_t* p_l3_cmd = (struct lt_l3_r_config_read_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_read_res_t* p_l3_res = (struct lt_l3_r_config_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_CONFIG_READ_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_CONFIG_READ_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *obj = p_l3_res->value;

    return LT_OK;
}

lt_ret_t lt_r_config_erase(lt_handle_t *h)
{
    if(!h) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_cmd_t* p_l3_cmd = (struct lt_l3_r_config_erase_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_r_config_erase_res_t* p_l3_res = (struct lt_l3_r_config_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_CONFIG_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_CONFIG_ERASE_CMD_ID;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_CONFIG_ERASE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index)
{
    if(    !h
        || !conf_obj_valid(addr)
        || (bit_index > 31)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_cmd_t* p_l3_cmd = (struct lt_l3_i_config_write_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_write_res_t* p_l3_res = (struct lt_l3_i_config_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_I_CONFIG_WRITE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_I_CONFIG_WRITE_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;
    p_l3_cmd->bit_index = bit_index;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_I_CONFIG_WRITE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj)
{
    if(    !h
        || !conf_obj_valid(addr)
        || !obj
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_cmd_t* p_l3_cmd = (struct lt_l3_i_config_read_cmd_t*)&h->l3_buff;
    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_i_config_read_res_t* p_l3_res = (struct lt_l3_i_config_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_I_CONFIG_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_I_CONFIG_READ_CMD_ID;
    p_l3_cmd->address = (uint16_t)addr;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_I_CONFIG_READ_RES_SIZE != (p_l3_res->res_size)) {
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_write_res_t* p_l3_res = (struct lt_l3_r_mem_data_write_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = size + 4;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_WRITE_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;
    memcpy(p_l3_cmd->data, data, size);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_MEM_DATA_WRITE_RES_SIZE != (p_l3_res->res_size)) {
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_read_res_t* p_l3_res = (struct lt_l3_r_mem_data_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_MEM_DATA_READ_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_READ_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_MEM_DATA_READ_RES_SIZE_MIN + size != (p_l3_res->res_size)) {
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_r_mem_data_erase_res_t* p_l3_res = (struct lt_l3_r_mem_data_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_R_MEM_DATA_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_R_MEM_DATA_ERASE_CMD_ID;
    p_l3_cmd->udata_slot = udata_slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_R_MEM_DATA_ERASE_RES_SIZE != (p_l3_res->res_size)) {
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_RANDOM_VALUE_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_RANDOM_VALUE_GET_CMD_ID;
    p_l3_cmd->n_bytes = len;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_RANDOM_VALUE_GET_RES_SIZE_MIN + len != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(buff, p_l3_res->random_data, p_l3_res->res_size);

    return LT_OK;
}

lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve)
{
    if(    !h
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
        || ((curve != CURVE_P256) && (curve != CURVE_ED25519))
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_generate_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_generate_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_GENERATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_ECC_KEY_GENERATE_CMD_ID;
    p_l3_cmd->slot = (uint8_t)slot;
    p_l3_cmd->curve = (uint8_t)curve;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_ECC_KEY_GENERATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key)
{
    if(    !h
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
        || ((curve != CURVE_P256) && (curve != CURVE_ED25519))
        || !key
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ecc_key_store_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_store_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_store_res_t* p_l3_res = (struct lt_l3_ecc_key_store_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_STORE_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECC_KEY_STORE_CMD_ID;
    p_l3_cmd->slot = slot;
    p_l3_cmd->curve = curve;
    memcpy(p_l3_cmd->k, key, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_ECC_KEY_STORE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t slot, uint8_t *key, const uint8_t keylen, lt_ecc_curve_type_t *curve, ecc_key_origin_t *origin)
{
    if(    !h
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_read_res_t* p_l3_res = (struct lt_l3_ecc_key_read_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_READ_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECC_KEY_READ_CMD_ID;
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if((p_l3_res->curve == (uint8_t)CURVE_ED25519) && ((p_l3_res->res_size -1-1-1-13) != 32)) {  // TODO make it nicer
        return LT_FAIL;
    }
    if((p_l3_res->curve == (uint8_t)CURVE_P256) && ((p_l3_res->res_size -1-1-1-13) != 64)) {     // TODO make it nicer
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
    // TODO I think that we are not checking exactly curve type in this function?
    // Function should return fail if curve is neither ED25519 or P256
    return LT_OK;
}

lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot)
{
    if( !h
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_ecc_key_erase_cmd_t* p_l3_cmd = (struct lt_l3_ecc_key_erase_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_erase_res_t* p_l3_res = (struct lt_l3_ecc_key_erase_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_ERASE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_ECC_KEY_ERASE_CMD_ID;
    p_l3_cmd->slot = slot;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_ECC_KEY_ERASE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len, uint8_t *rs, const uint8_t rs_len)
{
    if(    !h
        || !msg
        || !rs
        || (msg_len > LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX) || (rs_len < 64)
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
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
    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecdsa_sign_res_t* p_l3_res = (struct lt_l3_ecdsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECDSA_SIGN_CMD_SIZE;
    p_l3_cmd->cmd_id= LT_L3_ECDSA_SIGN_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg_hash, msg_hash, 32);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_ECDSA_SIGN_RES_SIZE != (p_l3_res->res_size)) {
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
        || ((msg_len < LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MIN) | (msg_len > LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX))
        || slot < ECC_SLOT_0
        || slot > ECC_SLOT_31
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_eddsa_sign_cmd_t* p_l3_cmd = (struct lt_l3_eddsa_sign_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_EDDSA_SIGN_CMD_SIZE_MIN + msg_len - 1;   // -1 Because the LT_L3_EDDSA_SIGN_CMD_SIZE_MIN already includes minimal message size 1B
    p_l3_cmd->cmd_id = LT_L3_EDDSA_SIGN_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->msg, msg, msg_len);

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_EDDSA_SIGN_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(rs, p_l3_res->r, 32);
    memcpy(rs + 32, p_l3_res->s, 32);

    return LT_OK;
}

lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    if (   !msg
        || ((msg_len < LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MIN) | (msg_len > LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX))
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

lt_ret_t lt_mcounter_init(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index, const uint32_t mcounter_value)
{
    if( !h
        || ((mcounter_index < 0) | (mcounter_index > 15))
        || mcounter_value == 0
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_init_cmd_t* p_l3_cmd = (struct lt_l3_mcounter_init_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_init_res_t* p_l3_res = (struct lt_l3_mcounter_init_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_MCOUNTER_INIT_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_MCOUNTER_INIT_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;
    p_l3_cmd->mcounter_val = mcounter_value;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_MCOUNTER_INIT_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}
lt_ret_t lt_mcounter_update(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index)
{
    if( !h
        || ((mcounter_index < 0) | (mcounter_index > 15))
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_update_cmd_t* p_l3_cmd = (struct lt_l3_mcounter_update_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_update_res_t* p_l3_res = (struct lt_l3_mcounter_update_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_MCOUNTER_UPDATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_MCOUNTER_UPDATE_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_MCOUNTER_UPDATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_mcounter_get(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index, uint32_t *mcounter_value)
{
    if( !h
        || ((mcounter_index < 0) | (mcounter_index > 15))
        || !mcounter_value
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mcounter_get_cmd_t* p_l3_cmd = (struct lt_l3_mcounter_get_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_mcounter_get_res_t* p_l3_res = (struct lt_l3_mcounter_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_MCOUNTER_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_MCOUNTER_GET_CMD_ID;
    p_l3_cmd->mcounter_index = mcounter_index;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_MCOUNTER_GET_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    *mcounter_value = p_l3_res->mcounter_val;

    return LT_OK;
}

lt_ret_t lt_mac_and_destroy(lt_handle_t *h, mac_and_destroy_slot_t slot, const uint8_t *data_out, uint8_t *data_in)
{
    if( !h
        || !data_out
        || !data_in
        || slot > MAC_AND_DESTROY_SLOT_127
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Setup a pointer to l3 buffer, which is placed in handle
    struct lt_l3_mac_and_destroy_cmd_t * p_l3_cmd = (struct lt_l3_mac_and_destroy_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_mac_and_destroy_res_t* p_l3_res = (struct lt_l3_mac_and_destroy_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_MAC_AND_DESTROY_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_MAC_AND_DESTROY_CMD_ID;
    p_l3_cmd->slot = slot;
    memcpy(p_l3_cmd->data_in, data_out, MAC_AND_DESTROY_DATA_SIZE);
    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_MAC_AND_DESTROY_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(data_in, p_l3_res->data_out, MAC_AND_DESTROY_DATA_SIZE);

    return LT_OK;
}

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
    struct lt_l3_serial_code_get_cmd_t* p_l3_cmd = (struct lt_l3_serial_code_get_cmd_t*)&h->l3_buff;
    // Pointer to access l3 buffer with result's data
    struct lt_l3_serial_code_get_res_t* p_l3_res = (struct lt_l3_serial_code_get_res_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_SERIAL_CODE_GET_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_SERIAL_CODE_GET_CMD_ID;

    lt_ret_t ret = lt_l3_cmd(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_SERIAL_CODE_GET_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(serial_code, p_l3_res->serial_code, SERIAL_CODE_SIZE);

    return LT_OK;
}

const char *lt_ret_verbose(lt_ret_t ret) {
    switch(ret) {
        // libtropic
        case LT_OK:
            return "LT_OK";
        case LT_FAIL:
            return "LT_FAIL";
        case LT_HOST_NO_SESSION:
            return "LT_HOST_NO_SESSION";
        case LT_PARAM_ERR:
            return "LT_PARAM_ERR";
        case LT_CRYPTO_ERR:
            return "LT_CRYPTO_ERR";
        // L1
        case LT_L1_SPI_ERROR:
            return "LT_L1_SPI_ERROR";
        case LT_L1_DATA_LEN_ERROR:
            return "LT_L1_DATA_LEN_ERROR";
        case LT_L1_CHIP_ALARM_MODE:
            return "LT_L1_CHIP_ALARM_MODE";
        case LT_L1_CHIP_STARTUP_MODE:
            return "LT_L1_CHIP_STARTUP_MODE";
        case LT_L1_CHIP_BUSY:
            return "LT_L1_CHIP_BUSY";
        // L3 custom
        case LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL:
            return "LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL";
        case LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED:
            return "LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED";
        case LT_L3_ECC_INVALID_KEY:
            return "LT_L3_ECC_INVALID_KEY";
        case LT_L3_MCOUNTER_UPDATE_UPDATE_ERR:
            return "LT_L3_MCOUNTER_UPDATE_UPDATE_ERR";
        case LT_L3_COUNTER_INVALID:
            return "LT_L3_COUNTER_INVALID";
        case LT_L3_PAIRING_KEY_EMPTY:
            return "LT_L3_PAIRING_KEY_EMPTY";
        case LT_L3_PAIRING_KEY_INVALID:
            return "LT_L3_PAIRING_KEY_INVALID";
        // L3 universal
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
        // Default
        default:
            return "FATAL ERROR, unknown return value";
  }
}

//--------------------------------------------------------------------------------------------------------//
#ifdef LT_UTILS
/** @brief This helper structure together with two get* interfaces is meant to be used to simplify looping
 *         through all config addresses and printing them out into log */
struct lt_config_obj_desc_t config_description_table[27] = {
    {"CONFIGURATION_OBJECTS_CFG_START_UP                   ", CONFIGURATION_OBJECTS_CFG_START_UP_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_SLEEP_MODE                 ", CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_SENSORS                    ", CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_DEBUG                      ", CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE      ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ       ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE   ", CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ          ", CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE         ", CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ          ", CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PING                   ", CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE       ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ        ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE       ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET       ", CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE       ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE          ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ           ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE          ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN             ", CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN             ", CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT          ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET           ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE        ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY        ", CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR},
    {"CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET        ", CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR}
};

uint16_t get_conf_addr(uint8_t i)
{
    return config_description_table[i].addr;
}

const char *get_conf_desc(uint8_t i)
{
    return config_description_table[i].desc;
}

lt_ret_t read_whole_R_config(lt_handle_t *h, struct lt_config_t *config)
{
    lt_ret_t ret;

    for (uint8_t i=0; i<27;i++) {
        ret = lt_r_config_read(h, get_conf_addr(i), &config->obj[i]);
        if(ret != LT_OK) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t write_whole_R_config(lt_handle_t *h, const struct lt_config_t *config)
{
    lt_ret_t ret;

    for(int i=0; i<27;i++) {
        ret = lt_r_config_write(h, get_conf_addr(i), config->obj[i]);
        if(ret != LT_OK) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t verify_chip_and_start_secure_session(lt_handle_t *h, uint8_t *shipriv, uint8_t *shipub, uint8_t pkey_index)
{
    lt_ret_t ret = LT_FAIL;

    // This is not used in this example, but let's read it anyway
    uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
    ret = lt_get_info_chip_id(h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    // This is not used in this example, but let's read it anyway
    uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    // This is not used in this example, but let's read it anyway
    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};
    ret = lt_get_info_spect_fw_ver(h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    uint8_t X509_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
    ret = lt_get_info_cert(h, X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE);
    if (ret != LT_OK) {
        return ret;
    }

    uint8_t stpub[32] = {0};
    ret = lt_cert_verify_and_parse(X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_session_start(h, stpub, pkey_index, shipriv, shipub);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
#endif
