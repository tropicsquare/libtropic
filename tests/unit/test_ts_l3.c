
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "ts_l3.h"

#include "mock_ts_l2.h"
#include "mock_ts_aesgcm.h"

void setUp(void)
{
}

void tearDown(void)
{
}

// Test correct nonce initialization
void test_ts_l3_nonce_init___correct()
{
    ts_handle_t handle = {0};
    memset(handle.IV, 0xff, 12);

    uint8_t expected_1[12] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_1, handle.IV, 12);

    int ret = ts_l3_nonce_init(&handle);
    TEST_ASSERT_EQUAL(TS_OK, ret);

    uint8_t expected_2[12] = {0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_2, handle.IV, 12);
}

// Test if nonce increases correctly
void test_ts_l3_nonce_increase___correct()
{
    ts_handle_t handle = {0};

    for(int i=0;i<0xfffe;i++) {
        int ret = ts_l3_nonce_increase(&handle);
        TEST_ASSERT_EQUAL(TS_OK, ret);
    }

    TEST_ASSERT_EQUAL(0xfe, handle.IV[0]);
    TEST_ASSERT_EQUAL(0xff, handle.IV[1]);
    // TODO test higher bytes as well, but this takes some time
    //TEST_ASSERT_EQUAL(0xff, h.IV[9]);
    //TEST_ASSERT_EQUAL(0xff, h.IV[8]);
}

// Test if ts_l3_cmd() fails when secure session is not active
void test_ts_l3_cmd___fail_when_no_session()
{
    ts_handle_t handle = {0};

    int ret = ts_l3_cmd(&handle);
    TEST_ASSERT_EQUAL(TS_HOST_NO_SESSION, ret);
}

// Test if ts_l3_cmd() returns TS_FAIL, if some error happens during aesgcm encrypt operations
void test_ts_l3_cmd___fail_during_ts_aesgcm_encrypt()
{
    ts_handle_t h = {0};
    h.session = SESSION_ON;

    struct ts_l3_gen_frame_t * p_frame = (struct ts_l3_gen_frame_t*)&h.l3_buff;
    ts_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_FAIL);

    int ret = ts_l3_cmd(&h);
    TEST_ASSERT_EQUAL(TS_FAIL, ret);
}

// Test if ts_l3_cmd() returns TS_FAIL, if some error happens during l2 exchange
void test_ts_l3_cmd___fail_during_ts_l2_encrypted_cmd()
{
    ts_handle_t h = {0};
    h.session = SESSION_ON;

    struct ts_l3_gen_frame_t * p_frame = (struct ts_l3_gen_frame_t*)&h.l3_buff;
    ts_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_OK);

    ts_l2_encrypted_cmd_ExpectAndReturn(&h, TS_FAIL);


    int ret = ts_l3_cmd(&h);
    TEST_ASSERT_EQUAL(TS_FAIL, ret);
}

// Test if ts_l3_cmd() returns TS_FAIL, if some error happens during aesgcm decrypt operations
void test_ts_l3_cmd___fail_during_ts_aesgcm_decrypt()
{
    ts_handle_t h = {0};
    h.session = SESSION_ON;

    struct ts_l3_gen_frame_t * p_frame = (struct ts_l3_gen_frame_t*)&h.l3_buff;
    ts_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_OK);

    ts_l2_encrypted_cmd_ExpectAndReturn(&h, TS_OK);

    ts_aesgcm_decrypt_ExpectAndReturn(&h.decrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_FAIL);


    int ret = ts_l3_cmd(&h);
    TEST_ASSERT_EQUAL(TS_FAIL, ret);
}

// Test that all possible L3_IDs are correctly parsed in ts_l3cmd() function and that they are propagated to upper layers
void test_ts_l3_cmd___test_all_l3_results()
{
    // Note: correct execution is tested at the end
    uint8_t results[] = {L3_RESULT_FAIL, L3_RESULT_UNAUTHORIZED, L3_RESULT_INVALID_CMD, 0x99, L3_RESULT_OK};
    uint8_t returned[] = {TS_L3_FAIL, TS_L3_UNAUTHORIZED, TS_L3_INVALID_CMD, TS_FAIL, TS_OK};
    for (int i=0; i<5;i++) {
        ts_handle_t h = {0};
        h.session = SESSION_ON;

        struct ts_l3_gen_frame_t * p_frame = (struct ts_l3_gen_frame_t*)&h.l3_buff;
        ts_aesgcm_encrypt_ExpectAndReturn(&h.encrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_OK);

        ts_l2_encrypted_cmd_ExpectAndReturn(&h, TS_OK);

        ts_aesgcm_decrypt_ExpectAndReturn(&h.decrypt, h.IV, L3_IV_SIZE, (uint8_t *)"", 0, p_frame->data, p_frame->packet_size, p_frame->data + p_frame->packet_size, L3_TAG_SIZE,TS_OK);

        p_frame->data[0] = results[i];

        if(i == 4) {
            ts_l3_nonce_increase(&h);
        }

        int ret = ts_l3_cmd(&h);
        TEST_ASSERT_EQUAL(returned[i], ret);
    }
}