
#include "unity.h"
#include "string.h"
#include "time.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l2_api_structs.h"
#include "lt_l3_api_structs.h"

#include "mock_lt_random.h"
#include "mock_lt_l1_port_wrap.h"
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
        snprintf(buffer, sizeof(buffer), "Using random seed: %ld\n", seed);
        TEST_MESSAGE(buffer);
        srand((unsigned int)seed);
    #endif
}

void tearDown(void)
{
}

//---------------------------------------------------------------------

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

void test_lt_ping__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    uint8_t msg_out = 1;
    uint8_t msg_in = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ping(&h, &msg_out, &msg_in, 1));
}

void test_lt_ping__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    uint8_t msg_out, msg_in;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ping(&h, &msg_out, &msg_in, 0));
    }
}

uint16_t lt_ping_cmd_size_inject_value;

lt_ret_t callback_lt_ping_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;
    p_l3_res->res_size = lt_ping_cmd_size_inject_value;

    return LT_OK;
}

void test_lt_ping__len_mismatch()
{
    const int msg_max_size = 200;
    uint8_t   msg_out[msg_max_size], msg_in[msg_max_size];
    int       rand_size, rand_len_offset;

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % msg_max_size;
        rand_len_offset = rand() % (msg_max_size - rand_size);
        if (rand_len_offset == 1) {
            rand_len_offset++;
        }

        lt_ping_cmd_size_inject_value = (uint16_t)rand_size;

        // Packet size for both cmd and res has the same position, so
        // it would be overwritten by "p_l3_cmd->cmd_size = len + 1;",
        // so we need to modify the value inside the callback.
        lt_l3_cmd_StubWithCallback(callback_lt_ping_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_ping(&h, msg_out, msg_in, (uint16_t)(rand_size + rand_len_offset)));
    }
}

void test_lt_ping__correct()
{
    const int       msg_max_size = 200;
    uint8_t         msg_out[200], msg_in[200];

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    // Because packet size position is shared by both cmd and res,
    // it will already be set correctly by "p_l3_cmd->cmd_size = len + 1;".
    lt_l3_cmd_ExpectAndReturn(&h, LT_OK);
    TEST_ASSERT_EQUAL(LT_OK, lt_ping(&h, msg_out, msg_in, (uint16_t)(rand() % msg_max_size)));
}

//---------------------------------------------------------------------

void test_lt_pairing_key_write__no_session()
{
    lt_handle_t h =  {0};

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_pairing_key_write(&h, pubkey, slot));
}

void test_lt_pairing_key_write__l3_cmd_fail()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_ExpectAndReturn(&h, LT_FAIL);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_write(&h, pubkey, slot));
}

// // // //
lt_ret_t callback_lt_pairing_key_write(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_write_res_t* p_l3_res = (struct lt_l3_pairing_key_write_res_t*)&h->l3_buff;
    p_l3_res->res_size = 100;

    return LT_OK;
}

void test_lt_pairing_key_write__len_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_Stub(callback_lt_pairing_key_write);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_write(&h, pubkey, slot));
}

lt_ret_t callback2_lt_pairing_key_write(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_write_res_t* p_l3_res = (struct lt_l3_pairing_key_write_res_t*)&h->l3_buff;
    p_l3_res->res_size = 1;

    return LT_OK;
}

void test_lt_pairing_key_write__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_Stub(callback2_lt_pairing_key_write);

    TEST_ASSERT_EQUAL(LT_OK, lt_pairing_key_write(&h, pubkey, slot));
}

//---------------------------------------------------------------------

void test_lt_pairing_key_read__no_session()
{
    lt_handle_t h =  {0};

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_pairing_key_read(&h, pubkey, slot));
}


void test_lt_pairing_key_read__l3_cmd_fail()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_ExpectAndReturn(&h, LT_FAIL);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_read(&h, pubkey, slot));
}

// // // //
lt_ret_t callback_lt_pairing_key_read(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = 100;

    return LT_OK;
}

void test_lt_pairing_key_read__len_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_Stub(callback_lt_pairing_key_read);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_read(&h, pubkey, slot));
}

lt_ret_t callback2_lt_pairing_key_read(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = LT_L3_PAIRING_KEY_READ_RES_SIZE;

    return LT_OK;
}

void test_lt_pairing_key_read__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_Stub(callback2_lt_pairing_key_read);

    TEST_ASSERT_EQUAL(LT_OK, lt_pairing_key_read(&h, pubkey, slot));
}

//---------------------------------------------------------------------

void test_lt_pairing_key_invalidate__no_session()
{
    lt_handle_t h =  {0};

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_pairing_key_invalidate(&h, slot));
}

void test_lt_pairing_key_invalidate__l3_cmd_fail()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_ExpectAndReturn(&h, LT_FAIL);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_invalidate(&h, slot));
}


lt_ret_t callback_lt_pairing_key_invalidate(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = 100;

    return LT_OK;
}

void test_lt_pairing_key_invalidate__len_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_Stub(callback_lt_pairing_key_invalidate);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_invalidate(&h, slot));
}

lt_ret_t callback2_lt_pairing_key_invalidate(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = 1;

    return LT_OK;
}

void test_lt_pairing_key_invalidate__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_Stub(callback2_lt_pairing_key_invalidate);

    TEST_ASSERT_EQUAL(LT_OK, lt_pairing_key_invalidate(&h, slot));
}




//---------------------------------------------------------------------
void test_lt_r_mem_data_write__no_session()
{
    lt_handle_t h = {0};
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_r_mem_data_write(&h, udata_slot, udata, size));
}

//---------------------------------------------------------------------
void test_lt_r_mem_data_read__no_session()
{
    lt_handle_t h = {0};
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_r_mem_data_read(&h, udata_slot, udata, size));
}

//---------------------------------------------------------------------
void test_lt_r_mem_data_erase__no_session()
{
    lt_handle_t h = {0};
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_r_mem_data_erase(&h, udata_slot));
}

//---------------------------------------------------------------------

void test_lt_random_get__no_session()
{
    uint8_t     buff[200];
    lt_handle_t h =  {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_random_get(&h, buff, sizeof(buff)));
}

void test_lt_random_get__l3_fail()
{
    uint8_t     buff[10];
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < 4; i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_random_get(&h, buff, sizeof(buff)));
    }
}

uint16_t lt_random_get_cmd_size_inject_value;

lt_ret_t callback_lt_random_get_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;
    p_l3_res->res_size = lt_random_get_cmd_size_inject_value;

    return LT_OK;
}

void test_lt_random_get__len_mismatch()
{
    const int                buff_max_size = 200;
    uint8_t                  buff[buff_max_size];
    int                      rand_size, rand_len_offset;

    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % buff_max_size;
        rand_len_offset = rand() % (buff_max_size - rand_size);
        if (rand_len_offset == 4) {
            rand_len_offset++;
        }

        lt_random_get_cmd_size_inject_value = (uint16_t)rand_size;

        lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_random_get(&h, buff, (uint16_t)(rand_size + rand_len_offset)));
    }
}

void test_lt_random_get__correct()
{
    const int       buff_max_size = 200;
    uint8_t         buff[200];
    int             cmd_size;

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    // Making this at least 4 to not underflow in cmd_size - 4.
    cmd_size = (rand() % (buff_max_size - 4)) + 4;

    // No correct value will be set for us as in ping, so injecting again...
    lt_random_get_cmd_size_inject_value = (uint16_t)cmd_size;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_random_get(&h, buff, (uint16_t)(cmd_size - 4)));
}

//---------------------------------------------------------------------

void test_lt_ecc_key_generate__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));
    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_P256));
}

void test_lt_ecc_key_generate__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < 4; i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));
    }
}

uint16_t lt_ecc_key_generate_cmd_size_inject_value;

lt_ret_t callback_lt_ecc_key_generate_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;
    p_l3_res->res_size = lt_ecc_key_generate_cmd_size_inject_value;

    return LT_OK;
}

void test_lt_ecc_key_generate__cmd_size_mismatch()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_random_get_cmd_size_inject_value = 0;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));

    lt_random_get_cmd_size_inject_value = 2;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));

    lt_random_get_cmd_size_inject_value = (uint16_t)((rand() % (L3_PACKET_MAX_SIZE - 2)) + 2);
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, ECC_SLOT_1, CURVE_ED25519));
}

void test_lt_ecc_key_generate__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_random_get_cmd_size_inject_value = 1;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);

    for (ecc_slot_t slot = ECC_SLOT_1; slot < ECC_SLOT_31; slot++) {
        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, CURVE_ED25519));
        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, CURVE_P256));
    }
}

//---------------------------------------------------------------------

void test_lt_ecc_key_read__no_session()
{
    lt_handle_t h =  {0};
    h.session     =  0;

    uint8_t          key[64];
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
}

void test_lt_ecc_key_read__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    uint8_t          key[64] = {0};
    ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
    }
}

uint16_t            lt_ecc_key_read_cmd_size_inject_value;
ecc_curve_type_t    lt_ecc_key_read_curve_inject_value;

lt_ret_t callback_lt_ecc_key_read_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_ecc_key_read_res_t* p_l3_res = (struct lt_l3_ecc_key_read_res_t*)&h->l3_buff;

    p_l3_res->res_size = lt_ecc_key_read_cmd_size_inject_value;
    p_l3_res->curve       = lt_ecc_key_read_curve_inject_value;

    return LT_OK;
}

void test_lt_ecc_key_read__ed25519_size_mismatch()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    uint8_t          key[64] = {0};
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    for (int i = 0; i < 25; i++) {
        lt_ecc_key_read_cmd_size_inject_value = (uint16_t)(rand() % L3_PACKET_MAX_SIZE);

        if (lt_ecc_key_read_cmd_size_inject_value != 48) { // skip correct value
            lt_ecc_key_read_curve_inject_value       = CURVE_ED25519;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }

        if (lt_ecc_key_read_cmd_size_inject_value != 80) { // skip correct value
            lt_ecc_key_read_curve_inject_value       = CURVE_P256;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }
    }
}

void test_lt_ecc_key_read__correct()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    uint8_t          key[64] = {0};
    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    lt_ecc_key_read_cmd_size_inject_value = 48;
    lt_ecc_key_read_curve_inject_value       = CURVE_ED25519;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));

    lt_ecc_key_read_cmd_size_inject_value = 80;
    lt_ecc_key_read_curve_inject_value       = CURVE_P256;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
}

//---------------------------------------------------------------------

void test_lt_ecc_eddsa_sign__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    uint8_t     msg[10] = {0};
    uint8_t     rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}

void test_lt_ecc_eddsa_sign__l3_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t     msg[10] = {0};
    uint8_t     rs[64] = {0};

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
    }
}

lt_ret_t callback_lt_ecc_eddsa_sign_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    p_l3_res->res_size = 0x50;

    return LT_OK;
}

void test_lt_ecc_eddsa_sign__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t     msg[10] = {0};
    uint8_t     rs[64] = {0};

    lt_l3_cmd_Stub(callback_lt_ecc_eddsa_sign_lt_l3_cmd);

    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}

//---------------------------------------------------------------------

void test_lt_ecc_ecdsa_sign__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    uint8_t     msg[10] = {0};
    uint8_t     rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}

void test_lt_ecc_ecdsa_sign__l3_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t                msg[10] = {0};
    uint8_t                rs[64]  = {0};
    lt_crypto_sha256_ctx_t hctx         = {0};
    uint8_t                msg_hash[32] = {0};

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_sha256_init_Expect(&hctx);
        lt_sha256_start_Expect(&hctx);
        lt_sha256_update_Expect(&hctx, msg, sizeof(msg));
        lt_sha256_finish_Expect(&hctx, msg_hash);
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
    }
}

lt_ret_t callback_lt_ecc_ecdsa_sign_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    p_l3_res->res_size = 0x50;

    return LT_OK;
}

void test_lt_ecc_ecdsa_sign__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t                msg[10] = {0};
    uint8_t                rs[64]  = {0};
    lt_crypto_sha256_ctx_t hctx         = {0};
    uint8_t                msg_hash[32] = {0};

    lt_sha256_init_Expect(&hctx);
    lt_sha256_start_Expect(&hctx);
    lt_sha256_update_Expect(&hctx, msg, sizeof(msg));
    lt_sha256_finish_Expect(&hctx, msg_hash);
    lt_l3_cmd_Stub(callback_lt_ecc_ecdsa_sign_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}

//---------------------------------------------------------------------

void test_lt_ecc_eddsa_sig_verify__fail()
{
    uint8_t msg[10]= {0};
    uint8_t pubkey[64]= {0};
    uint8_t rs[64] = {0};

    lt_ed25519_sign_open_ExpectAndReturn(msg, sizeof(msg), pubkey, rs, 1);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_eddsa_sig_verify(msg, sizeof(msg), pubkey, rs));
}

void test_lt_ecc_eddsa_sig_verify__correct()
{
    uint8_t msg[10]    = {0};
    uint8_t pubkey[64] = {0};
    uint8_t rs[64]     = {0};

    lt_ed25519_sign_open_ExpectAndReturn(msg, sizeof(msg), pubkey, rs, 0);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_eddsa_sig_verify(msg, sizeof(msg), pubkey, rs));
}

//---------------------------------------------------------------------

void test_lt_ecc_key_erase__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_erase(&h, ECC_SLOT_1));
}

void test_lt_ecc_key_erase__l3_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_erase(&h, ECC_SLOT_1));
    }
}

lt_ret_t callback_lt_ecc_key_erase_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    p_l3_res->res_size = 1;

    return LT_OK;
}

void test_lt_ecc_key_erase__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_l3_cmd_Stub(callback_lt_ecc_key_erase_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_erase(&h, ECC_SLOT_1));
}

//---------------------------------------------------------------------

lt_ret_t callback1_lt_get_info_cert_lt_l2_transfer(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_rsp->rsp_len = 128;

    switch(cmock_num_calls) {
        case 0:
            return LT_L1_SPI_ERROR;

        case 1:
            return LT_OK;
        case 2:
            return LT_L1_SPI_ERROR;

        case 3:
            return LT_OK;
        case 4:
            return LT_OK;
        case 5:
            return LT_L1_SPI_ERROR;

        case 6:
            return LT_OK;
        case 7:
            return LT_OK;
        case 8:
            return LT_OK;
        case 9:
            return LT_L1_SPI_ERROR;

        default:
            return 100;
    }
}

void test_lt_get_info_cert__l2_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    lt_l2_transfer_StubWithCallback(callback1_lt_get_info_cert_lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));
}

lt_ret_t callback2_lt_get_info_cert_lt_l2_transfer(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_rsp->rsp_len = 128;

    switch(cmock_num_calls) {
        case 0:
            return LT_OK;
        case 1:
            return LT_OK;
        case 2:
            return LT_OK;
        case 3:
            return LT_OK;
        default:
            return 100;
    }
}

void test_lt_get_info_cert__l2_correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    for (int i = 0; i < 4; i++) {
        lt_l2_transfer_StubWithCallback(callback2_lt_get_info_cert_lt_l2_transfer);
    }
    TEST_ASSERT_EQUAL(LT_OK, lt_get_info_cert(&h, cert, sizeof(cert)));
}

//---------------------------------------------------------------------