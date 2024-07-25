#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// This header is only thing needed to work with libtropic
#include "libtropic.h"

// Some crypto is practical to have here, for example when application wants to hash a message or verify a signature
#include "ed25519.h"
#include "ts_sha256.h"

// Default initial Tropic handshake keys
#define PKEY_INDEX_BYTE   TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_0
#define SHiPRIV_BYTES    {0xf0,0xc4,0xaa,0x04,0x8f,0x00,0x13,0xa0,0x96,0x84,0xdf,0x05,0xe8,0xa2,0x2e,0xf7,0x21,0x38,0x98,0x28,0x2b,0xa9,0x43,0x12,0xf3,0x13,0xdf,0x2d,0xce,0x8d,0x41,0x64};
#define SHiPUB_BYTES     {0x84,0x2f,0xe3,0x21,0xa8,0x24,0x74,0x08,0x37,0x37,0xff,0x2b,0x9b,0x88,0xa2,0xaf,0x42,0x44,0x2d,0xb0,0xd8,0xaa,0xcc,0x6d,0xc6,0x9e,0x99,0x53,0x33,0x44,0xb2,0x46};


#define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#define LOG_OUT_INFO(f_, ...) printf("\t[INFO] "f_, ##__VA_ARGS__)
#define LOG_OUT_VALUE(f_, ...) printf("\t\t\t\t\t"f_, ##__VA_ARGS__)
#define LOG_OUT_LINE(f_, ...) printf("\t-------------------------------------------------------------------------------------------------------------\r\n"f_, ##__VA_ARGS__)

void bytes_to_chars(uint8_t const *key, char *buffer, uint16_t len)
{
    uint16_t offset = 0;
    memset(buffer, 0, len);

    for (size_t i = 0; i < len; i++)
    {
        offset += sprintf(buffer + offset, "%02X", key[i]);
    }
    sprintf(buffer + offset, "%c", '\0');
}


int main(void)
{
    /************************************************************************************************************/
    LOG_OUT("\r\n");
    LOG_OUT("\t\t======================================================================\r\n");
    LOG_OUT("\t\t=====  libtropic Unix demo, running over TCP against model         ===\r\n");
    LOG_OUT("\t\t======================================================================\r\n\n");
    LOG_OUT_INFO("List all possible return values:\r\n\n");
    for(int i=0; i<TS_L2_UNKNOWN_REQ+1; i++) {
        LOG_OUT_VALUE("(ts_ret_t) %d: \t%s \r\n", i,   ts_ret_verbose(i));
    }
    LOG_OUT("\r\n");
    /************************************************************************************************************/



    /************************************************************************************************************/
    LOG_OUT_INFO("Initialize transport layer (l1):\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ts_handle_t handle = {0};
    ts_ret_t ret = TS_FAIL;

    ret = ts_init(&handle);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_l1_init();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01 device's certificate\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t X509_cert[TS_L2_GET_INFO_REQ_CERT_SIZE] = {0};

    ret = ts_get_info_cert(&handle, X509_cert, TS_L2_GET_INFO_REQ_CERT_SIZE);

    /* Following lines only print out some debug: */
    char X509_cert_str[1024+1] = {0};
    bytes_to_chars(X509_cert, X509_cert_str, TS_L2_GET_INFO_REQ_CERT_SIZE);
    LOG_OUT_INFO("ts_get_info_cert();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("X509 DER cert:       %s\r\n", X509_cert_str);
        //printf("\r\nCert debug:\r\n");
        //for (int x = 0; x<16;x++) {
        //  for(int i=0;i<32; i++) {
        //    printf("0x%02X,", X509_cert[i+32*x]);
        //  }
        //  printf("\r\n");
        //}
        //printf("\r\nEnd of cert debug\r\n");
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Verify and parse TROPIC01 device's certificate\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t stpub[32] = {0};
    ret = ts_cert_verify_and_parse(X509_cert, 512, stpub);

    /* Following lines only print out some debug: */
    char stpub_str[64+1] = {0};
    bytes_to_chars(stpub, stpub_str, 32);
    LOG_OUT_INFO("ts_cert_verify_and_parse();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("STPUB:         %s\r\n", stpub_str);
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Establish a session by doing a handshake with provided keys\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t pkey_index  = PKEY_INDEX_BYTE;
    uint8_t shipriv[]   = SHiPRIV_BYTES;
    uint8_t shipub[]    = SHiPUB_BYTES;

    ret = ts_handshake(&handle, stpub, pkey_index, shipriv, shipub);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_handshake();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Test session by sending a ping command:\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t msg_out[PING_LEN_MAX] = {0};
    uint8_t msg_in[PING_LEN_MAX]  = {0};
    uint16_t len_ping = PING_LEN_MAX;// rand() % PING_LEN_MAX;

    for(int i=0; i<127;i++) {
        memcpy(msg_out + (32*i), "XSome content for Ping message Y", 32);
    }
    memcpy(msg_out + (32*127), "X The end of for Ping message Y", 31);

    ret = ts_ping(&handle, (uint8_t *)msg_out, (uint8_t *)msg_in, len_ping);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_ping();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if(ret == TS_OK) {
        LOG_OUT_VALUE("ping length:   %d\r\n", len_ping);
        LOG_OUT_VALUE("msg compare: %s\r\n", !memcmp(msg_out, msg_in, len_ping) ? "OK" : "ERROR");
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Get a few random bytes from the chip\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t buff[RANDOM_VALUE_GET_LEN_MAX] = {0};
    uint16_t len_rand = 70;//RANDOM_VALUE_GET_LEN_MAX;//rand() % RANDOM_VALUE_GET_LEN_MAX;

    ret = ts_random_get(&handle, buff, len_rand);

    /* Following lines only print out some debug: */
    char string[505] = {0};
    bytes_to_chars(buff, string, len_rand);
    LOG_OUT_INFO("ts_random_get();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("random length: %d\r\n", len_rand);
        LOG_OUT_VALUE("bytes:         %s\r\n", string);
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_generate(&handle, ECC_SLOT_1, TS_L3_ECC_KEY_GENERATE_CURVE_ED25519);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_ecc_key_generate();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t key[64] = {0};
    uint8_t curve, origin;

    ret = ts_ecc_key_read(&handle, ECC_SLOT_1, key, 64, &curve, &origin);

    /* Following lines only print out some debug: */
    uint8_t key_type = TS_L3_ECC_KEY_GENERATE_CURVE_ED25519;
    int n_of_bytes_in_key = (key_type == TS_L3_ECC_KEY_GENERATE_CURVE_ED25519 ? 32:64);
    char key_str[64] = {0};
    bytes_to_chars(key, key_str, n_of_bytes_in_key);
    LOG_OUT_INFO("ts_ecc_key_read();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("curve:         %s\r\n", (curve == TS_L3_ECC_KEY_GENERATE_CURVE_ED25519 ? "ED25519" : "P256"));
        LOG_OUT_VALUE("origin:        %s\r\n", (origin == 0x01 ? "Generated" : "Saved"));
        LOG_OUT_VALUE("pubkey:        %s\r\n", key_str);
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("EDDSA Sign with L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t msg[] = {'T','r','o','p','i','c',' ','S','q','u','a','r','e',' ','F','T','W','\0'};
    uint8_t rs[64];
    ret = ts_eddsa_sign(&handle, ECC_SLOT_1, msg, 17, rs, 64);

    /* Following lines only print out some debug: */
    char R_str[64+1] = {0};
    char S_str[64+1] = {0};
    bytes_to_chars(rs, R_str, 32);
    bytes_to_chars(rs+32, S_str, 32);
    LOG_OUT_INFO("ts_eddsa_sign();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("msg:           %s\r\n", msg);
        LOG_OUT_VALUE("R:             %s\r\n", R_str);
        LOG_OUT_VALUE("S:             %s\r\n", S_str);
        LOG_OUT_INFO("Verify ED25519 signature on host side:\r\n");
        int verified = ed25519_sign_open(msg, 17, key, rs);
        LOG_OUT_VALUE("Signature is %s\r\n", verified == 0 ? "CORRECT" : "WRONG");
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_erase(&handle, ECC_SLOT_1);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_ecc_key_erase();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

        ret = ts_ecc_key_generate(&handle, ECC_SLOT_2, TS_L3_ECC_KEY_GENERATE_CURVE_P256);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_ecc_key_generate();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t key2[64] = {0};
    uint8_t curve2, origin2;

    ret = ts_ecc_key_read(&handle, ECC_SLOT_2, key2, 64, &curve2, &origin2);

    /* Following lines only print out some debug: */
    char key2_str[64+1] = {0};
    bytes_to_chars((uint8_t*)&key2, key2_str, 64);
    LOG_OUT_INFO("ts_ecc_key_read();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("curve:         %s\r\n", (curve2 == TS_L3_ECC_KEY_GENERATE_CURVE_ED25519 ? "ED25519" : "P256"));
        LOG_OUT_VALUE("origin:        %s\r\n", (origin2 == 0x01 ? "Generated" : "Saved"));
        LOG_OUT_VALUE("pubkey:        %s\r\n", key2_str);
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("ECDSA Sign with L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t rs2[64];
    char ecdsa_msg[] = "Tropic Square FTW -- 32B message";
    uint8_t msg_hash[32];

    ts_sha256_ctx_t hctx = {0};
    ts_sha256_init(&hctx);
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, (uint8_t*)ecdsa_msg, 32);
    ts_sha256_finish(&hctx, msg_hash);

    ret = ts_ecdsa_sign(&handle, ECC_SLOT_2, msg_hash, 32, rs2, 64);


    /* Following lines only print out some debug: */
    char R_str2[64+1] = {0};
    char S_str2[64+1] = {0};
    char msg_hash_str[64+1] = {0};
    bytes_to_chars(rs2, R_str2, 32);
    bytes_to_chars(rs2+32, S_str2, 32);
    bytes_to_chars(msg_hash, msg_hash_str, 32);
    LOG_OUT_INFO("ts_eddsa_sign();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    if (ret == TS_OK) {
        LOG_OUT_VALUE("msg:           %s\r\n", ecdsa_msg);
        LOG_OUT_VALUE("MSG_HASH:      %s\r\n", msg_hash_str);
        LOG_OUT_VALUE("R:             %s\r\n", R_str2);
        LOG_OUT_VALUE("S:             %s\r\n", S_str2);
    }
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_erase(&handle, ECC_SLOT_2);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_ecc_key_erase();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Denitialize transport layer (l1): :\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_deinit(&handle);

    /* Following lines only print out some debug: */
    LOG_OUT_INFO("ts_l1_deinit();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();

    LOG_OUT("\r\n\n");

    return 0;
}
