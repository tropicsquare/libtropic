/**
 * @file libtropic_example_1.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

// This header is only thing needed to work with libtropic
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "libtropic.h"

// Default initial Tropic handshake keys
#define PKEY_INDEX_BYTE   PAIRING_KEY_SLOT_INDEX_0
#define SHiPRIV_BYTES    {0xf0,0xc4,0xaa,0x04,0x8f,0x00,0x13,0xa0,0x96,0x84,0xdf,0x05,0xe8,0xa2,0x2e,0xf7,0x21,0x38,0x98,0x28,0x2b,0xa9,0x43,0x12,0xf3,0x13,0xdf,0x2d,0xce,0x8d,0x41,0x64};
#define SHiPUB_BYTES     {0x84,0x2f,0xe3,0x21,0xa8,0x24,0x74,0x08,0x37,0x37,0xff,0x2b,0x9b,0x88,0xa2,0xaf,0x42,0x44,0x2d,0xb0,0xd8,0xaa,0xcc,0x6d,0xc6,0x9e,0x99,0x53,0x33,0x44,0xb2,0x46};

int libtropic_example___verify_chip_and_start_secure_session(lt_handle_t *h)
{
    LOG_OUT("\r\n");
    LOG_OUT_INFO("List all possible return values:\r\n\n");
    for(int i=0; i<LT_L3_DATA_LEN_ERROR+1; i++) {
        LOG_OUT_VALUE("(lt_ret_t) %d: \t%s \r\n", i,   lt_ret_verbose(i));
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Initialize transport layer (l1):\r\n");
    /************************************************************************************************************/
    lt_ret_t ret = LT_FAIL;

    /* Example of a call: */
    ret = lt_init(h);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("lt_init();\r\n");
    LOG_OUT_VALUE("lt_ret_t:    %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01's CHIP ID\r\n");
    /************************************************************************************************************/
    uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};

    /* Example of a call: */
    ret = lt_get_info_chip_id(h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE);

    /* Following lines only print out some debug: */
    char chip_id_str[2*LT_L2_GET_INFO_CHIP_ID_SIZE+1] = {0};
    bytes_to_chars(chip_id, chip_id_str, LT_L2_GET_INFO_CHIP_ID_SIZE);
    LOG_OUT_INFO("lt_get_info_chip_id();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("CHIP ID:       %s\r\n", chip_id_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01's RISCV FW VERSION\r\n");
    /************************************************************************************************************/
    uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    /* Example of a call: */
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);

    /* Following lines only print out some debug: */
    char riscv_fw_ver_str[LT_L2_GET_INFO_RISCV_FW_SIZE+1] = {0};
    bytes_to_chars(riscv_fw_ver, riscv_fw_ver_str, LT_L2_GET_INFO_RISCV_FW_SIZE);
    LOG_OUT_INFO("lt_get_info_riscv_fw_ver();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("RISCV FW VER:       %s\r\n", riscv_fw_ver_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01 SPECT FW VERSION\r\n");
    /************************************************************************************************************/
    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};

    /* Example of a call: */
    ret = lt_get_info_spect_fw_ver(h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);

    /* Following lines only print out some debug: */
    char spect_fw_ver_str[2*LT_L2_GET_INFO_SPECT_FW_SIZE+1] = {0};
    bytes_to_chars(spect_fw_ver, spect_fw_ver_str, LT_L2_GET_INFO_SPECT_FW_SIZE);
    LOG_OUT_INFO("lt_get_info_spect_fw_ver();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("SPECT FW VER:       %s\r\n", spect_fw_ver_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01 info fw bank\r\n");
    /************************************************************************************************************/
    uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};

    /* Example of a call: */
    ret = lt_get_info_fw_bank(h, chip_id, LT_L2_GET_INFO_FW_HEADER_SIZE);

    /* Following lines only print out some debug: */
    char header_str[2*LT_L2_GET_INFO_FW_HEADER_SIZE+1] = {0};
    bytes_to_chars(header, header_str, LT_L2_GET_INFO_FW_HEADER_SIZE);
    LOG_OUT_INFO("lt_get_info_fw_bank();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("FW BANK INFO:       %s\r\n", chip_id);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01 device's certificate\r\n");
    /************************************************************************************************************/
    uint8_t X509_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};

    /* Example of a call: */
    ret = lt_get_info_cert(h, X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE);

    /* Following lines only print out some debug: */
    char X509_cert_str[1024+1] = {0};
    bytes_to_chars(X509_cert, X509_cert_str, LT_L2_GET_INFO_REQ_CERT_SIZE);
    LOG_OUT_INFO("lt_get_info_cert();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("X509 DER cert:       %s\r\n", X509_cert_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Verify and parse TROPIC01 device's certificate\r\n");
    /************************************************************************************************************/
    uint8_t stpub[32] = {0};

    /* Example of a call: */
    ret = lt_cert_verify_and_parse(X509_cert, 512, stpub);

    /* Following lines only print out some debug: */
    char stpub_str[64+1] = {0};
    bytes_to_chars(stpub, stpub_str, 32);
    LOG_OUT_INFO("lt_cert_verify_and_parse();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("STPUB:         %s\r\n", stpub_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Establish a session by doing a handshake with provided keys\r\n");
    /************************************************************************************************************/
    uint8_t pkey_index  = PKEY_INDEX_BYTE;
    uint8_t shipriv[]   = SHiPRIV_BYTES;
    uint8_t shipub[]    = SHiPUB_BYTES;

    /* Example of a call: */
    ret = lt_handshake(h, stpub, pkey_index, shipriv, shipub);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("lt_handshake();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }

    return 0;
}


int libtropic_example___other_commands(lt_handle_t *h)
{
    lt_ret_t ret = LT_FAIL;

    /************************************************************************************************************/
    LOG_OUT_INFO("Test session by sending a ping command:\r\n");
    /************************************************************************************************************/
    uint8_t msg_out[PING_LEN_MAX] = {0};
    uint8_t msg_in[PING_LEN_MAX]  = {0};
    uint16_t len_ping = PING_LEN_MAX;// Note: PING_LEN_MAX takes some time

    for(int i=0; i<127;i++) {
        memcpy(msg_out + (32*i), "XSome content for Ping message Y", 32);
    }
    memcpy(msg_out + (32*127), "X The end of for Ping message Y", 31);

    /* Example of a call: */
    ret = lt_ping(h, (uint8_t *)msg_out, (uint8_t *)msg_in, len_ping);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("lt_ping();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if(ret == LT_OK) {
        LOG_OUT_VALUE("ping length:   %d\r\n", len_ping);
        LOG_OUT_VALUE("msg compare:   %s\r\n", !memcmp(msg_out, msg_in, len_ping) ? "OK" : "ERROR");
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Read pairing key\r\n");
    /************************************************************************************************************/
    uint8_t pairing_key[32] = {0};

    /* Example of a call: */
    ret = lt_pairing_key_read(h, pairing_key, SH0PUB);

    /* Following lines only print out some debug: */
    char pairing_key_str[64+1] = {0};
    bytes_to_chars(pairing_key, pairing_key_str, 32);
    LOG_OUT_INFO("lt_pairing_key_read();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("pubkey:        %s\r\n", pairing_key_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



//    /************************************************************************************************************/
//    LOG_OUT_INFO("Invalidate pairing key\r\n");
//    /************************************************************************************************************/
//    /* Example of a call: */
//    ret = lt_pairing_key_invalidate(h, SH0PUB);
//
//    LOG_OUT_INFO("lt_pairing_key_read();\r\n");
//    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
//    if (ret == LT_OK) {
//        LOG_OUT_LINE();
//    } else {
//        ret = lt_deinit(h);
//        return 1;
//    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Write slot 0 sector in R MEMORY (444B)\r\n");
    /************************************************************************************************************/
    uint8_t data_to_write[R_MEM_DATA_SIZE_MAX] = {0};
    memset(data_to_write, 's', R_MEM_DATA_SIZE_MAX);

    /* Example of a call: */
    ret = lt_r_mem_data_write(h, 0, data_to_write, R_MEM_DATA_SIZE_MAX);

    LOG_OUT_INFO("lt_r_mem_data_write();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Read slot 0 sector from R MEMORY (444B)\r\n");
    /************************************************************************************************************/
    uint8_t data_to_read[R_MEM_DATA_SIZE_MAX] = {0};

    /* Example of a call: */
    ret = lt_r_mem_data_read(h, 0, data_to_read, R_MEM_DATA_SIZE_MAX);

    LOG_OUT_INFO("lt_r_mem_data_read();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("msg compare:   %s\r\n", !memcmp(data_to_read, data_to_write, R_MEM_DATA_SIZE_MAX) ? "OK" : "ERROR");
        char str_readed[2*R_MEM_DATA_SIZE_MAX+1];
        bytes_to_chars(data_to_read, str_readed, R_MEM_DATA_SIZE_MAX);
        char str_written[2*R_MEM_DATA_SIZE_MAX+1];
        bytes_to_chars(data_to_write, str_written, R_MEM_DATA_SIZE_MAX);
        LOG_OUT_VALUE("str_readed:         %s\r\n", str_readed);
        LOG_OUT_VALUE("str_written:         %s\r\n", str_written);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase slot 0 sector in R MEMORY (444B)\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_r_mem_data_erase(h, 0);

    LOG_OUT_INFO("lt_r_mem_data_erase();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Get a few random bytes from the chip\r\n");
    /************************************************************************************************************/
    uint8_t buff[RANDOM_VALUE_GET_LEN_MAX] = {0};
    uint16_t len_rand = RANDOM_VALUE_GET_LEN_MAX;//70;//L3_RANDOM_VALUE_GET_LEN_MAX;//rand() % L3_RANDOM_VALUE_GET_LEN_MAX;

    /* Example of a call: */
    ret = lt_random_get(h, buff, len_rand);

    /* Following lines only print out some debug: */
    char string[505] = {0};
    bytes_to_chars(buff, string, len_rand);
    LOG_OUT_INFO("lt_random_get();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("random length: %d\r\n", len_rand);
        LOG_OUT_VALUE("bytes:         %s\r\n", string);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_ecc_key_generate(h, ECC_SLOT_1, CURVE_ED25519);

    /* Following lines only print out some debug: */
    if (ret == LT_OK) {
        LOG_OUT_INFO("lt_ecc_key_generate();\r\n");
        LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    uint8_t key[64] = {0};
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    /* Example of a call: */
    ret = lt_ecc_key_read(h, ECC_SLOT_1, key, 64, &curve, &origin);

    /* Following lines only print out some debug: */
    uint8_t key_type = CURVE_ED25519;
    int n_of_bytes_in_key = (key_type == CURVE_ED25519 ? 32:64);
    char key_str[64+1] = {0};
    bytes_to_chars(key, key_str, n_of_bytes_in_key);
    LOG_OUT_INFO("lt_ecc_key_read();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("curve:         %s\r\n", (curve == CURVE_ED25519 ? "ED25519" : "P256"));
        LOG_OUT_VALUE("origin:        %s\r\n", (origin == 0x01 ? "Generated" : "Saved"));
        LOG_OUT_VALUE("pubkey:        %s\r\n", key_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("EdDSA Sign with L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    uint8_t msg[] = {'T','r','o','p','i','c',' ','S','q','u','a','r','e',' ','F','T','W','\0'};
    uint8_t rs[64] = {0};

    /* Example of a call: */
    ret = lt_ecc_eddsa_sign(h, ECC_SLOT_1, msg, 17, rs, 64);

    /* Following lines only print out some debug: */
    char R_str[64+1] = {0};
    char S_str[64+1] = {0};
    bytes_to_chars(rs, R_str, 32);
    bytes_to_chars(rs+32, S_str, 32);
    LOG_OUT_INFO("lt_ecc_eddsa_sign();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("msg:           %s\r\n", msg);
        LOG_OUT_VALUE("R:             %s\r\n", R_str);
        LOG_OUT_VALUE("S:             %s\r\n", S_str);
        LOG_OUT_INFO("Verify ED25519 signature on host side:\r\n");
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Verify EdDSA signature of previously signed message\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_ecc_eddsa_sig_verify(msg, 17, key, rs);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("lt_ecc_eddsa_sig_verify();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("Signature is CORRECT\r\n");
        LOG_OUT_LINE();
    } else {
        LOG_OUT_VALUE("Signature verification FAILED\r\n");
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_ecc_key_erase(h, ECC_SLOT_1);

    /* Following lines only print out some debug: */
    if (ret == LT_OK) {
        LOG_OUT_INFO("lt_ecc_key_erase();\r\n");
        LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_ecc_key_generate(h, ECC_SLOT_2, CURVE_P256);

    /* Following lines only print out some debug: */
    if (ret == LT_OK) {
        LOG_OUT_INFO("lt_ecc_key_generate();\r\n");
        LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    uint8_t key2[64] = {0};
    ecc_curve_type_t curve2;
    ecc_key_origin_t origin2;

    /* Example of a call: */
    ret = lt_ecc_key_read(h, ECC_SLOT_2, key2, 64, &curve2, &origin2);

    /* Following lines only print out some debug: */
    char key2_str[64+1] = {0};
    bytes_to_chars((uint8_t*)&key2, key2_str, 64);
    LOG_OUT_INFO("lt_ecc_key_read();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("curve:         %s\r\n", (curve2 == CURVE_ED25519 ? "ED25519" : "P256"));
        LOG_OUT_VALUE("origin:        %s\r\n", (origin2 == 0x01 ? "Generated" : "Saved"));
        LOG_OUT_VALUE("pubkey:        %s\r\n", key2_str);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("ECDSA Sign with L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    uint8_t rs2[64] = {0};
    char msg2[] = "Tropic Square FTW";

    /* Example of a call: */
    ret = lt_ecc_ecdsa_sign(h, ECC_SLOT_2, (uint8_t*)msg2, strlen(msg2), rs2, 64);

    /* Following lines only print out some debug: */
    char R_str2[64+1] = {0};
    char S_str2[64+1] = {0};
    bytes_to_chars(rs2, R_str2, 32);
    bytes_to_chars(rs2+32, S_str2, 32);
    LOG_OUT_INFO("lt_ecc_ecdsa_sign();\r\n");
    LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
    if (ret == LT_OK) {
        LOG_OUT_VALUE("msg:           %s\r\n", msg2);
        LOG_OUT_VALUE("R:             %s\r\n", R_str2);
        LOG_OUT_VALUE("S:             %s\r\n", S_str2);
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_ecc_key_erase(h, ECC_SLOT_2);

    /* Following lines only print out some debug: */
    if (ret == LT_OK) {
        LOG_OUT_INFO("lt_ecc_key_erase();\r\n");
        LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }



    /************************************************************************************************************/
    LOG_OUT_INFO("Denitialize transport layer (l1): :\r\n");
    /************************************************************************************************************/
    /* Example of a call: */
    ret = lt_deinit(h);

    /* Following lines only print out some debug: */
    if (ret == LT_OK) {
        LOG_OUT_INFO("lt_l1_deinit();\r\n");
        LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
        LOG_OUT_LINE();
    } else {
        ret = lt_deinit(h);
        return 1;
    }

    LOG_OUT("\r\n\n");

    return 0;
}

