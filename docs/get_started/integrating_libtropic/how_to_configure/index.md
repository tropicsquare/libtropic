# How To Configure
Libtropic can be configured using CMake in the following ways:

1. Via a command line when building the project:
```cmake
cmake -D<config_option_name>=value ..
```
Where `<config_option_name>` can be substituted with one of the [Available CMake Options](#available-cmake-options). Multiple options can be passed to `cmake`.

2. CMake GUI. This makes the configuring process more user-friendly. For more information, refer to the [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) documentation.

## Available CMake Options

### `LT_BUILD_EXAMPLES`
- boolean
- default value: `OFF`

[Examples](../../examples/index.md) will be built as part of the Libtropic library.

### `LT_BUILD_TESTS`
- boolean
- default value: `OFF`

[Functional tests](../../../for_contributors/functional_tests.md) will be built as part of the Libtropic library.

### `LT_SH0_KEYS`
- string
- default value: `"prod0"`

Programs, compiled when `LT_BUILD_EXAMPLES` or `LT_BUILD_TESTS` are set, depend on the [Default Pairing Keys for a Secure Channel Handshake](../../default_pairing_keys.md). Keys that will be used in these programs can be changed by setting `LT_SH0_KEYS` to one of the available values.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_SH0_KEYS=` to `cmake`, which will invoke an error, but will print the available values.

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

Needed for TROPIC01's firmware update and functional tests, as some behavior differs between the TROPIC01 revisions.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_SILICON_REV=` to `cmake`, which will invoke an error, but will print the available values.

### `LT_CPU_FW_VERSION`
- string
- default value: latest FW version available in the current Libtropic release

Used for compiling the correct FW update files. Available versions can be seen in the compatibility table in the repository's main `README.md`.

!!! tip "See Available Values When Using CMake CLI"
    Pass `-DLT_CPU_FW_VERSION=` to `cmake`, which will invoke an error, but will print the available values.

!!! tip "See Current Configuration"
    Use `cmake -LAH | grep -B 1 LT_` to check current value of all Libtropic options.