# Reversible Examples

For better orientation, we highlight specific properties of each example, namely:

- Reversibility,
- Model compatibility,
- Level of complexity (🐣 -> 🐤 -> 🐓).

The source code for the examples can be found in the `examples/` directory and also within this document inside "Source code" boxes.

!!! tip
    You can copy the example code using the :material-content-copy: icon located in the top-right corner of each source code listing.

## lt_ex_hello_world.c
- ✅ Reversible
- ✅ Compatible with model
- 🐣 Level: Basic

This example demonstrates the basic libtropic API and can be used to verify that the chip works correctly.
In this example, you will learn about the following functions:

- `lt_init()`: function used to initialize context for communication with the TROPIC01,
- `lt_verify_chip_and_start_secure_session()`: helper function to start Secure Session and allow L3 communication,
- `lt_ping()`: L3 command to verify communication with the TROPIC01,
- `lt_session_abort()`: L3 command to abort Secure Session,
- `lt_deinit()`: function used to deinitialize context.

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_hello_world.c"
    ```

## lt_ex_show_chip_id_and_fwver.c
- ✅ Reversible
- ✅ Compatible with model
- 🐣 Level: Basic

Demonstrates how to read and display the chip’s unique ID and firmware version information (bootloader, application and SPECT firmware versions). You will learn about following functions:

- `lt_reboot()`: L2 request to reboot to either application or maintenance mode,
- `lt_get_info_riscv_fw_ver()`, `lt_get_info_spect_fw_ver()`: L2 requests to read CPU and SPECT firmware versions,
- `lt_get_info_chip_id()`: L2 request to read chip identification (e.g., serial number),

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_show_chip_id_and_fwver.c"
    ```

## lt_ex_hello_world_separate_API.c
- ✅ Reversible
- ✅ Compatible with model
- 🐤 Level: Moderate

Functionally similar to `lt_ex_hello_world.c`, but it uses distinct API calls for incoming and outgoing data. This approach is useful for secure, tunneled communication, such as during chip provisioning in a factory.

You will learn about low-level API functions used to process outgoing and incoming data. For example:

- `lt_out__session_start()`: prepare Handshake_Req L2 request (for Secure Session establishment),
- `lt_l2_send()`: send L2 request,
- `lt_l2_receive()`: receive L2 response.
- `lt_in__session_start()`: process L2 response to the Handshake_Req,

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_hello_world_separate_API.c"
    ```

## lt_ex_macandd.c
- ✅ Reversible
- ✅ Compatible with model
- 🐓 Level: Advanced

This example illustrates the MAC-and-destroy feature. You will learn about the following functions:

- `lt_hmac_sha256()`: function to compute HMAC based on SHA256,
- `lt_mac_and_destroy()`: L3 command to process MAC-and-Destroy operation,
- `lt_r_mem_data_erase()`: L3 command to erase R-memory data slot,
- `lt_r_mem_data_write()`: L3 command to write R-memory data slot,
- `lt_r_mem_data_read()`: L3 command to read R-memory data slot,
- `lt_random_bytes()`: function to generate random number using platform's RNG (not TROPIC01's),

In this example, we also define two functions to implement PIN verification functionality: `lt_PIN_set()` and `lt_PIN_check()`. You can use these functions as inspiration for your project.

You can find more information in the application note and code comments.

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_macandd.c"
    ```