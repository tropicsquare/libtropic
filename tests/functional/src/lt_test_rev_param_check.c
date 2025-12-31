/**
 * @file lt_test_rev_param_check.c
 * @brief Test parameter checking in API functions.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_l2_api_structs.h"
#include "lt_l3_api_structs.h"
#include "lt_test_common.h"

void lt_test_rev_param_check(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_param_check()");
    LT_LOG_INFO("----------------------------------------------");

    // --------------------------------------------------------
    // Common functions
    // --------------------------------------------------------

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_init(NULL));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_deinit(NULL));

    {
        lt_tr01_mode_t dummy_mode;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_tr01_mode(NULL, &dummy_mode));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_tr01_mode(h, NULL));
    }

    {
        lt_cert_store_t dummy_store = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_cert_store(NULL, &dummy_store));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_cert_store(h, NULL));
    }

    {
        lt_cert_store_t dummy_store = {0};
        uint8_t dummy_stpub[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_st_pub(NULL, dummy_stpub));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_st_pub(&dummy_store, NULL));
    }

    {
        lt_chip_id_t dummy_chip_id = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_chip_id(NULL, &dummy_chip_id));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_chip_id(h, NULL));
    }

    {
        uint8_t dummy_ver[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_riscv_fw_ver(NULL, dummy_ver));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_riscv_fw_ver(h, NULL));
    }

    {
        uint8_t dummy_ver[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_spect_fw_ver(NULL, dummy_ver));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_spect_fw_ver(h, NULL));
    }

    {
        uint8_t dummy_header[1];
        uint16_t dummy_size = 0;

        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_get_info_fw_bank(NULL, TR01_FW_BANK_FW1, dummy_header, sizeof(dummy_header), &dummy_size));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_fw_bank(h, 0xFF, dummy_header, sizeof(dummy_header), &dummy_size));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_info_fw_bank(h, TR01_FW_BANK_FW1, NULL, sizeof(dummy_header), &dummy_size));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_get_info_fw_bank(h, TR01_FW_BANK_FW1, dummy_header, sizeof(dummy_header), NULL));
    }

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_session_start(NULL, (const uint8_t *)1, TR01_PAIRING_KEY_SLOT_INDEX_0,
                                                  (const uint8_t *)1, (const uint8_t *)1));

    LT_TEST_ASSERT(LT_PARAM_ERR,
                   lt_session_start(h, NULL, TR01_PAIRING_KEY_SLOT_INDEX_0, (const uint8_t *)1, (const uint8_t *)1));

    LT_TEST_ASSERT(LT_PARAM_ERR,
                   lt_session_start(h, (const uint8_t *)1, (lt_pkey_index_t)(TR01_PAIRING_KEY_SLOT_INDEX_3 + 1),
                                    (const uint8_t *)1, (const uint8_t *)1));

    LT_TEST_ASSERT(LT_PARAM_ERR,
                   lt_session_start(h, (const uint8_t *)1, TR01_PAIRING_KEY_SLOT_INDEX_0, NULL, (const uint8_t *)1));

    LT_TEST_ASSERT(LT_PARAM_ERR,
                   lt_session_start(h, (const uint8_t *)1, TR01_PAIRING_KEY_SLOT_INDEX_0, (const uint8_t *)1, NULL));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_session_abort(NULL));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_sleep(NULL, TR01_L2_SLEEP_KIND_SLEEP));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_sleep(h, 0));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_reboot(NULL, TR01_REBOOT));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_reboot(h, (lt_startup_id_t)0xFF));

    {
        uint8_t buf[TR01_GET_LOG_MAX_MSG_LEN];
        uint16_t rd = 0;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_log_req(NULL, buf, sizeof(buf), &rd));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_log_req(h, NULL, sizeof(buf), &rd));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_get_log_req(h, buf, sizeof(buf), NULL));
    }

    {
        uint8_t out[1], in[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ping(NULL, out, in, 1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ping(h, NULL, in, 1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ping(h, out, NULL, 1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ping(h, out, in, (uint16_t)(TR01_PING_LEN_MAX + 1)));
    }

    {
        uint8_t pub[TR01_SHIPUB_LEN];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_write(NULL, pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_write(h, NULL, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_write(h, pub, TR01_PAIRING_KEY_SLOT_INDEX_3 + 1));
    }

    {
        uint8_t pub[TR01_SHIPUB_LEN];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_read(NULL, pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_read(h, NULL, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_read(h, pub, TR01_PAIRING_KEY_SLOT_INDEX_3 + 1));
    }

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_invalidate(NULL, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_pairing_key_invalidate(h, TR01_PAIRING_KEY_SLOT_INDEX_3 + 1));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_config_write(NULL, TR01_CFG_START_UP_ADDR, 0));

    {
        uint32_t obj;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_config_read(NULL, TR01_CFG_START_UP_ADDR, &obj));
    }
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_config_read(h, TR01_CFG_START_UP_ADDR, NULL));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_config_erase(NULL));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_i_config_write(NULL, TR01_CFG_START_UP_ADDR, 0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_i_config_write(h, TR01_CFG_START_UP_ADDR, 32));

    {
        uint32_t obj;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_i_config_read(NULL, TR01_CFG_START_UP_ADDR, &obj));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_i_config_read(h, TR01_CFG_START_UP_ADDR, NULL));
    }

    {
        uint8_t data[TR01_R_MEM_DATA_SIZE_MIN];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_write(NULL, 0, data, sizeof(data)));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_r_mem_data_write(h, (uint16_t)(TR01_R_MEM_DATA_SLOT_MAX + 1), data, sizeof(data)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_write(h, 0, NULL, sizeof(data)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_write(h, 0, data, 0));

        uint16_t data_read_size;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_read(NULL, 0, data, sizeof(data), &data_read_size));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_read(h, (uint16_t)(TR01_R_MEM_DATA_SLOT_MAX + 1), data, sizeof(data),
                                                        &data_read_size));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_read(h, 0, NULL, sizeof(data), &data_read_size));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_read(h, 0, data, sizeof(data), NULL));

        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_erase(NULL, 0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_r_mem_data_erase(h, (uint16_t)(TR01_R_MEM_DATA_SLOT_MAX + 1)));
    }

    {
        uint8_t buf[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_random_value_get(NULL, buf, sizeof(buf)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_random_value_get(h, NULL, sizeof(buf)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_random_value_get(h, buf, (uint16_t)(TR01_RANDOM_VALUE_GET_LEN_MAX + 1)));
    }

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_generate(NULL, TR01_ECC_SLOT_0, TR01_CURVE_ED25519));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_generate(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1), TR01_CURVE_ED25519));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_generate(h, TR01_ECC_SLOT_0, 0xFF));

    {
        uint8_t key[32];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_store(NULL, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, key));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_ecc_key_store(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1), TR01_CURVE_ED25519, key));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_store(h, TR01_ECC_SLOT_0, (lt_ecc_curve_type_t)0xFF, key));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, NULL));
    }

    {
        uint8_t key[64];
        lt_ecc_curve_type_t curve;
        lt_ecc_key_origin_t origin;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_read(NULL, TR01_ECC_SLOT_0, key, sizeof(key), &curve, &origin));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_ecc_key_read(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1), key, sizeof(key), &curve, &origin));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_read(h, TR01_ECC_SLOT_0, NULL, sizeof(key), &curve, &origin));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_read(h, TR01_ECC_SLOT_0, key, sizeof(key), NULL, &origin));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_read(h, TR01_ECC_SLOT_0, key, sizeof(key), &curve, NULL));
    }

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_erase(NULL, TR01_ECC_SLOT_0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_key_erase(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1)));

    {
        uint8_t msg[1], sig[64];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_ecdsa_sign(NULL, TR01_ECC_SLOT_0, msg, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_ecc_ecdsa_sign(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1), msg, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_ecdsa_sign(h, TR01_ECC_SLOT_0, NULL, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_ecdsa_sign(h, TR01_ECC_SLOT_0, msg, sizeof(msg), NULL));
    }

    {
        uint8_t msg[1], sig[64];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_eddsa_sign(NULL, TR01_ECC_SLOT_0, msg, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_ecc_eddsa_sign(h, (lt_ecc_slot_t)(TR01_ECC_SLOT_31 + 1), msg, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_eddsa_sign(h, TR01_ECC_SLOT_0, NULL, sizeof(msg), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_eddsa_sign(h, TR01_ECC_SLOT_0, msg,
                                                       (uint16_t)(TR01_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX + 1), sig));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_ecc_eddsa_sign(h, TR01_ECC_SLOT_0, msg, sizeof(msg), NULL));
    }

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_init(NULL, TR01_MCOUNTER_INDEX_0, 0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_init(h, (enum lt_mcounter_index_t)(TR01_MCOUNTER_INDEX_15 + 1), 0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_init(h, TR01_MCOUNTER_INDEX_0, TR01_MCOUNTER_VALUE_MAX + 1));

    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_update(NULL, TR01_MCOUNTER_INDEX_0));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_update(h, (enum lt_mcounter_index_t)(TR01_MCOUNTER_INDEX_15 + 1)));

    {
        uint32_t mv;
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_get(NULL, TR01_MCOUNTER_INDEX_0, &mv));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_get(h, (enum lt_mcounter_index_t)(TR01_MCOUNTER_INDEX_15 + 1), &mv));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mcounter_get(h, TR01_MCOUNTER_INDEX_0, NULL));
    }

    {
        uint8_t out[1], in[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mac_and_destroy(NULL, TR01_MAC_AND_DESTROY_SLOT_0, out, in));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_mac_and_destroy(h, (lt_mac_and_destroy_slot_t)(TR01_MAC_AND_DESTROY_SLOT_127 + 1), out, in));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mac_and_destroy(h, TR01_MAC_AND_DESTROY_SLOT_0, NULL, in));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mac_and_destroy(h, TR01_MAC_AND_DESTROY_SLOT_0, out, NULL));
    }

    LT_TEST_ASSERT(0, strcmp(lt_ret_verbose(LT_RET_T_LAST_VALUE), "FATAL ERROR, unknown return value"));

    // --------------------------------------------------------
    // Silicon revision specific functions
    // --------------------------------------------------------
#ifdef ABAB
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_erase(NULL, TR01_FW_BANK_FW1));
    LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_erase(h, 0xFFFFFFFF));

    {
        uint8_t dummy_data[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update(NULL, dummy_data, sizeof(dummy_data), TR01_FW_BANK_FW1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update(h, NULL, sizeof(dummy_data), TR01_FW_BANK_FW1));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_mutable_fw_update(h, dummy_data, TR01_MUTABLE_FW_UPDATE_SIZE_MAX + 1, TR01_FW_BANK_FW1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update(h, dummy_data, sizeof(dummy_data), 0xFFFFFFFF));
    }
#elif ACAB
    {
        uint8_t dummy_data[1];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update(NULL, dummy_data));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update(h, NULL));

        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update_data(NULL, dummy_data, TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update_data(h, NULL, TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_mutable_fw_update_data(h, dummy_data, TR01_MUTABLE_FW_UPDATE_SIZE_MAX + 1));
    }
#else
#warning "Unknown silicon revision, no revision specific parameter checks implemented!"
#endif

    // --------------------------------------------------------
    // LT_HELPERS
    // --------------------------------------------------------
#ifdef LT_HELPERS
    {
        struct lt_config_t cfg = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_read_whole_R_config(NULL, &cfg));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_read_whole_R_config(h, NULL));

        LT_TEST_ASSERT(LT_PARAM_ERR, lt_write_whole_R_config(NULL, &cfg));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_write_whole_R_config(h, NULL));

        LT_TEST_ASSERT(LT_PARAM_ERR, lt_read_whole_I_config(NULL, &cfg));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_read_whole_I_config(h, NULL));

        LT_TEST_ASSERT(LT_PARAM_ERR, lt_write_whole_I_config(NULL, &cfg));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_write_whole_I_config(h, NULL));
    }

    {
        uint8_t shipriv[TR01_SHIPRIV_LEN] = {0};
        uint8_t shipub[TR01_SHIPUB_LEN] = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_verify_chip_and_start_secure_session(NULL, shipriv, shipub, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_verify_chip_and_start_secure_session(h, NULL, shipub, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR,
                       lt_verify_chip_and_start_secure_session(h, shipriv, NULL, TR01_PAIRING_KEY_SLOT_INDEX_0));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_verify_chip_and_start_secure_session(
                                         h, shipriv, shipub, (lt_pkey_index_t)(TR01_PAIRING_KEY_SLOT_INDEX_3 + 1)));
    }

    {
        char out_small[1];
        uint8_t bb[1] = {0};
        char out_ok[3];
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_bytes(NULL, 0, out_ok, sizeof(out_ok)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_bytes(bb, sizeof(bb), NULL, sizeof(out_ok)));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_bytes(bb, sizeof(bb), out_small, sizeof(out_small)));
    }

    {
        lt_chip_id_t chip_id = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_chip_id(NULL, lt_port_log));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_chip_id(&chip_id, NULL));
    }

    {
        uint8_t data[1] = {0};
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_do_mutable_fw_update(NULL, data, sizeof(data), TR01_FW_BANK_FW1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_do_mutable_fw_update(h, NULL, sizeof(data), TR01_FW_BANK_FW1));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_do_mutable_fw_update(h, data, (uint16_t)(TR01_MUTABLE_FW_UPDATE_SIZE_MAX + 1),
                                                             TR01_FW_BANK_FW1));
    }

    {
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_fw_header(NULL, TR01_FW_BANK_FW1, lt_port_log));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_fw_header(h, 0xFFFFFFFF, lt_port_log));
        LT_TEST_ASSERT(LT_PARAM_ERR, lt_print_fw_header(h, TR01_FW_BANK_FW1, NULL));
    }
#endif
}