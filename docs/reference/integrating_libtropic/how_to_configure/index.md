# How to Configure
Libtropic can be configured using the [Available CMake Options](#available-cmake-options) (let's say `LT_CFG_OPT`) in the following ways:

!!! example "Configuring Libtropic"
    1. Via a command line when building the project:
    ```cmake { .copy }
    cmake -DLT_CFG_OPT=value ..
    ```
    2. Using the CMake GUI. This makes the configuring process more user-friendly compared to the previous way. For more information, refer to the [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) documentation.
    3. In your project's `CMakeLists.txt`:
    ```cmake  { .copy }
    set(LT_CFG_OPT value)
    ```

## Available CMake Options

### `LT_HELPERS`
- boolean
- default value: `ON`

Compile the [helper functions](../../../doxygen/build/html/group__libtropic__API__helpers.html).

### `LT_LOG_LVL`
- string
- default value: `"None"`
- default value if `LT_BUILD_EXAMPLES` or `LT_BUILD_TESTS` are set: `"Info"`

Specifies the log level. See [Logging](../../logging.md) for more information.

### `LT_USE_INT_PIN`
- boolean
- default value: `OFF`

Use TROPIC01's interrupt pin while waiting for TROPIC01's response.

### `LT_SEPARATE_L3_BUFF`
- boolean
- default value: `OFF`

Buffer used for sending and receiving L3 Layer data will be defined by the user. The user then has to pass a pointer to their buffer into the instance of `lt_handle_t`:
```c
#include "libtropic_common.h"

lt_handle_t handle;
uint8_t user_l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16)));

handle.l3.buff = user_l3_buffer;
handle.l3.buff_len = sizeof(user_l3_buffer);
```

### `LT_PRINT_SPI_DATA`
- boolean
- default value: `OFF`

Log SPI communication using `printf`. Handy to debug low level communication.

### `LT_SILICON_REV_ABAB`
- boolean
- default value: `OFF`

Enable FW update support for TROPIC01 with the ABAB silicon revision (engineering samples). By default, Libtropic supports FW update of TROPIC01 with the ACAB silicon revision and newer (e.g. BDBB). Only the FW update API differs between ABAB and the newer silicon revisions; the rest of Libtropic works with all silicon revisions regardless of this option.

!!! question "What Is the Silicon Revision of My TROPIC01?"
    Refer to the dedicated section in the [FAQ](../../../faq.md#what-is-the-silicon-revision-of-my-tropic01).

!!! warning
    Because the implementation of Libtropic's FW update functions is chosen at compile-time based on `LT_SILICON_REV_ABAB`, in one compiled instance of Libtropic, FW update can be done either only with TROPIC01 of the ABAB silicon revision (`ON`), or only with TROPIC01 of the ACAB silicon revision and newer (`OFF`).
    !!! example
        I did not pass `LT_SILICON_REV_ABAB` to `cmake` during the build. I will be able to do FW updates with TROPIC01 chips that have silicon revision ACAB or BDBB. Updating a TROPIC01 chip with ABAB silicon revision will **not** work.

!!! note "Migration from `LT_SILICON_REV`"
    The `LT_SILICON_REV` option was removed. Replace `-DLT_SILICON_REV=ABAB` with `-DLT_SILICON_REV_ABAB=ON` and drop `-DLT_SILICON_REV=ACAB`. When building Libtropic without CMake, define the `LT_SILICON_REV_ABAB` macro to `1` instead of the former `LT_SILICON_REV_ABAB`/`LT_SILICON_REV_ACAB` macros (it defaults to `0` if not defined).

### `LT_CPU_FW_UPDATE_DATA_VER`
- string
- default value: latest FW version available in the current Libtropic release

Defines the TROPIC01's RISC-V CPU FW version (e.g. `"1_0_1"`) to update to. It is used for compiling the correct FW update files for both the RISC-V CPU and SPECT. Available versions can be seen in the [compatibility table](https://github.com/tropicsquare/libtropic?tab=readme-ov-file#firmware-compatibility) in the repository's main `README.md`.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_CPU_FW_UPDATE_DATA_VER=` to `cmake`, which will invoke an error, but will print the available values.

!!! tip "See Current Configuration"
    Use `cmake -LAH | grep -B 1 LT_` to check current value of all Libtropic options.

### `LT_CRC_ERR_RETRY_ATTEMPTS`
- integer (non-negative)
- default value: 3

Defines how many times Libtropic retries communication after either of the following CRC-related errors:

- An L2 Response frame with `STATUS=CRC_ERR` (`LT_L2_CRC_ERR`).
- An L2 Response frame with an invalid CRC in the `RSP_CRC` field (`LT_L2_IN_CRC_ERR`).

Retry behavior:

- `LT_L2_CRC_ERR`: Libtropic resends the original L2 Request.
- `LT_L2_IN_CRC_ERR`: Libtropic sends a Resend Request (`Resend_Req`).

Both error types use the same retry counter.

!!! example
    If Libtropic receives a frame with `STATUS=CRC_ERR` and then two L2 Response frames with invalid CRC during retries, the retry counter is exhausted for that communication (a single L2 API call or a single L3 chunk; see below).

!!! important
    This retry counter is independent of CRC diagnostic counters (see [FAQ](../../../faq.md)). CRC diagnostic counters track the total number of CRC errors since initialization (`lt_init`).

The counter scope is always one L2 frame operation:

- It starts at zero for each invocation of an L2 API function (for example, `lt_get_info_chip_id`).

    !!! example
        Even if the first call to `lt_get_info_chip_id` exhausts all retry attempts, the next call to `lt_get_info_chip_id` (or any other L2 API function) starts with a fresh retry counter.

- It also starts at zero for each L3 Command/Result chunk in L3 API functions (for example, `lt_ping`).

    !!! example
        Even if two errors occur while transmitting the first chunk, the second chunk starts with a fresh retry counter.

Set this parameter to zero to disable retries.

### `LT_L1_READ_MAX_TRIES`
- integer (positive)
- default value: 50

Sets how many times to try L1 transfer (if the chip is busy) before failing.

See also [`LT_L1_READ_RETRY_DELAY_MS`](#lt_l1_read_retry_delay_ms).

!!! tip "Tip: Use interrupt pin"
    It is better to use the interrupt pin functionality of TROPIC01 if you need to shorten reaction time. See [`LT_USE_INT_PIN`](#lt_use_int_pin) parameter.

### `LT_L1_READ_RETRY_DELAY_MS`
- integer (positive)
- default value: 25 (ms)

Sets how long to wait in milliseconds before retrying L1 transfer (if the chip is busy).

See also [`LT_L1_READ_MAX_TRIES`](#lt_l1_read_max_tries).

!!! tip "Tip: Use interrupt pin"
    It is better to use the interrupt pin functionality of TROPIC01 if you need to shorten reaction time. See [`LT_USE_INT_PIN`](#lt_use_int_pin) parameter.

### `LT_L1_SPI_TIMEOUT_MS`
- integer (non-negative)
- default value: 70 (ms)

Sets timeout on SPI transfer. If the transfer operation times out, `LT_HAL_ERROR` is returned by Libtropic API functions.

!!! info
    Used only by certain HALs, refer to the source code of the HAL you use.

The default value was tested with supported HALs and normally does not have to be changed. Lowering the value increases a risk of communication errors.

!!! warning "Warning: value '0'"
    This parameter also accepts the value of zero, however, behavior is HAL-dependent. For example, in STM32 HAL the value of '0' disables polling and the function returns immediately. Check behavior of HAL you use carefully.

### `LT_L1_INT_TIMEOUT_MS`
- integer (positive)
- default value: 200 (ms)

Sets maximum waiting time for interrupt from TROPIC01's GPO pin. If the operation times out, `LT_L1_INT_TIMEOUT` is returned by Libtropic API functions.

!!! info
    Used only by certain HALs when [`LT_USE_INT_PIN`](#lt_use_int_pin) is enabled. Refer to the source code of the HAL you use.

The default value was tested with supported HALs and normally does not have to be changed. It is recommended to keep this value equal or higher than the longest time a TROPIC01 operation can take. Refer to the [TROPIC01 Datasheet](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#documentation).
