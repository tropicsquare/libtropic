
/**
 * @file test_lt_l3_process.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "lt_l3_process.h"

#include "mock_lt_l2.h"
#include "mock_lt_aesgcm.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void)
{
}

void tearDown(void)
{
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid handle
//void test__handle_invalid()
//{
//    int ret = lt_l3_cmd(NULL);
//    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
//}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if lt_l3_cmd() fails when secure session is not active
//void test__fail_when_no_session()
//{
//    lt_handle_t handle = {0};
//
//    int ret = lt_l3_cmd(&handle);
//    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, ret);
//}
//
// Test if lt_l3_cmd() returns LT_FAIL, if some error happens during aesgcm encrypt operations
//void test__lt_aesgcm_encrypt__LT_FAIL()
//{
//    lt_handle_t h = {0};
//    h.session = SESSION_ON;
//
//    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)&h.l3_buff;
//    lt_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_FAIL);
//    int ret = lt_l3_cmd(&h);
//    TEST_ASSERT_EQUAL(LT_FAIL, ret);
//}
/*
// Test if lt_l3_cmd() returns LT_FAIL, if some error happens during l2 exchange
void test__lt_l2_encrypted_cmd__LT_FAIL()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)&h.l3_buff;
    lt_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_OK);
    lt_l2_encrypted_cmd_ExpectAndReturn(&h, LT_FAIL);
    int ret = lt_l3_cmd(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if lt_l3_cmd() returns LT_FAIL, if some error happens during aesgcm decrypt operations
void test__lt_aesgcm_decrypt__LT_FAIL()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)&h.l3_buff;
    lt_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_OK);
    lt_l2_encrypted_cmd_ExpectAndReturn(&h, LT_OK);
    lt_aesgcm_decrypt_ExpectAndReturn(&h.decrypt, h.decryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_FAIL);
    int ret = lt_l3_cmd(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test that all possible L3_IDs are correctly parsed in lt_l3_cmd() function and that they are propagated to upper layers
void test__test_all_l3_results()
{
    // Note: correct execution is tested at the end
    uint8_t results[] = {L3_RESULT_FAIL, L3_RESULT_UNAUTHORIZED, L3_RESULT_INVALID_CMD, 0x99, L3_RESULT_OK};
    uint8_t returned[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL, LT_OK};

    for (int i=0; i<5;i++) {
        lt_handle_t h = {0};
        h.session = SESSION_ON;

        struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)&h.l3_buff;
        lt_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.encryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_OK);
        lt_l2_encrypted_cmd_ExpectAndReturn(&h, LT_OK);
        lt_aesgcm_decrypt_ExpectAndReturn(&h.decrypt, h.decryption_IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size, L3_TAG_SIZE,LT_OK);
        p_frame->data[0] = results[i];

        int ret = lt_l3_cmd(&h);
        TEST_ASSERT_EQUAL(returned[i], ret);
    }
}
*/