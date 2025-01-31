

/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"

void lt_ex_test_reversible(void)
{
    LT_LOG("ireversible features\r\n");

    lt_handle_t h = {0};

    lt_init(&h);

    LT_LOG("%s", "lt_get_info_chip_id() ");
    uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
    LT_LOG_RESULT("%d", lt_get_info_chip_id(&h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE));

    LT_LOG("%s", "lt_get_info_riscv_fw_ver() ");
    uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    LT_LOG_RESULT("%d", lt_get_info_riscv_fw_ver(&h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE));

    LT_LOG("%s", "lt_get_info_spect_fw_ver() ");
    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};
    LT_LOG_RESULT("%d", lt_get_info_spect_fw_ver(&h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE));

    uint8_t X509_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
    LT_LOG("%s", "lt_get_info_cert() ");
    LT_LOG_RESULT("%d", lt_get_info_cert(&h, X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE));

    LT_LOG("%s", "lt_cert_verify_and_parse() ");
    uint8_t stpub[32] = {0};
    LT_LOG_RESULT("%d", lt_cert_verify_and_parse(X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub));

    LT_LOG("%s", "lt_session_start() ");
    LT_LOG_RESULT("%d", lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub));

    LT_LOG("%s", "lt_ping() ");
    uint8_t in[100];
    uint8_t out[100];
    LT_PRINT_RES(lt_ping(&h, out, in, 100));

    LT_LOG("---------------- Loop through all key slots, ed25519, stored key ----------------");

    for(uint16_t i=0; i<32; i++) {
        LT_LOG("lt_ecc_key_store() slot        %d  ", i);
        uint8_t attestation_key[32] = {0x22,0x57,0xa8,0x2f,0x85,0x8f,0x13,0x32,0xfa,0x0f,0xf6,0x0c,0x76,0x29,0x42,0x70,0xa9,0x58,0x9d,0xfd,0x47,0xa5,0x23,0x78,0x18,0x4d,0x2d,0x38,0xf0,0xa7,0xc4,0x01};
        LT_LOG_RESULT("%d", lt_ecc_key_store(&h, i, CURVE_ED25519, attestation_key));

        LT_LOG("lt_ecc_key_read() slot         %d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_ASSERT(lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin), LT_OK);

        LT_LOG("lt_ecc_eddsa_sign() slot       %d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%d", lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot %d  ", i);
        LT_LOG_RESULT("%d", lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs));

        LT_LOG("lt_ecc_key_erase() slot        %d  ", i);
        LT_LOG_RESULT("%d", lt_ecc_key_erase(&h, i));

        LT_LOG("");
    }

    LT_LOG("---------------- Loop through all key slots, ed25519, generated key ----------------");

    for(int i=0; i<32; i++) {
        LT_LOG("lt_ecc_key_generate() slot     %d  ", i);
        LT_LOG_RESULT("%d", lt_ecc_key_generate(&h, i, CURVE_ED25519));

        LT_LOG("lt_ecc_key_read() slot         %d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_LOG_RESULT("%d", lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin));

        LT_LOG("lt_ecc_eddsa_sign() slot       %d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%d", lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot %d  ", i);
        LT_LOG_RESULT("%d", lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs));

        LT_LOG("lt_ecc_key_erase() slot        %d  ", i);
        LT_LOG_RESULT("%d", lt_ecc_key_erase(&h, i));

        LT_LOG("");
    }

    LT_LOG("lt_random_get() RANDOM_VALUE_GET_LEN_MAX == 255 ");
    uint8_t buff[RANDOM_VALUE_GET_LEN_MAX];
    LT_LOG_RESULT("%d", lt_random_get(&h, buff, RANDOM_VALUE_GET_LEN_MAX));

    LT_LOG("lt_session_abort()");
    LT_LOG_RESULT("%d", lt_session_abort(&h));

    LT_LOG("lt_deinit()");
    LT_LOG_RESULT("%d", lt_deinit(&h));

}
