# Functional Mock Tests
Functional Mock Tests are tests run against the mock HAL (not against any real target, including the model). As such, all communication must be mocked. They supplement the classic [functional tests](./functional_tests.md) and are intended to test behavior that is difficult to trigger on real targets.

## Compiling and Running Tests
These tests are compiled standalone in the `tests/functional_mock` directory. The tests can be compiled and run as follows:

```shell
cd tests/functional_mock
mkdir -p build
cd build
cmake ..
make
ctest -V
```

!!! info "Strict Compilation"
    Strict compilation flags are applied by default. It is strongly recommended to develop tests with strict compilation flags enabled. You can disable the flags by passing `-DLT_STRICT_COMPILATION=0` to `cmake`.

## Adding a New Test

!!! important
    If possible and feasible, prefer implementing classic functional tests, as they can run on all platforms. Use functional mock tests only to cover behavior that cannot be easily tested on real targets (for example, extremely rare hardware errors).

### Basic Concepts
To write these tests, you need to understand TROPIC01's communication protocol at a low level. Here are the units and layers of communication:

- *One unit of communication on L1 = SPI transfer*, represented by `lt_port_spi_transfer`. One or more bytes can be transferred in a single transfer.
- *One unit of communication on L2 = frame*. A frame is initiated by a falling edge on CSN and terminated by a rising edge. A frame can be transferred in a single L1 transfer or across multiple L1 transfers. As long as CSN is low and part of the frame remains, the host can perform SPI transfers. Consequently, a frame can be read byte-by-byte.
- *One unit of communication on L3 = packet*. A packet is encapsulated in an L2 frame's REQ_DATA/RSP_DATA (similar to ISO/OSI encapsulation). Because L2 frames have a limited size, an L3 packet may need to be split into multiple frames. From the HAL perspective this is still part of the frame.

Additional key points:

- We mock incoming data — the data that would appear on the MISO line.
- There is no distinction between "writing" and "reading" at the SPI level. Everything is an SPI transaction where the contents of the buffers are swapped. We distinguish "reads" from "writes" only by the `REQ_ID`: "reads" have `REQ_ID == Get_Response (0xAA)`, and everything else is treated as a "write". Therefore you must also mock incoming data for "writes" (currently, this is only a single `CHIP_STATUS` byte). Other bytes may be undefined, invalid, or zero.
- In principle, the lengths of outgoing and incoming data must match due to the nature of SPI. However, to simplify tests the mock HAL does not require filling the remainder of the buffer after `CHIP_STATUS`. If Libtropic attempts to read incoming data (MISO) beyond what the test queued, the mock HAL returns zeros (up to `LT_L1_MAX_LENGTH`). A logging macro can be enabled to simplify test design.
- Data sent to the mocked HAL (outgoing data — "MOSI") are ignored by the current implementation to keep test complexity manageable.
- Data are mocked by "frames" (see next point), not by individual bytes. After a CSN rising edge, the next mocked "frame" is taken from the internal mock queue.
- There are two types of incoming data (from chip — on MISO):
  - *Answer to response frames* (answer to frames which have `REQ_ID == Get_Response`) have following structure: `CHIP_STATUS`, `STATUS`, `RSP_LEN`, `RSP_DATA`, `RSP_CRC`.
  - *Answer to request frames* (answer to frames with all other `REQ_ID`s) contain only `CHIP_STATUS`. As such, it is correct to complete transaction after receiving only this byte in this case, although it is not a complete "frame".
  - For simplicity, we refer to both types as "frames" => even single mocked `CHIP_STATUS` byte is a frame from the mock HAL perspective, although it does not contain other fields.
- If no mocked transaction is available in the queue, attempting to start a transaction by pulling CSN low results in an error.
- Any SPI transfer attempted while CSN is high results in an error, since transfers are only allowed while CSN is low (after CSN falling edge).

There are also some quirks to be aware of:

- When determining the size of a mocked L2 request, do not rely on `sizeof()` because some structures are reused for multiple argument types. For example, `lt_l2_get_info_rsp_t` is not always `sizeof(lt_l2_get_info_rsp_t)` bytes long. You must either determine the size manually or use `calc_mocked_resp_len()`, which calculates the correct length for you (including the CRC length).
- Another side effect is that the CRC is not always stored in the `crc` field; it can appear in the previous fields if the data are shorter.

### Creating the Test
To add a new test, do the following:

1. Write the test. Add a new file to `tests/functional_mock` named `lt_test_mock_<name>.c`. The test must contain an entry-point function with the same name as the file for clarity; this function will be the entry point. You can use the [Test Template](#test-template).
2. Add the declaration and a Doxygen comment to `tests/functional_mock/lt_functional_mock_tests.h`.
3. Add the test to `tests/functional_mock/CMakeLists.txt`:
     - Add the test name to the `LIBTROPIC_MOCK_TEST_LIST` (it must match the name of the entry-point function).
4. Make sure your test passes. If it fails, either you made a mistake in the test (fix it) or you found a bug. If you are certain it is a bug and not an issue with your test, [open an issue](https://github.com/tropicsquare/libtropic/issues/new).

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