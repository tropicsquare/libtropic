# Functional Mock Tests
Functional Mock Tests run against the *mock HAL* (not a real target or model). They are used to verify behavior that is difficult or impossible to reproduce on a real device (for example, rare hardware error conditions or unusual timing). These tests live in `tests/functional_mock` and require you to explicitly mock the data that would appear on the MISO line.

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
    If possible and feasible, prefer implementing classic functional tests, as they can run on all platforms. Use functional mock tests only to cover behavior that cannot be easily tested on real targets (for example, extremely rare hardware errors) or do not use any communication at all (typically parameter checking tests).

### Basic concepts
You must understand three TROPIC01 protocol layers when writing these tests:

- L1 — SPI transfer: a single call to `lt_port_spi_transfer`. Any number of bytes can be sent/received in one transfer.
- L2 — Frame: started by CSN falling edge and ended by CSN rising edge. A frame may be delivered in one L1 transfer or across multiple L1 transfers (sending byte-by-byte is allowed while CSN is low).
- L3 — Packet: a logical packet carried inside L2 frames' REQ_DATA/RSP_DATA. L3 packets can be split across several L2 frames.

Key practical points and rules

- SPI always exchanges bytes: every transfer swaps master's and slave's data. For the mock HAL you should *mock the MISO side* (what the chip would send back). MOSI (what host — Libtropic — sends) is ignored by the current mock implementation.
- We distinguish a host "read" vs "write" only by the packet `REQ_ID`. Frames with `REQ_ID == Get_Response (0xAA)` are treated as reads (the host expects complete frame on MISO). All other REQ_IDs are requests; the chip normally answers those with a single `CHIP_STATUS` byte.
- Mocked data are queued as L2 frames — not as individual bytes. After a CSN rising edge the next queued mocked frame becomes the source for subsequent MISO bytes.
- If your code attempts to read more MISO bytes than you queued, the mock HAL returns zeros (up to `LT_L1_MAX_LENGTH`). This keeps tests simpler but means you should queue the bytes the code will actually read for clarity.
- If there is no mocked frame in the queue when the test pulls CSN low, the mock HAL reports an error. Any SPI transfer while CSN is high is also an error.

Two common mocked frame shapes

- Response frame (for `REQ_ID == Get_Response`, 0xAA): MISO contains
    `CHIP_STATUS`, `STATUS`, `RSP_LEN`, `RSP_DATA`, `RSP_CRC`.
- Request acknowledgement frame (for other `REQ_ID`s): MISO contains only `CHIP_STATUS`.

Notes about lengths and CRC

- Do not assume `sizeof()` matches the transmitted length — some reply structures are overlayed or have variable-length fields. Use the helper `calc_mocked_resp_len()` (found in the mock helpers) to produce correct mocked lengths including CRC.
- The CRC bytes may not always sit in a named `crc` field in the C struct; if the data are shorter the CRC can appear earlier in the layout. Use the `add_resp_crc()` helper or compute the CRC manually when constructing the frame bytes.

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