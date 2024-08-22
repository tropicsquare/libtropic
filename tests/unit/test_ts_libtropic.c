
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l2_api_structs.h"
#include "lt_l3_api_structs.h"

#include "mock_lt_random.h"
#include "mock_lt_l1.h"
#include "mock_lt_l2.h"
#include "mock_lt_l3.h"
#include "mock_lt_x25519.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_sha256.h"
#include "mock_lt_aesgcm.h"


void setUp(void)
{
    
}

void tearDown(void)
{
}

// Test if function returns LT_FAIL when l1 init failed
void test_lt_init___error_during_lt_l1_init()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_init___correct()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------

// Test if function returns LT_FAIL when l1 deinit failed
void test_lt_deinit___error_during_lt_l1_deinit()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_deinit___correct()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------

void test_lt_ping__l3_fail()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg_out, msg_in;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (int i = 0; i < sizeof(rets); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ping(&h, &msg_out, &msg_in, 0));
    }
}

int lt_ping_packet_size_inject_value;

lt_ret_t callback_lt_ping_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;
    p_l3_res->packet_size = lt_ping_packet_size_inject_value;

    return LT_OK;
}

void test_lt_ping__len_mismatch()
{   
    const size_t             msg_max_size = 200;
    lt_handle_t              h;
    uint8_t                  msg_out[msg_max_size], msg_in[msg_max_size];
    int                      rand_size, rand_len_offset;

    h.session = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % msg_max_size;
        rand_len_offset = rand() % (msg_max_size - rand_size);
        if (rand_len_offset == 1) {
            rand_len_offset++;
        }

        lt_ping_packet_size_inject_value = rand_size;

        lt_l3_cmd_Stub(callback_lt_ping_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_ping(&h, msg_out, msg_in, rand_size + rand_len_offset));
    }
}

void test_lt_ping__correct()
{
    const size_t    msg_max_size = 200;
    lt_handle_t     h;
    uint8_t         msg_out[200], msg_in[200];

    h.session = SESSION_ON;

    lt_l3_cmd_ExpectAndReturn(&h, LT_OK);
    TEST_ASSERT_EQUAL(LT_OK, lt_ping(&h, msg_out, msg_in, rand() % msg_max_size));
}
