/**
 * @file test_ts_l1.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"

#include "libtropic_common.h"
#include "lt_l1.h"

#include "mock_lt_l1_port_wrap.h"


#define SOME_UNUSED_DEFAULT_BYTE 0xfe

#define L2_BUFF_WHEN_NOT_LT_L1_DATA_LEN_ERROR 0

void setUp(void)
{
}

void tearDown(void)
{
}


//---------------------------------------------------------------------------------------------------------------------
// Used to force l2_buff[0] to contain zeroed busy bit
static lt_ret_t callback_CHIP_BUSY(lt_handle_t* h, uint8_t offset, uint16_t tx_len, uint32_t timeout,
                                   int cmock_num_calls) {
    h->l2_buff[0] = 0;
    return LT_OK;
}

// Test if function returns LT_L1_CHIP_BUSY when transferred chip status byte has READY bit == 0
void test_lt_l1_read___CHIP_BUSY()
{
    lt_handle_t h = {0};

    for(int i=0; i< LT_L1_READ_MAX_TRIES; i++) {
        lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
        lt_l1_spi_transfer_StubWithCallback(callback_CHIP_BUSY);
        lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);
        lt_l1_delay_ExpectAndReturn(&h, LT_L1_READ_RETRY_DELAY, LT_OK);
    }
    TEST_ASSERT_EQUAL(LT_L1_CHIP_BUSY, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test SPI error during transfer
void test_lt_l1_read___LT_L1_SPI_ERROR()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_ExpectAndReturn(&h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_FAIL);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to force l2_buff[0] to contain ALARM bit
static lt_ret_t callback_LT_L1_CHIP_ALARM_MOD(lt_handle_t* h, uint8_t offset, uint16_t tx_len,
                                              uint32_t timeout, int cmock_num_calls) {
    h->l2_buff[0] = CHIP_MODE_ALARM_bit;
    return LT_OK;
}

// Test if function returns LT_L1_CHIP_ALARM_MODE when transferred chip status byte has ALARM bit == 1
void test_lt_l1_read___LT_L1_CHIP_ALARM_MODE()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_StubWithCallback(callback_LT_L1_CHIP_ALARM_MOD);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_CHIP_ALARM_MODE, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to force l2_buff[0] to contain STARTUP bit
static lt_ret_t callback_LT_L1_CHIP_STARTUP_MODE(lt_handle_t* h, uint8_t offset, uint16_t tx_len,
                                                 uint32_t timeout, int cmock_num_calls) {
    h->l2_buff[0] = CHIP_MODE_STARTUP_bit;
    return LT_OK;
}
// Test if function returns LT_L1_CHIP_STARTUP_MODE when transferred chip status byte has STARTUP bit == 1
void test_lt_l1_read___LT_L1_CHIP_STARTUP_MODE()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_StubWithCallback(callback_LT_L1_CHIP_STARTUP_MODE);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_CHIP_STARTUP_MODE, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to force values so first LT_L1_SPI_ERROR is returned
static lt_ret_t callback_CHIP_MODE_READY_bit(lt_handle_t* h, uint8_t offset, uint16_t tx_len,
                                             uint32_t timeout, int cmock_num_calls)
{
    if(cmock_num_calls == 0) {
        h->l2_buff[0] = CHIP_MODE_READY_bit;
        return LT_OK;
    }
    if(cmock_num_calls == 1) {
        return LT_FAIL;
    }

    // Just in case, return some invalid number:
    return 0xfefe;
}

// Test if function returns LT_L1_SPI_ERROR if chip is in ready mode and lt_l1_spi_transfer() fails
void test_lt_l1_read___CHIP_MODE_READY_LT_L1_SPI_ERROR()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_StubWithCallback(callback_CHIP_MODE_READY_bit);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to values so fpga resp handling is always called
static lt_ret_t callback_CHIP_MODE_READY_fpga_no_resp(lt_handle_t* h, uint8_t offset, uint16_t tx_len,
                                                      uint32_t timeout, int cmock_num_calls)
{
    if(cmock_num_calls % 2 == 0) {
        h->l2_buff[0] = CHIP_MODE_READY_bit;
        return LT_OK;
    }
    if(cmock_num_calls % 2 == 1) {
        h->l2_buff[1] = 0xff;
        return LT_OK;
    }

    // Just in case, return some invalid number:
    return 0xfefe;
}

// Test if function returns LT_L1_CHIP_BUSY when chip status is ready and lt_handle_t.l2_buff[1] == 0xff
void test_lt_l1_read___CHIP_MODE_READY_fpga_no_resp()
{
    lt_handle_t h = {0};

    for(int i=0; i< LT_L1_READ_MAX_TRIES; i++) {
        lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
        lt_l1_spi_transfer_StubWithCallback(callback_CHIP_MODE_READY_fpga_no_resp);
        lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);
        lt_l1_delay_ExpectAndReturn(&h, LT_L1_READ_RETRY_DELAY, LT_OK);
    }
    TEST_ASSERT_EQUAL(LT_L1_CHIP_BUSY, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to force values so LT_L1_LEN_MAX is returned
static lt_ret_t callback_CHIP_MODE_READY_LT_L1_DATA_LEN_ERROR(lt_handle_t* h, uint8_t offset,
                                                              uint16_t tx_len, uint32_t timeout,
                                                              int cmock_num_calls)
{
    if(cmock_num_calls == 0) {
        h->l2_buff[0] = CHIP_MODE_READY_bit;
        return LT_OK;
    }
    if(cmock_num_calls == 1) {
        h->l2_buff[1] = 0;
        h->l2_buff[2] = LT_L1_LEN_MAX - 2;
        return LT_OK;
    }

    // Just in case, return some invalid number:
    return 0xfefe;
}

// Test if function returns LT_L1_DATA_LEN_ERROR if chip is in ready mode
void test_lt_l1_read___CHIP_MODE_READY_LT_L1_DATA_LEN_ERROR()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_StubWithCallback(callback_CHIP_MODE_READY_LT_L1_DATA_LEN_ERROR);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_DATA_LEN_ERROR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Used to force values so second LT_L1_SPI_ERROR is returned
static lt_ret_t callback_CHIP_MODE_READY_LT_L1_SPI_ERROR_2(lt_handle_t* h, uint8_t offset, uint16_t tx_len,
                                                           uint32_t timeout, int cmock_num_calls)
{
    if(cmock_num_calls == 0) {
        h->l2_buff[0] = CHIP_MODE_READY_bit;
        return LT_OK;
    }
    if(cmock_num_calls == 1) {
        h->l2_buff[1] = 0;
        h->l2_buff[2] = L2_BUFF_WHEN_NOT_LT_L1_DATA_LEN_ERROR;
        return LT_OK;
    }
    if(cmock_num_calls == 2) {
        return LT_FAIL;
    }

    // Just in case, return some invalid number:
    return 0xfefe;
}

// Test if function returns LT_L1_SPI_ERROR when receiving the rest of incoming bytes
void test_lt_l1_read___CHIP_MODE_READY_LT_L1_SPI_ERROR_2()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_StubWithCallback(callback_CHIP_MODE_READY_LT_L1_SPI_ERROR_2);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test LT_L1_SPI_ERROR return value during write
void test_lt_l1_write___LT_L1_SPI_ERROR()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_ExpectAndReturn(&h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_FAIL);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l1_write(&h, 1, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test LT_OK return value
void test_lt_l1_write___LT_OK()
{
    lt_handle_t h = {0};

    lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
    lt_l1_spi_transfer_ExpectAndReturn(&h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    TEST_ASSERT_EQUAL(LT_OK, lt_l1_write(&h, 1, LT_L1_TIMEOUT_MS_DEFAULT));
}

//---------------------------------------------------------------------------------------------------------------------
