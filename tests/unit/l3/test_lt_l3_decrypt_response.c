
/**
 * @file test_lt_l3_process.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_l3_process.h"
#include "mock_lt_aesgcm.h"
#include "mock_lt_l2.h"
#include "string.h"
#include "unity.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void) {}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid handle
void test__handle_invalid()
{
    int ret = lt_l3_decrypt_response(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if lt_l3_cmd() fails when secure session is not active
void test__fail_when_no_session()
{
    lt_handle_t handle = {0};

    int ret = lt_l3_decrypt_response(&handle.l3);
    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, ret);
}

// Test if lt_l3_cmd() returns LT_FAIL, if some error happens during aesgcm encrypt operations
void test__lt_aesgcm_encrypt__LT_FAIL()
{
    lt_handle_t h = {0};
    h.l3.session = LT_SECURE_SESSION_ON;

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)&h.l3.buff;
    lt_aesgcm_decrypt_ExpectAndReturn(&h.l3.encrypt, h.l3.encryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0,
                                      p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size,
                                      TR01_L3_TAG_SIZE, LT_FAIL);

    int ret = lt_l3_decrypt_response(&h.l3);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test that all possible L3_IDs are correctly parsed in lt_l3_decrypt_response() function and that they are propagated
// to upper layers
void test__test_all_l3_results()
{
    // Note: correct execution is tested at the end
    uint8_t results[] = {TR01_L3_RESULT_FAIL,
                         TR01_L3_RESULT_UNAUTHORIZED,
                         TR01_L3_RESULT_INVALID_CMD,
                         TR01_L3_PAIRING_KEY_EMPTY,
                         TR01_L3_PAIRING_KEY_INVALID,
                         TR01_L3_ECC_INVALID_KEY,
                         0x99,
                         TR01_L3_RESULT_OK,
                         TR01_L3_R_MEM_DATA_WRITE_WRITE_FAIL,
                         TR01_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED};
    uint8_t returned[] = {LT_L3_FAIL,
                          LT_L3_UNAUTHORIZED,
                          LT_L3_INVALID_CMD,
                          LT_L3_PAIRING_KEY_EMPTY,
                          LT_L3_PAIRING_KEY_INVALID,
                          LT_L3_ECC_INVALID_KEY,
                          LT_FAIL,
                          LT_OK,
                          LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL,
                          LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED};

    for (int i = 0; i < 10; i++) {
        lt_handle_t h = {0};
        h.l3.session = LT_SECURE_SESSION_ON;

        struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)&h.l3.buff;
        lt_aesgcm_decrypt_ExpectAndReturn(&h.l3.decrypt, h.l3.decryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0,
                                          p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size,
                                          TR01_L3_TAG_SIZE, LT_OK);
        p_frame->data[0] = results[i];

        int ret = lt_l3_decrypt_response(&h.l3);
        TEST_ASSERT_EQUAL(returned[i], ret);
    }
}
