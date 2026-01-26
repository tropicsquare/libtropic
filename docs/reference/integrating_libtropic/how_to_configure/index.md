# How to Configure
Libtropic can be configured using the [Available CMake Options](#available-cmake-options) (let's say `LT_CFG_OPT`) in the following ways:

1. Via a command line when building the project:
```cmake
cmake -DLT_CFG_OPT=value ..
```
2. Using the CMake GUI. This makes the configuring process more user-friendly compared to the previous way. For more information, refer to the [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) documentation.
3. In your project's `CMakeLists.txt`:
```cmake
set(LT_CFG_OPT value)
```

## Available CMake Options

### `LT_BUILD_TESTS`
- boolean
- default value: `OFF`

[Functional tests](../../../for_contributors/tests/functional_tests.md) will be built as part of the Libtropic library.

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

### `LT_SILICON_REV`
- string
- default value: latest silicon revision available in the current Libtropic release

Silicon version (e.g. `"ACAB"`) of the currently used TROPIC01 has to be set in this option. It is needed for TROPIC01's firmware update and functional tests, as some behavior differs between the TROPIC01 revisions.

!!! question "What Is the Silicon Revision of My TROPIC01?"
    Refer to the dedicated section in the [FAQ](../../../faq.md#what-is-the-silicon-revision-of-my-tropic01).

!!! warning
    Because the implementation of Libtropic's FW update functions is chosen at compile-time based on `LT_SILICON_REV`, in one compiled instance of Libtropic, FW update can be done only with TROPIC01 of this silicon revision.
    !!! example
        I passed `-DLT_SILICON_REV=ACAB` to `cmake` during the build. I will be able to do FW updates with TROPIC01 chips that have silicon revision ACAB **only**. Updating a TROPIC01 chip with e.g. ABAB silicon revision will **not** work.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_SILICON_REV=` to `cmake`, which will invoke an error, but will print the available values.

### `LT_CPU_FW_UPDATE_DATA_VER`
- string
- default value: latest FW version available in the current Libtropic release

Defines the TROPIC01's RISC-V CPU FW version (e.g. `"1_0_1"`) to update to. It is used for compiling the correct FW update files for both the RISC-V CPU and SPECT. Available versions can be seen in the [compatibility table](https://github.com/tropicsquare/libtropic?tab=readme-ov-file#compatibility-with-tropic01-firmware-versions) in the repository's main `README.md`.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_CPU_FW_VERSION=` to `cmake`, which will invoke an error, but will print the available values.

!!! tip "See Current Configuration"
    Use `cmake -LAH | grep -B 1 LT_` to check current value of all Libtropic options.