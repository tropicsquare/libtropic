
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
    int ret = lt_l3_encrypt_request(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if lt_l3_cmd() fails when secure session is not active
void test__fail_when_no_session()
{
    lt_handle_t h = {0};

    int ret = lt_l3_encrypt_request(&h.l3);
    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, ret);
}

// Test if lt_l3_cmd() returns LT_FAIL, if some error happens during aesgcm encrypt operations
void test__lt_aesgcm_encrypt__LT_FAIL()
{
    lt_handle_t h = {0};
    h.l3.session = LT_SECURE_SESSION_ON;

    struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)&h.l3.buff;
    lt_aesgcm_encrypt_ExpectAndReturn(&h.l3.encrypt, h.l3.encryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0,
                                      p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size,
                                      TR01_L3_TAG_SIZE, LT_FAIL);

    int ret = lt_l3_encrypt_request(&h.l3);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when executed correctly
void test__correct()
{
    for (int i = 0; i < 5; i++) {
        lt_handle_t h = {0};
        h.l3.session = LT_SECURE_SESSION_ON;

        struct lt_l3_gen_frame_t *p_frame = (struct lt_l3_gen_frame_t *)&h.l3.buff;
        lt_aesgcm_encrypt_ExpectAndReturn(&h.l3.encrypt, h.l3.encryption_IV, TR01_L3_IV_SIZE, (uint8_t *)"", 0,
                                          p_frame->data, p_frame->cmd_size, p_frame->data + p_frame->cmd_size,
                                          TR01_L3_TAG_SIZE, LT_OK);

        int ret = lt_l3_encrypt_request(&h.l3);
        TEST_ASSERT_EQUAL(LT_OK, ret);
    }
}
