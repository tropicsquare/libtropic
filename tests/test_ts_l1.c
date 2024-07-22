
#include "unity.h"

#include "ts_common.h"

#include "ts_l1.h"
#include "mock_support_ts_l1.h"


#define SOME_UNUSED_DEFAULT_BYTE 0xfe

/*
[ ] pridat popis erroru do verbose output funkce
*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_ts_l1_read___CHIP_BUSY()
{
    ts_handle_t h = {0};

    for(int i=0; i< TS_L1_READ_MAX_TRIES; i++) {
        ts_l1_spi_csn_low_ExpectAndReturn(&h, TS_OK);
        ts_l1_spi_transfer_IgnoreAndReturn(TS_OK);
        ts_l1_spi_csn_high_ExpectAndReturn(&h, TS_OK);
        ts_l1_delay_ExpectAndReturn(&h, TS_L1_READ_RETRY_DELAY, TS_OK);
    }
    TEST_ASSERT_EQUAL(TS_L1_CHIP_BUSY, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___TS_L1_SPI_ERROR()
{
    ts_handle_t h = {0};

    ts_l1_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_l1_spi_transfer_IgnoreAndReturn(TS_FAIL);
    ts_l1_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_SPI_ERROR, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___TS_L1_CHIP_ALARM_MODE()
{
    ts_handle_t h = {0};

    h.l2_buff_resp[0] = CHIP_MODE_ALARM_mask;

    ts_l1_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_l1_spi_transfer_IgnoreAndReturn(TS_OK);
    ts_l1_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_CHIP_ALARM_MODE, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___TS_L1_CHIP_STARTUP_MODE()
{
    ts_handle_t h = {0};

    h.l2_buff_resp[0] =CHIP_MODE_STARTUP_mask;

    ts_l1_spi_csn_low_ExpectAndReturn(&h, TS_OK);
    ts_l1_spi_transfer_IgnoreAndReturn(TS_OK);
    ts_l1_spi_csn_high_ExpectAndReturn(&h, TS_OK);

    TEST_ASSERT_EQUAL(TS_L1_CHIP_STARTUP_MODE, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}
