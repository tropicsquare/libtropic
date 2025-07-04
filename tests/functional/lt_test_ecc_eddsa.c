/**
 * @file lt_test_ecc_eddsa.c
 * @brief Test function to test all ECC EdDSA operations on all key slots
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

uint8_t attestation_key[32]
    = {0x22, 0x57, 0xa8, 0x2f, 0x85, 0x8f, 0x13, 0x32, 0xfa, 0x0f, 0xf6, 0x0c, 0x76, 0x29, 0x42, 0x70,
       0xa9, 0x58, 0x9d, 0xfd, 0x47, 0xa5, 0x23, 0x78, 0x18, 0x4d, 0x2d, 0x38, 0xf0, 0xa7, 0xc4, 0x01};

/**
 * @brief
 *
 * @return int
 */
int lt_test_ecc_eddsa(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_ecc_eddsa() "
        "--------------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    // Ping with SH0
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    // Erase all keys, used in case previous run failed in the middle
    // for(uint16_t i=0; i<32; i++) {
    //    LT_LOG("lt_ecc_key_erase() slot        n.%d  ", i);
    //    LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_erase(&h, i)));
    //}

    LT_LOG("---------------- Loop through all key slots, ed25519, stored key ----------------");

    for (uint16_t i = 0; i < 32; i++) {
        LT_LOG("lt_ecc_key_store() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_store(&h, i, CURVE_ED25519, attestation_key)));

        LT_LOG("lt_ecc_key_read() slot         n.%d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_ASSERT(lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin), LT_OK);

        LT_LOG("lt_ecc_eddsa_sign() slot       n.%d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64)));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs)));

        LT_LOG("lt_ecc_key_erase() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_erase(&h, i)));

        LT_LOG("");
    }

    LT_LOG("---------------- Loop through all key slots, ed25519, generated key ----------------");

    for (int i = 0; i < 32; i++) {
        LT_LOG("lt_ecc_key_generate() slot     n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_generate(&h, i, CURVE_ED25519)));

        LT_LOG("lt_ecc_key_read() slot         n.%d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin)));

        LT_LOG("lt_ecc_eddsa_sign() slot       n.%d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64)));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs)));

        LT_LOG("lt_ecc_key_erase() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_erase(&h, i)));

        LT_LOG("");
    }

    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
