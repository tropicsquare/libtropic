/**
 * @file libtropic.c
 * @brief Implementation of libtropic API
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_l2.h"
#include "libtropic_l3.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "lt_asn1_der.h"
#include "lt_crypto_common.h"
#include "lt_hkdf.h"
#include "lt_l1.h"
#include "lt_l1_port_wrap.h"
#include "lt_l2_api_structs.h"
#include "lt_l3_api_structs.h"
#include "lt_l3_process.h"
#include "lt_random.h"
#include "lt_sha256.h"
#include "lt_tr01_props.h"
#include "lt_x25519.h"

#define TR01_GET_INFO_BLOCK_LEN 128

lt_ret_t lt_init(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;

    // When compiling libtropic with l3 buffer embedded into handle,
    // define buffer's length here (later used to prevent overflow during communication).
#if !LT_SEPARATE_L3_BUFF
    h->l3.buff_len = LT_SIZE_OF_L3_BUFF;  // Size of l3 buffer is defined in libtropic_common.h
#endif

    h->l3.session_status = LT_SECURE_SESSION_OFF;
    ret = lt_l1_init(&h->l2);
    h->l2.startup_req_sent = false;
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_crypto_ctx_init(h->l3.crypto_ctx);
    if (ret != LT_OK) {
        return ret;
    }

    // Prevent usage of insufficient buffer.
    if (h->l3.buff_len < LT_SIZE_OF_L3_BUFF) {
        return LT_L3_BUFFER_TOO_SMALL;
    }

    // Initialize the TROPIC01 properties based on its Application FW.
    ret = lt_init_tr01_props(h);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_deinit(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }

    lt_l3_invalidate_host_session_data(&h->l3);

    lt_ret_t ret = lt_l1_deinit(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_update_mode(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;

    // The byte used here must not be ID byte of some request, otherwise chip would be confused
    // and would return CRC error.
    // GET_RESP 0xAA works fine.
    h->l2.buff[0] = TR01_L1_GET_RESPONSE_REQ_ID;

    // Transfer just one byte to read CHIP_STATUS byte
    ret = lt_l1_spi_csn_low(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l1_spi_transfer(&h->l2, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT);
    if (ret != LT_OK) {
        lt_ret_t ret_unused = lt_l1_spi_csn_high(&h->l2);
        LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
        return ret;
    }

    ret = lt_l1_spi_csn_high(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    // Buffer in handle now contains CHIP_STATUS byte,
    // Save info about chip mode into 'mode' variable in handle
    if (h->l2.buff[0] & TR01_L1_CHIP_MODE_STARTUP_bit) {
        h->l2.mode = LT_TR01_MAINTENANCE_MODE;
    }
    else {
        h->l2.mode = LT_TR01_APP_MODE;
    }

    return LT_OK;
}

lt_ret_t lt_get_info_cert_store(lt_handle_t *h, struct lt_cert_store_t *store)
{
    if (!h || !store) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer with request data
    struct lt_l2_get_info_req_t *p_l2_req = (struct lt_l2_get_info_req_t *)h->l2.buff;

    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;

    // Max cert-store length not read out -> Optimized as being read to read out only needed part!
    int curr_cert = LT_CERT_KIND_DEVICE;
    uint8_t *cert_head = store->certs[curr_cert];

    // Worst case full ceert-store is read out
    for (int i = 0; i < (TR01_L2_GET_INFO_REQ_CERT_SIZE_TOTAL / TR01_GET_INFO_BLOCK_LEN); i++) {
        p_l2_req->req_id = TR01_L2_GET_INFO_REQ_ID;
        p_l2_req->req_len = TR01_L2_GET_INFO_REQ_LEN;
        p_l2_req->object_id = TR01_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE;
        p_l2_req->block_index = i;

        lt_ret_t ret = lt_l2_send(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }

        ret = lt_l2_receive(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }

        if (TR01_GET_INFO_BLOCK_LEN != (p_l2_resp->rsp_len)) {
            return LT_L2_RSP_LEN_ERROR;
        }

        uint8_t *head = ((struct lt_l2_get_info_rsp_t *)h->l2.buff)->object;
        uint8_t *tail = head + TR01_GET_INFO_BLOCK_LEN;

        // Parse the header - Gets lengths and checks buffers are large enough
        if (i == 0) {
            if (*head != LT_CERT_STORE_VERSION) {
                return LT_CERT_STORE_INVALID;
            }
            head++;
            if (*head != LT_NUM_CERTIFICATES) {
                return LT_CERT_STORE_INVALID;
            }
            head++;

            for (int j = 0; j < LT_NUM_CERTIFICATES; j++) {
                uint16_t curr_len = *head;
                curr_len <<= 8;
                head++;
                curr_len |= *head;
                head++;

                if (curr_len > store->buf_len[j]) {
                    return LT_PARAM_ERR;
                }
                store->cert_len[j] = curr_len;
            }
        }

        // Read out and copy certificate chunk. Assumes that:
        //  - A single certificate is always larger than 128 bytes -> There is at most one "trailing chunk"!
        //    No need to handle this case explicitly since it is very likely to be like that, but worth mentioning.

        // Copy certificate chunk
        int available = tail - head;
        int till_now = cert_head - store->certs[curr_cert];
        int till_end = store->cert_len[curr_cert] - till_now;

        int to_copy = (till_end > available) ? available : till_end;

        memcpy(cert_head, head, to_copy);
        cert_head += to_copy;
        head += to_copy;

        // Move to the next certificate or finish upon last chunk of last certificate
        if ((cert_head - store->certs[curr_cert]) >= store->cert_len[curr_cert]) {
            if (curr_cert >= LT_NUM_CERTIFICATES - 1) {
                break;
            }
            else {
                curr_cert++;
                cert_head = store->certs[curr_cert];
            }
        }

        // Handle trailing rest of the next certificate
        if (available > to_copy) {
            int trailer_len = TR01_GET_INFO_BLOCK_LEN - to_copy;
            memcpy(cert_head, head, trailer_len);
            cert_head += trailer_len;
        }
    }

    return LT_OK;
}

lt_ret_t lt_get_st_pub(const struct lt_cert_store_t *store, uint8_t *stpub)
{
    if (!store || !stpub) {
        return LT_PARAM_ERR;
    }

    uint8_t *head = store->certs[LT_CERT_KIND_DEVICE];
    uint16_t len = store->cert_len[LT_CERT_KIND_DEVICE];

    return asn1der_find_object(head, len, LT_OBJ_ID_CURVEX25519, stpub, TR01_STPUB_LEN, LT_ASN1DER_CROP_PREFIX);
}

lt_ret_t lt_get_info_chip_id(lt_handle_t *h, struct lt_chip_id_t *chip_id)
{
    if (!h || !chip_id) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t *p_l2_req = (struct lt_l2_get_info_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = TR01_L2_GET_INFO_REQ_LEN;
    p_l2_req->object_id = TR01_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID;
    p_l2_req->block_index = TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_GET_INFO_CHIP_ID_SIZE != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    memcpy(chip_id, ((struct lt_l2_get_info_rsp_t *)h->l2.buff)->object, TR01_L2_GET_INFO_CHIP_ID_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t *ver)
{
    if (!h || !ver) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t *p_l2_req = (struct lt_l2_get_info_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = TR01_L2_GET_INFO_REQ_LEN;
    p_l2_req->object_id = TR01_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION;
    p_l2_req->block_index = TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_GET_INFO_RISCV_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t *)h->l2.buff)->object, TR01_L2_GET_INFO_RISCV_FW_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t *ver)
{
    if (!h || !ver) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t *p_l2_req = (struct lt_l2_get_info_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = TR01_L2_GET_INFO_REQ_LEN;
    p_l2_req->object_id = TR01_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION;
    p_l2_req->block_index = TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_GET_INFO_SPECT_FW_SIZE != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    memcpy(ver, ((struct lt_l2_get_info_rsp_t *)h->l2.buff)->object, TR01_L2_GET_INFO_SPECT_FW_SIZE);

    return LT_OK;
}

lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, const lt_bank_id_t bank_id, uint8_t *header,
                             const uint16_t header_max_size, uint16_t *header_read_size)
{
    if (!h || !header || !header_read_size
        || ((bank_id != TR01_FW_BANK_FW1) && (bank_id != TR01_FW_BANK_FW2) && (bank_id != TR01_FW_BANK_SPECT1)
            && (bank_id != TR01_FW_BANK_SPECT2))) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_info_req_t *p_l2_req = (struct lt_l2_get_info_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_GET_INFO_REQ_ID;
    p_l2_req->req_len = TR01_L2_GET_INFO_REQ_LEN;
    p_l2_req->object_id = TR01_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK;
    p_l2_req->block_index = bank_id;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if ((TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V1 != p_l2_resp->rsp_len)
        && (TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2 != p_l2_resp->rsp_len)
        && (TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2_EMPTY_BANK != p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    // Check if the output buffer for the header is big enough
    if (header_max_size < p_l2_resp->rsp_len) {
        *header_read_size = 0;
        return LT_PARAM_ERR;
    }

    memcpy(header, ((struct lt_l2_get_info_rsp_t *)h->l2.buff)->object, p_l2_resp->rsp_len);
    *header_read_size = p_l2_resp->rsp_len;

    return LT_OK;
}

lt_ret_t lt_session_start(lt_handle_t *h, const uint8_t *stpub, const lt_pkey_index_t pkey_index,
                          const uint8_t *shipriv, const uint8_t *shipub)
{
    if (!h || !stpub || (pkey_index > TR01_PAIRING_KEY_SLOT_INDEX_3) || !shipriv || !shipub) {
        return LT_PARAM_ERR;
    }

    lt_host_eph_keys_t host_eph_keys = {0};

    lt_ret_t ret = lt_out__session_start(h, pkey_index, &host_eph_keys);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_in__session_start(h, stpub, pkey_index, shipriv, shipub, &host_eph_keys);
    memset(&host_eph_keys, 0, sizeof(lt_host_eph_keys_t));

    return ret;
}

lt_ret_t lt_session_abort(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }

    lt_l3_invalidate_host_session_data(&h->l3);

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_encrypted_session_abt_req_t *p_l2_req = (struct lt_l2_encrypted_session_abt_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_encrypted_session_abt_rsp_t *p_l2_resp = (struct lt_l2_encrypted_session_abt_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_ENCRYPTED_SESSION_ABT_ID;
    p_l2_req->req_len = TR01_L2_ENCRYPTED_SESSION_ABT_LEN;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_ENCRYPTED_SESSION_ABT_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_sleep(lt_handle_t *h, const uint8_t sleep_kind)
{
    if (!h || ((sleep_kind != TR01_L2_SLEEP_KIND_SLEEP))) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_startup_req_t *p_l2_req = (struct lt_l2_startup_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_startup_rsp_t *p_l2_resp = (struct lt_l2_startup_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_SLEEP_REQ_ID;
    p_l2_req->req_len = TR01_L2_SLEEP_REQ_LEN;
    p_l2_req->startup_id = sleep_kind;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_SLEEP_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_reboot(lt_handle_t *h, const lt_startup_id_t startup_id)
{
    if (!h || ((startup_id != TR01_REBOOT) && (startup_id != TR01_MAINTENANCE_REBOOT))) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_startup_req_t *p_l2_req = (struct lt_l2_startup_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_startup_rsp_t *p_l2_resp = (struct lt_l2_startup_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_STARTUP_REQ_ID;
    p_l2_req->req_len = TR01_L2_STARTUP_REQ_LEN;
    p_l2_req->startup_id = startup_id;

    lt_ret_t ret = lt_l2_send(&h->l2);
    h->l2.startup_req_sent = true;
    if (ret != LT_OK) {
        h->l2.startup_req_sent = false;
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    h->l2.startup_req_sent = false;
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_STARTUP_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    ret = lt_l1_delay(&h->l2, LT_TR01_REBOOT_DELAY_MS);
    if (ret != LT_OK) {
        return ret;
    }

    // Update mode variable in handle after reboot
    ret = lt_update_mode(h);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

#ifdef ABAB
lt_ret_t lt_mutable_fw_erase(lt_handle_t *h, const lt_bank_id_t bank_id)
{
    if (!h
        || ((bank_id != TR01_FW_BANK_FW1) && (bank_id != TR01_FW_BANK_FW2) && (bank_id != TR01_FW_BANK_SPECT1)
            && (bank_id != TR01_FW_BANK_SPECT2))) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_erase_req_t *p_l2_req = (struct lt_l2_mutable_fw_erase_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_erase_rsp_t *p_l2_resp = (struct lt_l2_mutable_fw_erase_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_MUTABLE_FW_ERASE_REQ_ID;
    p_l2_req->req_len = TR01_L2_MUTABLE_FW_ERASE_REQ_LEN;
    p_l2_req->bank_id = bank_id;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_MUTABLE_FW_ERASE_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *fw_data, const uint16_t fw_data_size, lt_bank_id_t bank_id)
{
    if (!h || !fw_data || fw_data_size > TR01_MUTABLE_FW_UPDATE_SIZE_MAX
        || ((bank_id != TR01_FW_BANK_FW1) && (bank_id != TR01_FW_BANK_FW2) && (bank_id != TR01_FW_BANK_SPECT1)
            && (bank_id != TR01_FW_BANK_SPECT2))) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_update_req_t *p_l2_req = (struct lt_l2_mutable_fw_update_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_update_rsp_t *p_l2_resp = (struct lt_l2_mutable_fw_update_rsp_t *)h->l2.buff;

    uint16_t loops = fw_data_size / 128;
    uint16_t rest = fw_data_size % 128;

    for (int16_t i = 0; i < loops; i++) {
        p_l2_req->req_id = TR01_L2_MUTABLE_FW_UPDATE_REQ_ID;
        p_l2_req->req_len = TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN + 128;
        p_l2_req->bank_id = bank_id;
        p_l2_req->offset = i * 128;
        memcpy(p_l2_req->data, fw_data + (i * 128), 128);

        lt_ret_t ret = lt_l2_send(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }
        ret = lt_l2_receive(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }

        if (TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
            return LT_L2_RSP_LEN_ERROR;
        }
    }

    if (rest != 0) {
        p_l2_req->req_id = TR01_L2_MUTABLE_FW_UPDATE_REQ_ID;
        p_l2_req->req_len = TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN + rest;
        p_l2_req->bank_id = bank_id;
        p_l2_req->offset = loops * 128;
        memcpy(p_l2_req->data, fw_data + (loops * 128), rest);

        lt_ret_t ret = lt_l2_send(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }
        ret = lt_l2_receive(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }

        if (TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
            return LT_L2_RSP_LEN_ERROR;
        }
    }

    return LT_OK;
}
#elif ACAB
lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *update_request)
{
    if (!h || !update_request) {
        return LT_PARAM_ERR;
    }

    // This structure reflects incomming data and is used for passing those data into l2 frame
    struct data_format_t {
        uint8_t req_len;        /**< Length byte */
        uint8_t signature[64];  /**< Signature of SHA256 hash of all following data in this packet */
        uint8_t hash[32];       /**< SHA256 HASH of first FW chunk of data sent using Mutable_FW_Update_Data */
        uint16_t type;          /**< FW type which is going to be updated */
        uint8_t padding;        /**< Padding, zero value */
        uint8_t header_version; /**< Version of used header */
        uint32_t version;       /**< Version of FW */
    } __attribute__((__packed__));

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_update_req_t *p_l2_req = (struct lt_l2_mutable_fw_update_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_update_rsp_t *p_l2_resp = (struct lt_l2_mutable_fw_update_rsp_t *)h->l2.buff;

    // Setup a pointer to incomming data
    struct data_format_t *data_p = (struct data_format_t *)(update_request);

    p_l2_req->req_id = TR01_L2_MUTABLE_FW_UPDATE_REQ_ID;
    p_l2_req->req_len = TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN;

    memcpy(p_l2_req->signature, data_p->signature, sizeof(data_p->signature));
    memcpy(p_l2_req->hash, data_p->hash, sizeof(data_p->hash));

    p_l2_req->type = data_p->type;
    p_l2_req->padding = data_p->padding;
    p_l2_req->header_version = data_p->header_version;
    p_l2_req->version = data_p->version;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    if (TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
        return LT_L2_RSP_LEN_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_mutable_fw_update_data(lt_handle_t *h, const uint8_t *update_data, const uint16_t update_data_size)
{
    if (!h || !update_data || update_data_size > TR01_MUTABLE_FW_UPDATE_SIZE_MAX) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_mutable_fw_update_data_req_t *p2_l2_req = (struct lt_l2_mutable_fw_update_data_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_mutable_fw_update_rsp_t *p_l2_resp = (struct lt_l2_mutable_fw_update_rsp_t *)h->l2.buff;

    // Data consist of "request" and "data" parts,
    // 'data' byte chunks are taken from following index:
    int chunk_index = TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN + 1;

    do {
        uint8_t len = update_data[chunk_index];
        p2_l2_req->req_id = TR01_L2_MUTABLE_FW_UPDATE_DATA_REQ;
        memcpy((uint8_t *)&p2_l2_req->req_len, update_data + chunk_index, len + 1);

        lt_ret_t ret = lt_l2_send(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }
        ret = lt_l2_receive(&h->l2);
        if (ret != LT_OK) {
            return ret;
        }

        if (TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN != (p_l2_resp->rsp_len)) {
            return LT_L2_RSP_LEN_ERROR;
        }

        chunk_index += len + 1;
    } while ((chunk_index) < update_data_size);

    return LT_OK;
}
#else
#error "Undefined silicon revision. Please define either ABAB or ACAB."
#endif

lt_ret_t lt_get_log_req(lt_handle_t *h, uint8_t *log_msg, const uint16_t log_msg_max_size, uint16_t *log_msg_read_size)
{
    if (!h || !log_msg || !log_msg_read_size) {
        return LT_PARAM_ERR;
    }

    // Setup a request pointer to l2 buffer, which is placed in handle
    struct lt_l2_get_log_req_t *p_l2_req = (struct lt_l2_get_log_req_t *)h->l2.buff;
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_log_rsp_t *p_l2_resp = (struct lt_l2_get_log_rsp_t *)h->l2.buff;

    p_l2_req->req_id = TR01_L2_GET_LOG_REQ_ID;
    p_l2_req->req_len = TR01_L2_GET_LOG_REQ_LEN;

    lt_ret_t ret = lt_l2_send(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }
    ret = lt_l2_receive(&h->l2);
    if (ret != LT_OK) {
        return ret;
    }

    // Check if the output buffer for the log message is big enough
    if (log_msg_max_size < p_l2_resp->rsp_len) {
        *log_msg_read_size = 0;
        return LT_PARAM_ERR;
    }

    *log_msg_read_size = p_l2_resp->rsp_len;
    memcpy(log_msg, p_l2_resp->log_msg, p_l2_resp->rsp_len);

    return LT_OK;
}

lt_ret_t lt_ping(lt_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t msg_len)
{
    if (!h || !msg_out || !msg_in || (msg_len > TR01_PING_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ping(h, msg_out, msg_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_PING_RES_PACKET_SIZE_MAX));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ping(h, msg_in, msg_len);
}

lt_ret_t lt_pairing_key_write(lt_handle_t *h, const uint8_t *pairing_pub, const uint8_t slot)
{
    if (!h || !pairing_pub || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__pairing_key_write(h, pairing_pub, slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_PAIRING_KEY_WRITE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__pairing_key_write(h);
}

lt_ret_t lt_pairing_key_read(lt_handle_t *h, uint8_t *pairing_pub, const uint8_t slot)
{
    if (!h || !pairing_pub || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__pairing_key_read(h, slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_PAIRING_KEY_READ_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__pairing_key_read(h, pairing_pub);
}

lt_ret_t lt_pairing_key_invalidate(lt_handle_t *h, const uint8_t slot)
{
    if (!h || (slot > 3)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__pairing_key_invalidate(h, slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_PAIRING_KEY_INVALIDATE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__pairing_key_invalidate(h);
}

lt_ret_t lt_r_config_write(lt_handle_t *h, const enum lt_config_obj_addr_t addr, const uint32_t obj)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_config_write(h, addr, obj);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_R_CONFIG_WRITE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_config_write(h);
}

lt_ret_t lt_r_config_read(lt_handle_t *h, const enum lt_config_obj_addr_t addr, uint32_t *obj)
{
    if (!h || !obj) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_config_read(h, addr);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_R_CONFIG_READ_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_config_read(h, obj);
}

lt_ret_t lt_r_config_erase(lt_handle_t *h)
{
    if (!h) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_config_erase(h);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_R_CONFIG_ERASE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_config_erase(h);
}

lt_ret_t lt_i_config_write(lt_handle_t *h, const enum lt_config_obj_addr_t addr, const uint8_t bit_index)
{
    if (!h || (bit_index > 31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__i_config_write(h, addr, bit_index);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_I_CONFIG_WRITE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__i_config_write(h);
}

lt_ret_t lt_i_config_read(lt_handle_t *h, const enum lt_config_obj_addr_t addr, uint32_t *obj)
{
    if (!h || !obj) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__i_config_read(h, addr);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_I_CONFIG_READ_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__i_config_read(h, obj);
}

lt_ret_t lt_r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, const uint8_t *data, const uint16_t data_size)
{
    if (!h || !data || data_size < TR01_R_MEM_DATA_SIZE_MIN || data_size > h->tr01_props.r_mem_udata_slot_size_max
        || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_mem_data_write(h, udata_slot, data, data_size);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_R_MEM_DATA_WRITE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_mem_data_write(h);
}

lt_ret_t lt_r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t data_max_size,
                            uint16_t *data_read_size)
{
    if (!h || !data || !data_read_size || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_mem_data_read(h, udata_slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(
        &h->l2, h->l3.buff,
        lt_min(h->l3.buff_len, TR01_L3_SIZE_SIZE + TR01_L3_RESULT_SIZE
                                   + (TR01_L3_R_MEM_DATA_READ_PADDING_SIZE + h->tr01_props.r_mem_udata_slot_size_max)
                                   + TR01_L3_TAG_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_mem_data_read(h, data, data_max_size, data_read_size);
}

lt_ret_t lt_r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot)
{
    if (!h || (udata_slot > TR01_R_MEM_DATA_SLOT_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__r_mem_data_erase(h, udata_slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_R_MEM_DATA_ERASE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__r_mem_data_erase(h);
}

lt_ret_t lt_random_value_get(lt_handle_t *h, uint8_t *rnd_bytes, const uint16_t rnd_bytes_cnt)
{
    if (!h || !rnd_bytes || (rnd_bytes_cnt > TR01_RANDOM_VALUE_GET_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__random_value_get(h, rnd_bytes_cnt);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_RANDOM_VALUE_GET_RES_PACKET_SIZE_MAX));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__random_value_get(h, rnd_bytes, rnd_bytes_cnt);
}

lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const lt_ecc_slot_t slot, const lt_ecc_curve_type_t curve)
{
    if (!h || (slot > TR01_ECC_SLOT_31) || ((curve != TR01_CURVE_P256) && (curve != TR01_CURVE_ED25519))) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ecc_key_generate(h, slot, curve);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_ECC_KEY_GENERATE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_key_generate(h);
}

lt_ret_t lt_ecc_key_store(lt_handle_t *h, const lt_ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key)
{
    if (!h || (slot > TR01_ECC_SLOT_31) || ((curve != TR01_CURVE_P256) && (curve != TR01_CURVE_ED25519)) || !key) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }
    lt_ret_t ret = lt_out__ecc_key_store(h, slot, curve, key);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_ECC_KEY_STORE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_key_store(h);
}

lt_ret_t lt_ecc_key_read(lt_handle_t *h, const lt_ecc_slot_t ecc_slot, uint8_t *key, const uint8_t key_max_size,
                         lt_ecc_curve_type_t *curve, lt_ecc_key_origin_t *origin)
{
    if (!h || (ecc_slot > TR01_ECC_SLOT_31) || !key || !curve || !origin) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ecc_key_read(h, ecc_slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff,
                                   lt_min(h->l3.buff_len, TR01_L3_ECC_KEY_READ_RES_PACKET_SIZE_MAX));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_key_read(h, key, key_max_size, curve, origin);
}

lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const lt_ecc_slot_t ecc_slot)
{
    if (!h || (ecc_slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ecc_key_erase(h, ecc_slot);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_ECC_KEY_ERASE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_key_erase(h);
}

lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const lt_ecc_slot_t ecc_slot, const uint8_t *msg, const uint32_t msg_len,
                           uint8_t *rs)
{
    if (!h || !msg || !rs || (ecc_slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ecc_ecdsa_sign(h, ecc_slot, msg, msg_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_ECDSA_SIGN_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_ecdsa_sign(h, rs);
}

lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const lt_ecc_slot_t ecc_slot, const uint8_t *msg, const uint16_t msg_len,
                           uint8_t *rs)
{
    if (!h || !msg || !rs || (msg_len > TR01_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX) || (ecc_slot > TR01_ECC_SLOT_31)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__ecc_eddsa_sign(h, ecc_slot, msg, msg_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_EDDSA_SIGN_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__ecc_eddsa_sign(h, rs);
}

lt_ret_t lt_mcounter_init(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, const uint32_t mcounter_value)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15) || mcounter_value > TR01_MCOUNTER_VALUE_MAX) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__mcounter_init(h, mcounter_index, mcounter_value);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_MCOUNTER_INIT_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__mcounter_init(h);
}

lt_ret_t lt_mcounter_update(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15)) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__mcounter_update(h, mcounter_index);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_MCOUNTER_UPDATE_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__mcounter_update(h);
}

lt_ret_t lt_mcounter_get(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, uint32_t *mcounter_value)
{
    if (!h || (mcounter_index > TR01_MCOUNTER_INDEX_15) || !mcounter_value) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__mcounter_get(h, mcounter_index);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_MCOUNTER_GET_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__mcounter_get(h, mcounter_value);
}

lt_ret_t lt_mac_and_destroy(lt_handle_t *h, const lt_mac_and_destroy_slot_t slot, const uint8_t *data_out,
                            uint8_t *data_in)
{
    if (!h || !data_out || !data_in || slot > TR01_MAC_AND_DESTROY_SLOT_127) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    lt_ret_t ret = lt_out__mac_and_destroy(h, slot, data_out);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_send_encrypted_cmd(&h->l2, h->l3.buff, h->l3.buff_len);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_l2_recv_encrypted_res(&h->l2, h->l3.buff, lt_min(h->l3.buff_len, TR01_L3_MAC_AND_DESTROY_RES_PACKET_SIZE));
    if (ret != LT_OK) {
        return ret;
    }

    return lt_in__mac_and_destroy(h, data_in);
}

static const char *lt_ret_strs[] = {"LT_OK",
                                    "LT_FAIL",
                                    "LT_HOST_NO_SESSION",
                                    "LT_PARAM_ERR",
                                    "LT_CRYPTO_ERR",
                                    "LT_L1_SPI_ERROR",
                                    "LT_L1_DATA_LEN_ERROR",
                                    "LT_L1_CHIP_STARTUP_MODE",
                                    "LT_L1_CHIP_ALARM_MODE",
                                    "LT_L1_CHIP_BUSY",
                                    "LT_L1_INT_TIMEOUT",
                                    "LT_L3_SLOT_NOT_EMPTY",
                                    "LT_L3_SLOT_EXPIRED",
                                    "LT_L3_INVALID_KEY",
                                    "LT_L3_UPDATE_ERR",
                                    "LT_L3_COUNTER_INVALID",
                                    "LT_L3_SLOT_EMPTY",
                                    "LT_L3_SLOT_INVALID",
                                    "LT_L3_OK",
                                    "LT_L3_FAIL",
                                    "LT_L3_UNAUTHORIZED",
                                    "LT_L3_INVALID_CMD",
                                    "LT_L3_HARDWARE_FAIL",
                                    "LT_L3_DATA_LEN_ERROR",
                                    "LT_L3_RES_SIZE_ERROR",
                                    "LT_L3_BUFFER_TOO_SMALL",
                                    "LT_L3_R_MEM_DATA_READ_SLOT_EMPTY",
                                    "LT_L3_RESULT_UNKNOWN",
                                    "LT_L2_REQ_CONT",
                                    "LT_L2_RES_CONT",
                                    "LT_L2_RESP_DISABLED",
                                    "LT_L2_HSK_ERR",
                                    "LT_L2_NO_SESSION",
                                    "LT_L2_TAG_ERR",
                                    "LT_L2_CRC_ERR",
                                    "LT_L2_GEN_ERR",
                                    "LT_L2_NO_RESP",
                                    "LT_L2_UNKNOWN_REQ",
                                    "LT_L2_IN_CRC_ERR",
                                    "LT_L2_RSP_LEN_ERROR",
                                    "LT_L2_STATUS_UNKNOWN",
                                    "LT_CERT_STORE_INVALID",
                                    "LT_CERT_UNSUPPORTED",
                                    "LT_CERT_ITEM_NOT_FOUND",
                                    "LT_NONCE_OVERFLOW"};

const char *lt_ret_verbose(lt_ret_t ret)
{
    if (ret < LT_RET_T_LAST_VALUE) return lt_ret_strs[ret];

    return "FATAL ERROR, unknown return value";
}

//--------------------------------------------------------------------------------------------------------//
#ifdef LT_HELPERS

struct lt_config_obj_desc_t cfg_desc_table[LT_CONFIG_OBJ_CNT]
    = {{"TR01_CFG_START_UP                   ", TR01_CFG_START_UP_ADDR},
       {"TR01_CFG_SENSORS                    ", TR01_CFG_SENSORS_ADDR},
       {"TR01_CFG_DEBUG                      ", TR01_CFG_DEBUG_ADDR},
       {"TR01_CFG_GPO_ADDR                   ", TR01_CFG_GPO_ADDR},
       {"TR01_CFG_SLEEP_MODE                 ", TR01_CFG_SLEEP_MODE_ADDR},
       {"TR01_CFG_UAP_PAIRING_KEY_WRITE      ", TR01_CFG_UAP_PAIRING_KEY_WRITE_ADDR},
       {"TR01_CFG_UAP_PAIRING_KEY_READ       ", TR01_CFG_UAP_PAIRING_KEY_READ_ADDR},
       {"TR01_CFG_UAP_PAIRING_KEY_INVALIDATE ", TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR},
       {"TR01_CFG_UAP_R_CONFIG_WRITE_ERASE   ", TR01_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR},
       {"TR01_CFG_UAP_R_CONFIG_READ          ", TR01_CFG_UAP_R_CONFIG_READ_ADDR},
       {"TR01_CFG_UAP_I_CONFIG_WRITE         ", TR01_CFG_UAP_I_CONFIG_WRITE_ADDR},
       {"TR01_CFG_UAP_I_CONFIG_READ          ", TR01_CFG_UAP_I_CONFIG_READ_ADDR},
       {"TR01_CFG_UAP_PING                   ", TR01_CFG_UAP_PING_ADDR},
       {"TR01_CFG_UAP_R_MEM_DATA_WRITE       ", TR01_CFG_UAP_R_MEM_DATA_WRITE_ADDR},
       {"TR01_CFG_UAP_R_MEM_DATA_READ        ", TR01_CFG_UAP_R_MEM_DATA_READ_ADDR},
       {"TR01_CFG_UAP_R_MEM_DATA_ERASE       ", TR01_CFG_UAP_R_MEM_DATA_ERASE_ADDR},
       {"TR01_CFG_UAP_RANDOM_VALUE_GET       ", TR01_CFG_UAP_RANDOM_VALUE_GET_ADDR},
       {"TR01_CFG_UAP_ECC_KEY_GENERATE       ", TR01_CFG_UAP_ECC_KEY_GENERATE_ADDR},
       {"TR01_CFG_UAP_ECC_KEY_STORE          ", TR01_CFG_UAP_ECC_KEY_STORE_ADDR},
       {"TR01_CFG_UAP_ECC_KEY_READ           ", TR01_CFG_UAP_ECC_KEY_READ_ADDR},
       {"TR01_CFG_UAP_ECC_KEY_ERASE          ", TR01_CFG_UAP_ECC_KEY_ERASE_ADDR},
       {"TR01_CFG_UAP_ECDSA_SIGN             ", TR01_CFG_UAP_ECDSA_SIGN_ADDR},
       {"TR01_CFG_UAP_EDDSA_SIGN             ", TR01_CFG_UAP_EDDSA_SIGN_ADDR},
       {"TR01_CFG_UAP_MCOUNTER_INIT          ", TR01_CFG_UAP_MCOUNTER_INIT_ADDR},
       {"TR01_CFG_UAP_MCOUNTER_GET           ", TR01_CFG_UAP_MCOUNTER_GET_ADDR},
       {"TR01_CFG_UAP_MCOUNTER_UPDATE        ", TR01_CFG_UAP_MCOUNTER_UPDATE_ADDR},
       {"TR01_CFG_UAP_MAC_AND_DESTROY        ", TR01_CFG_UAP_MAC_AND_DESTROY_ADDR}};

lt_ret_t lt_read_whole_R_config(lt_handle_t *h, struct lt_config_t *config)
{
    if (!h || !config) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;

    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        ret = lt_r_config_read(h, cfg_desc_table[i].addr, &config->obj[i]);
        if (ret != LT_OK) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t lt_write_whole_R_config(lt_handle_t *h, const struct lt_config_t *config)
{
    if (!h || !config) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;

    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        ret = lt_r_config_write(h, cfg_desc_table[i].addr, config->obj[i]);
        if (ret != LT_OK) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t lt_read_whole_I_config(lt_handle_t *h, struct lt_config_t *config)
{
    if (!h || !config) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;

    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        ret = lt_i_config_read(h, cfg_desc_table[i].addr, &config->obj[i]);
        if (ret != LT_OK) {
            return ret;
        }
    }

    return LT_OK;
}

lt_ret_t lt_write_whole_I_config(lt_handle_t *h, const struct lt_config_t *config)
{
    if (!h || !config) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret;
    uint32_t cfg_obj;
    enum lt_config_obj_addr_t addr;

    for (uint8_t i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        cfg_obj = config->obj[i];
        addr = cfg_desc_table[i].addr;
        for (uint8_t j = 0; j <= 31; j++) {
            if (!FIELD_GET(BIT(j), cfg_obj)) {
                ret = lt_i_config_write(h, addr, j);
                if (ret != LT_OK) {
                    return ret;
                }
            }
        }
    }

    return LT_OK;
}

lt_ret_t lt_verify_chip_and_start_secure_session(lt_handle_t *h, const uint8_t *shipriv, const uint8_t *shipub,
                                                 const lt_pkey_index_t pkey_index)
{
    if (!h || !shipriv || !shipub || (pkey_index > TR01_PAIRING_KEY_SLOT_INDEX_3)) {
        return LT_PARAM_ERR;
    }

    lt_ret_t ret = LT_FAIL;

    // This is not used here, but let's read it anyway
    struct lt_chip_id_t chip_id = {0};
    ret = lt_get_info_chip_id(h, &chip_id);
    if (ret != LT_OK) {
        return ret;
    }

    // This is not used in this example, but let's read it anyway
    uint8_t riscv_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver);
    if (ret != LT_OK) {
        return ret;
    }

    // This is not used in this example, but let's read it anyway
    uint8_t spect_fw_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE] = {0};
    ret = lt_get_info_spect_fw_ver(h, spect_fw_ver);
    if (ret != LT_OK) {
        return ret;
    }

    // Read certificate store
    uint8_t cert_ese[TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE] = {0};
    uint8_t cert_xxxx[TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE] = {0};
    uint8_t cert_tr01[TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE] = {0};
    uint8_t cert_root[TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE] = {0};

    struct lt_cert_store_t cert_store
        = {.cert_len = {0, 0, 0, 0},
           .buf_len = {TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE, TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE,
                       TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE, TR01_L2_GET_INFO_REQ_CERT_SIZE_SINGLE},
           .certs = {cert_ese, cert_xxxx, cert_tr01, cert_root}};

    ret = lt_get_info_cert_store(h, &cert_store);
    if (ret != LT_OK) {
        return ret;
    }

    // Extract STPub
    uint8_t stpub[TR01_STPUB_LEN] = {0};
    ret = lt_get_st_pub(&cert_store, stpub);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_session_start(h, stpub, pkey_index, shipriv, shipub);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_print_bytes(const uint8_t *bytes, const uint16_t bytes_cnt, char *out_buf, const uint16_t out_buf_size)
{
    if (!bytes || !out_buf || out_buf_size < (bytes_cnt * 2 + 1)) {
        // Write empty string if buffer too small
        if (out_buf && out_buf_size > 0) {
            out_buf[0] = '\0';
        }
        return LT_FAIL;
    }

    for (uint16_t i = 0; i < bytes_cnt; i++) {
        sprintf(&out_buf[i * 2], "%02" PRIX8, bytes[i]);
    }
    out_buf[bytes_cnt * 2] = '\0';

    return LT_OK;
}

lt_ret_t lt_print_chip_id(const struct lt_chip_id_t *chip_id, int (*print_func)(const char *format, ...))
{
    if (!chip_id || !print_func) {
        return LT_PARAM_ERR;
    }

    char print_bytes_buff[LT_CHIP_ID_FIELD_MAX_SIZE];

    if (LT_OK
            != lt_print_bytes(chip_id->chip_id_ver, sizeof(chip_id->chip_id_ver), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("CHIP_ID ver            = 0x%s (v%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 ")\r\n",
                          print_bytes_buff, chip_id->chip_id_ver[0], chip_id->chip_id_ver[1], chip_id->chip_id_ver[2],
                          chip_id->chip_id_ver[3])) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->fl_chip_info, sizeof(chip_id->fl_chip_info), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("FL_PROD_DATA           = 0x%s (%s)\r\n", print_bytes_buff,
                          chip_id->fl_chip_info[0] == 0x01 ? "PASSED" : "N/A")) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->func_test_info, sizeof(chip_id->func_test_info), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("MAN_FUNC_TEST          = 0x%s (%s)\r\n", print_bytes_buff,
                          chip_id->func_test_info[0] == 0x01 ? "PASSED" : "N/A")) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->silicon_rev, sizeof(chip_id->silicon_rev), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Silicon rev            = 0x%s (%c%c%c%c)\r\n", print_bytes_buff, chip_id->silicon_rev[0],
                          chip_id->silicon_rev[1], chip_id->silicon_rev[2], chip_id->silicon_rev[3])) {
        return LT_FAIL;
    }

    uint16_t packg_type_id = ((uint16_t)chip_id->packg_type_id[0] << 8) | ((uint16_t)chip_id->packg_type_id[1]);
    if (LT_OK
        != lt_print_bytes(chip_id->packg_type_id, sizeof(chip_id->packg_type_id), print_bytes_buff,
                          sizeof(print_bytes_buff))) {
        return LT_FAIL;
    }
    char packg_type_id_str[17];  // Length of the longest possible string ("Bare silicon die") + \0.
    switch (packg_type_id) {
        case TR01_CHIP_PKG_BARE_SILICON_ID:
            strcpy(packg_type_id_str, "Bare silicon die");
            break;

        case TR01_CHIP_PKG_QFN32_ID:
            strcpy(packg_type_id_str, "QFN32, 4x4mm");
            break;

        default:
            strcpy(packg_type_id_str, "N/A");
            break;
    }
    if (0 > print_func("Package ID             = 0x%s (%s)\r\n", print_bytes_buff, packg_type_id_str)) {
        return LT_FAIL;
    }

    if (0 > print_func("Prov info ver          = 0x%02" PRIX8 " (v%" PRIu8 ")\r\n", chip_id->prov_ver_fab_id_pn[0],
                       chip_id->prov_ver_fab_id_pn[0])) {
        return LT_FAIL;
    }

    uint16_t parsed_fab_id = ((chip_id->prov_ver_fab_id_pn[1] << 4) | (chip_id->prov_ver_fab_id_pn[2] >> 4)) & 0xfff;
    switch (parsed_fab_id) {
        case TR01_FAB_ID_TROPIC_SQUARE_LAB:
            if (0
                > print_func("Fab ID                 = 0x%03" PRIX16 " (%s)\r\n", parsed_fab_id, "Tropic Square Lab")) {
                return LT_FAIL;
            }
            break;

        case TR01_FAB_ID_EPS_BRNO:
            if (0
                > print_func("Fab ID                 = 0x%03" PRIX16 " (%s)\r\n", parsed_fab_id, "EPS Global - Brno")) {
                return LT_FAIL;
            }
            break;

        default:
            if (0 > print_func("Fab ID         = 0x%03" PRIX16 " (%s)\r\n", parsed_fab_id, "N/A")) {
                return LT_FAIL;
            }
            break;
    }

    uint16_t parsed_short_pn = ((chip_id->prov_ver_fab_id_pn[2] << 8) | (chip_id->prov_ver_fab_id_pn[3])) & 0xfff;
    if (0 > print_func("P/N ID (short P/N)     = 0x%03" PRIX16 "\r\n", parsed_short_pn)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->provisioning_date, sizeof(chip_id->provisioning_date), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Prov date              = 0x%s \r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    if (LT_OK != lt_print_bytes(chip_id->hsm_ver, sizeof(chip_id->hsm_ver), print_bytes_buff, sizeof(print_bytes_buff))
        || 0 > print_func("HSM HW/FW/SW ver       = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->prog_ver, sizeof(chip_id->prog_ver), print_bytes_buff, sizeof(print_bytes_buff))
        || 0 > print_func("Programmer ver         = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes((uint8_t *)&chip_id->ser_num, sizeof(chip_id->ser_num), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("S/N                    = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    uint8_t pn_len = chip_id->part_num_data[0];
    uint8_t pn_data[16];  // 15B for data, last byte for '\0'
    memcpy(pn_data, &chip_id->part_num_data[1], pn_len);
    pn_data[pn_len] = '\0';
    if (LT_OK
            != lt_print_bytes(chip_id->part_num_data, sizeof(chip_id->part_num_data), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("P/N (long)             = 0x%s (%s)\r\n", print_bytes_buff, pn_data)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->prov_templ_ver, sizeof(chip_id->prov_templ_ver), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Prov template ver      = 0x%s (v%" PRIu8 ".%" PRIu8 ")\r\n", print_bytes_buff,
                          chip_id->prov_templ_ver[0], chip_id->prov_templ_ver[1])) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->prov_templ_tag, sizeof(chip_id->prov_templ_tag), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Prov template tag      = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->prov_spec_ver, sizeof(chip_id->prov_spec_ver), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Prov specification ver = 0x%s (v%" PRIu8 ".%" PRIu8 ")\r\n", print_bytes_buff,
                          chip_id->prov_spec_ver[0], chip_id->prov_spec_ver[1])) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->prov_spec_tag, sizeof(chip_id->prov_spec_tag), print_bytes_buff,
                              sizeof(print_bytes_buff))
        || 0 > print_func("Prov specification tag = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    if (LT_OK
            != lt_print_bytes(chip_id->batch_id, sizeof(chip_id->batch_id), print_bytes_buff, sizeof(print_bytes_buff))
        || 0 > print_func("Batch ID               = 0x%s\r\n", print_bytes_buff)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_do_mutable_fw_update(lt_handle_t *h, const uint8_t *update_data, const uint16_t update_data_size,
                                 const lt_bank_id_t bank_id)
{
#ifdef ABAB
    if (!h || !update_data || update_data_size > TR01_MUTABLE_FW_UPDATE_SIZE_MAX
        || ((bank_id != TR01_FW_BANK_FW1) && (bank_id != TR01_FW_BANK_FW2) && (bank_id != TR01_FW_BANK_SPECT1)
            && (bank_id != TR01_FW_BANK_SPECT2))) {
        return LT_PARAM_ERR;
    }
    lt_ret_t ret = lt_mutable_fw_erase(h, bank_id);
    if (ret != LT_OK) {
        return ret;
    }

    ret = lt_mutable_fw_update(h, update_data, update_data_size, bank_id);
    if (ret != LT_OK) {
        return ret;
    }

#elif ACAB
    LT_UNUSED(bank_id);  // bank_id is not used with ACAB, chip handles banks on its own
    if (!h || !update_data || update_data_size > TR01_MUTABLE_FW_UPDATE_SIZE_MAX) {
        return LT_PARAM_ERR;
    }

    // send the update 'request'
    lt_ret_t ret = lt_mutable_fw_update(h, update_data);
    if (ret != LT_OK) {
        return ret;
    }

    // send the rest - update 'data'
    ret = lt_mutable_fw_update_data(h, update_data, update_data_size);
    if (ret != LT_OK) {
        return ret;
    }

#else
#error "Undefined silicon revision. Please define either ABAB or ACAB."
#endif

    return LT_OK;
}

lt_ret_t lt_print_fw_header(lt_handle_t *h, const lt_bank_id_t bank_id, int (*print_func)(const char *format, ...))
{
    if (!h || !print_func) {
        return LT_PARAM_ERR;
    }

    uint8_t header[TR01_L2_GET_INFO_FW_HEADER_SIZE] = {0};
    uint16_t read_header_size;

    switch (bank_id) {
        case TR01_FW_BANK_FW1:
            print_func("    Reading header from Application's firmware bank 1:\r\n");
            break;
        case TR01_FW_BANK_FW2:
            print_func("    Reading header from Application's firmware bank 2:\r\n");
            break;
        case TR01_FW_BANK_SPECT1:
            print_func("    Reading header from SPECT's firmware bank 1:\r\n");
            break;
        case TR01_FW_BANK_SPECT2:
            print_func("    Reading header from SPECT's foirmware bank 2:\r\n");
            break;
        default:
            print_func("    Reading header: Unknown bank ID: %d\r\n", (int)bank_id);
            return LT_FAIL;
    }

    lt_ret_t ret = lt_get_info_fw_bank(h, bank_id, header, sizeof(header), &read_header_size);
    if (ret != LT_OK) {
        print_func("Failed to read FW header, error: %s\r\n", lt_ret_verbose(ret));
        return ret;
    }

    if (read_header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V1) {
        print_func("    Bootloader v1.0.1 detected, reading %dB header\r\n", TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V1);
        struct lt_header_boot_v1_t *p_h = (struct lt_header_boot_v1_t *)header;

        print_func("      Type:      %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "\r\n", p_h->type[3], p_h->type[2],
                   p_h->type[1], p_h->type[0]);
        print_func("      Version:   %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "\r\n", p_h->version[3],
                   p_h->version[2], p_h->version[1], p_h->version[0]);
        print_func("      Size:      %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "\r\n", p_h->size[3], p_h->size[2],
                   p_h->size[1], p_h->size[0]);
        print_func("      Git hash:  %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "\r\n", p_h->git_hash[3],
                   p_h->git_hash[2], p_h->git_hash[1], p_h->git_hash[0]);
        print_func("      FW hash:   %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "\r\n", p_h->hash[3], p_h->hash[2],
                   p_h->hash[1], p_h->hash[0]);
    }
    else if (read_header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2) {
        print_func("    Bootloader v2.0.1 detected, reading %dB header\r\n", TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2);
        struct lt_header_boot_v2_t *p_h = (struct lt_header_boot_v2_t *)header;

        print_func("      Type:               %04" PRIX16 "\r\n", p_h->type);
        print_func("      Padding:            %02" PRIX8 "\r\n", p_h->padding);
        print_func("      FW header version:  %02" PRIX8 "\r\n", p_h->header_version);
        print_func("      Version:            %08" PRIX32 "\r\n", p_h->ver);
        print_func("      Size:               %08" PRIX32 "\r\n", p_h->size);
        print_func("      Git hash:           %08" PRIX32 "\r\n", p_h->git_hash);
        // Hash str has 32B
        char hash_str[32 * 2 + 1] = {0};
        for (int i = 0; i < 32; i++) {
            snprintf(hash_str + i * 2, sizeof(hash_str) - i * 2, "%02" PRIX8, p_h->hash[i]);
        }
        print_func("      Hash:          %s\r\n", hash_str);
        print_func("      Pair version:  %08" PRIX32 "\r\n", p_h->pair_version);
    }
    else if (read_header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2_EMPTY_BANK) {
        print_func("    No firmware present in a given bank\r\n");
    }
    else {
        print_func("Unexpected header size %" PRIu16 "\r\n", read_header_size);
        return LT_FAIL;
    }

    return LT_OK;
}
#endif
