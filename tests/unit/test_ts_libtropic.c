
#include "unity.h"
#include "string.h"
#include "time.h"

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
    char buffer[100];
    #ifdef RNG_SEED
        srand(RNG_SEED);
    #else
        time_t seed = time(NULL);
        // Using this approach, because in our version of Unity there's no TEST_PRINTF yet.
        // Also, raw printf is worse solution (without additional debug msgs, such as line).
        snprintf(buffer, sizeof(buffer), "Using random seed: %lu\n", seed);
        TEST_MESSAGE(buffer);
        srand(seed);
    #endif
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
    lt_handle_t h =  {0};
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
    lt_handle_t h =  {0};
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

        // Packet size for both cmd and res has the same position, so
        // it would be overwritten by "p_l3_cmd->packet_size = len + 1;",
        // so we need to modify the value inside the callback.
        lt_l3_cmd_Stub(callback_lt_ping_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_ping(&h, msg_out, msg_in, rand_size + rand_len_offset));
    }
}

void test_lt_ping__correct()
{
    const size_t    msg_max_size = 200;
    lt_handle_t h =  {0};
    uint8_t         msg_out[200], msg_in[200];

    h.session = SESSION_ON;

    // Because packet size position is shared by both cmd and res,
    // it will already be set correctly by "p_l3_cmd->packet_size = len + 1;".
    lt_l3_cmd_ExpectAndReturn(&h, LT_OK);
    TEST_ASSERT_EQUAL(LT_OK, lt_ping(&h, msg_out, msg_in, rand() % msg_max_size));
}

//---------------------------------------------------------------------

void test_lt_random_get__l3_fail()
{
    lt_handle_t h =  {0};
    uint8_t     buff[10];

    h.session = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (int i = 0; i < sizeof(rets); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_random_get(&h, buff, sizeof(buff)));
    }
}

int lt_random_get_packet_size_inject_value;

lt_ret_t callback_lt_random_get_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;
    p_l3_res->packet_size = lt_random_get_packet_size_inject_value;

    return LT_OK;
}

void test_lt_random_get__len_mismatch()
{
    const size_t             buff_max_size = 200;
    lt_handle_t h =  {0};
    uint8_t                  buff[buff_max_size];
    int                      rand_size, rand_len_offset;

    h.session = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % buff_max_size;
        rand_len_offset = rand() % (buff_max_size - rand_size);
        if (rand_len_offset == 4) {
            rand_len_offset++;
        }

        lt_random_get_packet_size_inject_value = rand_size;

        lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_random_get(&h, buff, rand_size + rand_len_offset));
    }
}

void test_lt_random_get__correct()
{
    const size_t    buff_max_size = 200;
    uint8_t         buff[200];
    int16_t         packet_size;
    lt_handle_t h =  {0};

    h.session   = SESSION_ON;
    packet_size = rand() % buff_max_size;

    // No correct value will be set for us as in ping, so injecting again...
    lt_random_get_packet_size_inject_value = packet_size;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_random_get(&h, buff, packet_size - 4));
}

//---------------------------------------------------------------------

void test_lt_ecc_key_generate__l3_fail()
{
    lt_handle_t h =  {0};

    h.session = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (int i = 0; i < sizeof(rets); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));
    }
}

int lt_ecc_key_generate_packet_size_inject_value;

lt_ret_t callback_lt_ecc_key_generate_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;
    p_l3_res->packet_size = lt_ecc_key_generate_packet_size_inject_value;

    return LT_OK;
}

void test_lt_ecc_key_generate__packet_size_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;

    lt_random_get_packet_size_inject_value = 0;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));

    lt_random_get_packet_size_inject_value = 2;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));

    lt_random_get_packet_size_inject_value = (rand() % 65534) + 2;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));
}

void test_lt_ecc_key_generate__correct()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;

    lt_random_get_packet_size_inject_value = 1;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);

    for (ecc_slot_t slot = ECC_SLOT_1; slot < ECC_SLOT_32; slot++) {
        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, CURVE_ED25519));
        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, CURVE_P256));
    }
}

//---------------------------------------------------------------------

void test_lt_ecc_key_read__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;
    
    uint8_t          key[64];
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (int i = 0; i < sizeof(rets); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
    }
}

int                 lt_ecc_key_read_packet_size_inject_value;
ecc_curve_type_t    lt_ecc_key_read_curve_inject_value;

lt_ret_t callback_lt_ecc_key_read_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_ecc_key_read_res_t* p_l3_res = (struct lt_l3_ecc_key_read_res_t*)&h->l3_buff;

    p_l3_res->packet_size = lt_ecc_key_read_packet_size_inject_value;
    p_l3_res->curve       = lt_ecc_key_read_curve_inject_value;

    return LT_OK;
}

void test_lt_ecc_key_read__ed25519_size_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    
    uint8_t          key[64];
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    for (int i = 0; i < 25; i++) {
        lt_ecc_key_read_packet_size_inject_value = rand() % L3_PACKET_MAX_SIZE;
        
        if (lt_ecc_key_read_packet_size_inject_value != 48) { // skip correct value
            lt_ecc_key_read_curve_inject_value       = CURVE_ED25519;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }

        if (lt_ecc_key_read_packet_size_inject_value != 80) { // skip correct value
            lt_ecc_key_read_curve_inject_value       = CURVE_P256;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }
    }
}

void test_lt_ecc_key_read__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    
    uint8_t          key[64];
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    lt_ecc_key_read_packet_size_inject_value = 48;
    lt_ecc_key_read_curve_inject_value       = CURVE_ED25519;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));

    lt_ecc_key_read_packet_size_inject_value = 80;
    lt_ecc_key_read_curve_inject_value       = CURVE_P256;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
}

//---------------------------------------------------------------------