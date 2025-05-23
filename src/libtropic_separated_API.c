
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_separated_API.h"
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




lt_ret_t lt_out__session_start(lt_handle_t *h, const pkey_index_t pkey_index, session_state_t *state)
{
    if (   !h
        || (pkey_index > PAIRING_KEY_SLOT_INDEX_3)
        || !state
    ) {
        return LT_PARAM_ERR;
    }

    memset(h, 0, sizeof(lt_handle_t));
    // Create ephemeral host keys
    //uint8_t ehpriv[32];
    //uint8_t ehpub[32];
    lt_ret_t ret = lt_random_bytes((uint32_t*)state->ehpriv, 8);
    if(ret != LT_OK) {
        return ret;
    }
    lt_X25519_scalarmult(state->ehpriv, state->ehpub);

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_req_t* p_req = (struct lt_l2_handshake_req_t*)h->l2_buff;

    p_req->req_id = LT_L2_HANDSHAKE_REQ_ID;
    p_req->req_len = LT_L2_HANDSHAKE_REQ_LEN;
    memcpy(p_req->e_hpub, state->ehpub, 32);
    p_req->pkey_index = pkey_index;

    return LT_OK;

    // Send l2 separately //
}


lt_ret_t lt_in__session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub, session_state_t *state)
{
    if (   !h
        || !stpub
        || (pkey_index > PAIRING_KEY_SLOT_INDEX_3)
        || !shipriv
        || !shipub
        || !state
    ) {
        return LT_PARAM_ERR;
    }

    // Setup a response pointer to l2 buffer, which is placed in handle
    struct lt_l2_handshake_rsp_t* p_rsp = (struct lt_l2_handshake_rsp_t*)h->l2_buff;

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
    lt_sha256_update(&hctx, state->ehpub, 32);
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
    lt_X25519(state->ehpriv, p_rsp->e_tpub, shared_secret);
    lt_hkdf(protocol_name, 32, shared_secret, 32, 1, output_1, output_2);
    // ck = HKDF (ck, X25519(SHiPRIV, ETPUB), 1)
    lt_X25519(shipriv, p_rsp->e_tpub, shared_secret);
    lt_hkdf(output_1, 32, shared_secret, 32, 1, output_1, output_2);
    // ck, kAUTH = HKDF (ck, X25519(EHPRIV, STPUB), 2)
    lt_X25519(state->ehpriv, stpub, shared_secret);
    uint8_t kauth[32] = {0};
    lt_hkdf(output_1, 32, shared_secret, 32, 2, output_1, kauth);
    // kCMD, kRES = HKDF (ck, emptystring, 2)
    uint8_t kcmd[32] = {0};
    uint8_t kres[32] = {0};
    lt_hkdf(output_1, 32, (uint8_t*)"", 0, 2, kcmd, kres);

    lt_ret_t ret = lt_aesgcm_init_and_key(&h->decrypt, kauth, 32);
    if(ret != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    ret = lt_aesgcm_decrypt(&h->decrypt, h->decryption_IV, 12u, hash, 32, (uint8_t*)"", 0, p_rsp->t_tauth, 16u);
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









lt_ret_t lt_out__ping(lt_handle_t *h, ping_state_t *state, const uint8_t *msg_out, const uint16_t len)
{
    if(    !h
        || !state
        || !msg_out
        || (len > PING_LEN_MAX)
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Save len into a state structure, so it can be used in decode function
    state->len = len;

    // Pointer to access l3 buffer when it contains command data
    struct lt_l3_ping_cmd_t* p_l3_cmd = (struct lt_l3_ping_cmd_t*)&h->l3_buff;

    // Fill l3 buffer
    p_l3_cmd->cmd_size = len + LT_L3_PING_CMD_SIZE_MIN;
    p_l3_cmd->cmd_id = LT_L3_PING_CMD_ID;
    memcpy(p_l3_cmd->data_in, msg_out, len);

    lt_ret_t ret = lt_l3_encrypt_request(h);
    if(ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
lt_ret_t lt_in__ping(lt_handle_t *h, ping_state_t *state, uint8_t *msg_in)
{
    if(    !h
        || !state
        || !msg_in
    ) {
        return LT_PARAM_ERR;
    }
    if(h->session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;

    lt_ret_t ret = lt_l3_decrypt_response(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if((LT_L3_PING_CMD_SIZE_MIN + state->len) != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    memcpy(msg_in, p_l3_res->data_out, state->len);

    return LT_OK;
}


lt_ret_t lt_out__ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve)
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

    // Fill l3 buffer
    p_l3_cmd->cmd_size = LT_L3_ECC_KEY_GENERATE_CMD_SIZE;
    p_l3_cmd->cmd_id = LT_L3_ECC_KEY_GENERATE_CMD_ID;
    p_l3_cmd->slot = (uint8_t)slot;
    p_l3_cmd->curve = (uint8_t)curve;

    lt_ret_t ret = lt_l3_encrypt_request(h);
    if(ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_in__ecc_key_generate(lt_handle_t *h)
{
    if (    !h
    ) {
        return LT_PARAM_ERR;
    }

    // Pointer to access l3 buffer with result's data
    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;

    lt_ret_t ret = lt_l3_decrypt_response(h);
    if(ret != LT_OK) {
        return ret;
    }

    // Check incomming l3 length
    if(LT_L3_ECC_KEY_GENERATE_RES_SIZE != (p_l3_res->res_size)) {
        return LT_FAIL;
    }

    return LT_OK;
}



