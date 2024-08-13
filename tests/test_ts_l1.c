
#include "unity.h"

#include "libtropic_common.h"
#include "ts_l1.h"

#include "mock_libtropic_port.h"


#define SOME_UNUSED_DEFAULT_BYTE 0xfe


void setUp(void)
{
}

void tearDown(void)
{
}

//------------------------------------------------------------------------//
// Used to force l2_buff[0] to contain zeroed busy bit
static ts_ret_t callback_CHIP_BUSY(ts_handle_t* h, uint8_t offset, uint16_t tx_len, uint32_t timeout, int cmock_num_calls) {
    h->l2_buff[0] = 0;
    return TS_OK;
}
void test_ts_l1_read___CHIP_BUSY()
{
    ts_handle_t h = {0};

    for(int i=0; i< TS_L1_READ_MAX_TRIES; i++) {
        ts_port_spi_csn_low_ExpectAndReturn(&h, TS_OK);
        ts_port_spi_transfer_StubWithCallback(callback_CHIP_BUSY);
        ts_port_spi_csn_high_ExpectAndReturn(&h, TS_OK);
        ts_port_delay_ExpectAndReturn(&h, TS_L1_READ_RETRY_DELAY, TS_OK);
    }
    TEST_ASSERT_EQUAL(TS_L1_CHIP_BUSY, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

//------------------------------------------------------------------------//
// SPI error during transfer
void test_ts_l1_read___TS_L1_SPI_ERROR()
{
    ts_handle_t h = {0};

    ts_port_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_port_spi_transfer_ExpectAndReturn(&h, 0, 1, TS_L1_TIMEOUT_MS_DEFAULT, TS_FAIL);
    ts_port_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_SPI_ERROR, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

//------------------------------------------------------------------------//
// Used to force l2_buff[0] to contain ALARM bit
static ts_ret_t callback_TS_L1_CHIP_ALARM_MOD(ts_handle_t* h, uint8_t offset, uint16_t tx_len, uint32_t timeout, int cmock_num_calls) {
    h->l2_buff[0] = CHIP_MODE_ALARM_bit;
    return TS_OK;
}
void test_ts_l1_read___TS_L1_CHIP_ALARM_MODE()
{
    ts_handle_t h = {0};

    ts_port_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_port_spi_transfer_StubWithCallback(callback_TS_L1_CHIP_ALARM_MOD);
    ts_port_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_CHIP_ALARM_MODE, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

//------------------------------------------------------------------------//
// Used to force l2_buff[0] to contain STARTUP bit
static ts_ret_t callback_TS_L1_CHIP_STARTUP_MODE(ts_handle_t* h, uint8_t offset, uint16_t tx_len, uint32_t timeout, int cmock_num_calls) {
    h->l2_buff[0] = CHIP_MODE_STARTUP_bit;
    return TS_OK;
}
void test_ts_l1_read___TS_L1_CHIP_STARTUP_MODE()
{
    ts_handle_t h = {0};

    ts_port_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_port_spi_transfer_StubWithCallback(callback_TS_L1_CHIP_STARTUP_MODE);
    ts_port_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_CHIP_STARTUP_MODE, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}
