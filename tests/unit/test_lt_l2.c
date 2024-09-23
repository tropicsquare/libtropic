
#include "unity.h"

#include "libtropic_common.h"
#include "lt_l2.h"
#include "lt_l2_api_structs.h"

#include "mock_lt_l1.h"
#include "mock_lt_l2_frame_check.h"
#include "mock_lt_crc16.h"


/*
[ ] pridat popis erroru do verbose output funkce
*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_lt_l2_transfer__fail_during_l1_write()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__fail_during_l1_read()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__fail_during_frame_check()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_IN_CRC_ERR);

    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__correct()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);

    TEST_ASSERT_EQUAL(LT_OK, lt_l2_transfer(&h));
}

//--------------------------------------------------------------------------------------------------

void test_lt_l2_encrypted_cmd__l1_write_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__l1_read_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__frame_check_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_IN_CRC_ERR);

    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_encrypted_cmd(&h));
}

// Test two chunk long packet, throw an error on the second chunk.
void test_lt_l2_encrypted_cmd__multiple_chunks()
{
    lt_handle_t h = {0};

    struct lt_l3_gen_frame_t * p_frame = (struct lt_l3_gen_frame_t*)h.l3_buff;
    p_frame->cmd_size = L2_CHUNK_MAX_DATA_SIZE * 2;

    add_crc_Ignore();
    lt_l1_write_ExpectAndReturn(&h, L2_CHUNK_MAX_DATA_SIZE + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_REQ_CONT);

    lt_l1_write_ExpectAndReturn(&h, L2_CHUNK_MAX_DATA_SIZE + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__recv_lt_l1_read_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

lt_ret_t test_lt_l2_encrypted_cmd__overflow_callback(const uint8_t *frame, int cmock_num_calls)
{
    if (cmock_num_calls > 0) {
        struct lt_l2_encrypted_cmd_rsp_t* resp = (struct lt_l2_encrypted_cmd_rsp_t*) frame;
        resp->rsp_len = 255;
        return LT_L2_RES_CONT;
    } else {
        return LT_OK;
    }

}

// This is kinda tricky. resp->rsp_len is unsigned 8bit int => max value cannot be larger
// than L3_FRAME_MAX_SIZE, so overflow cannot be triggered by a callback. So, we need to
// trigger this by making offset larger.
void test_lt_l2_encrypted_cmd__recv_overflow()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_Stub(test_lt_l2_encrypted_cmd__overflow_callback);
    lt_l1_read_IgnoreAndReturn(LT_OK);

    TEST_ASSERT_EQUAL(LT_L3_DATA_LEN_ERROR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__recv_last_frame()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);

    TEST_ASSERT_EQUAL(LT_OK, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__recv_frame_err()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_IN_CRC_ERR);

    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__loop_overflow()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);

    for (int i = 0; i < 42; i++) {
        lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
        lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_RES_CONT);
    }

    TEST_ASSERT_EQUAL(LT_FAIL, lt_l2_encrypted_cmd(&h));
}
