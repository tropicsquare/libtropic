# Functional Mock Tests
Functional Mock Tests are a special type of test which is run against mock HAL (not against any "real" target, including the model). As such, all communication can (and has to be) mocked. They are a supplement to classic [functional tests](./functional_tests.md) and are meant to test behavior that is not triggered easily on real targets.

## Compiling and Running Tests
These tests are compiled standalone in the `tests/functional_mock` directory. The tests can be compiled and run as following:

```shell
cd tests/functional_mock
mkdir -p build
cd build
cmake ..
make
ctest -V
```

## Adding a New Test

!!! important
    If possible and feasible, prefer to implement classic functional tests, as they can run against all platforms. Functional mock tests should be used to test only the behavior which cannot be easily tested against real targets, e.g., hardware errors that are extremely rare and cannot be easily triggered.

### Basic Concepts
To write this type of test, you need to understand TROPIC01's communication protocol at the lowest level. At first, remainder of units and layers of communication:

- *One unit of communication on L1 = SPI transfer*, represented by `lt_port_spi_transfer`. One more more bytes can be transferred during one transfer.
- *One unit of communication on L2 = frame*. Initiated by negedge on CSN and terminated by posedge on CSN. Frame can be transferred during single or multiple L1 transfer. As long as the CSN is low and any part of the frame is remaining, host can do SPI transfers. As a consequence, frame can be read even byte-by-byte.
- *One unit of communication on L3 = packet*. Encapsulated in L2 frame's REQ_DATA/RSP_DATA (similar to ISO/OSI encapsulation). L2 frame size is limited and as such it can happen that L3 packet has to be split into multiple frames. This is not really relevant from the point of the HAL — it is still a part of the frame.

Additional key points:

- We mock incoming data — the data that would be on the MISO line.
- There's no difference between "writing" and "reading". Everything is a SPI transaction, where the contents of buffers are swapped. We distinguish "reads" from "writes" only by a different `REQ_ID`. "Reads" have `REQ_ID == Get_Response (0xAA)`. Everything else is a "write". As such, you need to mock incoming data also for "writes", which is currently only a single `CHIP_STATUS` byte. Other bytes can be undefined/invalid/zero.
  - Actually, length of outcoming and incoming data shall always match, due to the nature of the SPI protocol. However, to simplify things, mock HAL does not require to fill the rest of the space after CHIP_STATUS with anything. If Libtropic tries to read incoming data (MISO) beyond data queued by user, mock HAL returns zeroes (up to the LT_L1_MAX_LENGTH). There's a logging macro which can be enabled to simplify test design.
- Data sent to the mocked HAL (outgoing data — "MOSI") are ignored in current implementation to keep test complexity bearable.
- Data are mocked by "frames", not by bytes. After CSN posedge, next mocked transaction is prepared in the internal mock queue.
  - By "frames", we also mean the single CHIP_STATUS that is sent when `REQ_ID == Get_Response`.
- If no mocked transaction is available in the queue, attempt at starting the transaction by setting CSN low (negedge) results in an error.
- Any attempt at SPI transfer while the CSN is high results in an error, as this would be an error in Libtropic implementation. Transfers can happen only during CSN low (after CSN negedge).

There are also some quirks to be aware of:

- When determining size of the mocked L2 Request, you cannot use `sizeof()`, as some structures are universal for multiple types of arguments. For example, `lt_l2_get_info_rsp_t()` is actually not always `sizeof(lt_l2_get_info_rsp_t)` bytes long. As a consequence, you have to either determine size manually, or use `calc_mocked_resp_len()`, which will calculate length for you (including the length of the CRC).
  - Another side effect is that CRC is not always in the `crc` field, it can be in the `object` field if data are shorter.

### Creating the Test
To add a new test, you need to:

1. Write the new test. Add a new file to `tests/functional_mock` called `lt_test_mock_<name>.c`. The test shall always contain a function called the same as the file for clarity. This function will be the entry point. You can use [Test Template](#test-template).
2. Add the declaration together with a Doxygen comment to `tests/functional_mock/lt_functional_mock_tests.h`.
3. Add the test to the `tests/functional_mock/CMakeLists.txt`.
    - Add the test name to the `LIBTROPIC_MOCK_TEST_LIST` (it must match the name of the entrypoint function).
4. Make sure your test works. If the test fails, you:
    - Did a mistake in the test. Fix it.
    - Found a bug. If you are certain it is a bug and not a problem in your test,
      [open an issue](https://github.com/tropicsquare/libtropic/issues/new). Thanks!

### Test Template
Change the lines marked with `TODO`.

```c
/**
 * @file TODO: FILL ME
 * @brief TODO: FILL ME
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_mock_helpers.h"
#include "lt_test_common.h"

int lt_test_mock_my_test(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_my_test()");
    LT_LOG_INFO("----------------------------------------------");

    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK, mock_init_communication(h, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    // TODO: Put the content of the test here.

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));

    return 0;
}
```