# Adding a New Cryptographic Functionality Provider
Because Libtropic is designed to run on the Host MCU, it requires certain cryptographic functionality — for example, to decrypt incoming L3 packets from TROPIC01. To enable this, Libtropic defines a Crypto Abstraction Layer (CAL) to abstract the functionality required from a chosen Cryptographic Functionality Provider (CFP). CFP can implement the function either purely in software (using a cryptographic library) or in hardware (using a cryptographic hardware accelerator) or by combining both (e.g., implement all operations in MbedTLS except of the AES-GCM, which will be handled by a STM32's peripheral). This interface-based design makes it easy to integrate and support additional CFPs in the future.

## Requirements
The new CFP has to support the following schemes:

- **AES-GCM**
    - encryption
    - decryption
- **SHA256**
    - hashing
- **Curve25519**
    - multiplication on both arbitrary and base point

## Guide
To add support for a new CFP (let's say `mycrypto`):

1. [Create and Implement the CAL C Files](#create-and-implement-the-cal-c-files),
2. [Create and Implement the CAL CMakeLists.txt](#create-and-implement-the-cal-cmakeliststxt),
3. [Provide Some Information About the CAL](#provide-some-information-about-the-cal).

!!! tip "Get Inspired by Existing CALs"
    For inspiration, see the existing CALs inside `cal/`.

After these steps, the sources and include directories of the new CAL should be available in consumer's `CMakeLists.txt` by calling:
```cmake
add_subdirectory("<path_to_libtropic>/cal/mycrypto")
```

By doing this, the CMake variables `LT_CAL_SRCS` and `LT_CAL_INC_DIRS` will become available to the consumer.

### Create and Implement the CAL C Files
1. Inside `cal/`, create a new directory called `mycrypto`.
2. Inside `cal/mycrypto/`, create the following source files:
    - `lt_mycrypto_common.c`
    - `lt_mycrypto_aesgcm.c`,
    - `lt_mycrypto_sha256.c`,
    - `lt_mycrypto_hmac_sha256.c`,
    - `lt_mycrypto_x25519.c`.
3. In each of the source files, implement all required functions — they are declared in the respective headers inside the `libtropic/src/` directory:
    - `lt_crypto_common.h`: Common CAL functions,
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_hmac_sha256.h`: HMAC SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    Look into each header — the exact purpose of every function is described in its comment. Copy the function declarations
    from the headers to the source files and implement the functions.
    !!! example
        To implement Curve25519 functions, copy declarations from `lt_x25519.h` to `lt_mycrypto_x25519.c` and provide implementations.

4. Inside `cal/mycrypto/`, create a file `libtropic_mycrypto.h`. This file should declare the **context structure** for `mycrypto`:
```c
typedef struct lt_ctx_mycrypto_t {
    /** @private @brief AES-GCM context for encryption. */
    // TODO
    /** @private @brief AES-GCM context for decryption. */
    // TODO
    /** @private @brief SHA-256 context. */
    // TODO
} lt_ctx_mycrypto_t;
```

    !!! question "Which Contexts Are Needed?"
        This structure must include all contexts the functions in the CAL might need. The structure will be defined in the user's application and assigned to the `crypto_ctx` void pointer in the `lt_handle_t` — see the [Libtropic Bare-Bone Example](../reference/integrating_libtropic/how_to_use/index.md#libtropic-bare-bone-example) for more information.

5. Additionally, other source files and headers can be created for the needs of the implementation.

### Create and Implement the CAL CMakeLists.txt
Inside `cal/mycrypto/`, create a `CMakeLists.txt` with the following contents:
```cmake
set(LT_CAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mycrypto_common.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mycrypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mycrypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mycrypto_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mycrypto_x25519.c
    # Other source files if needed
)

set(LT_CAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Other include directories if needed
)

# export generic names for parent to consume
set(LT_CAL_SRCS ${LT_CAL_SRCS} PARENT_SCOPE)
set(LT_CAL_INC_DIRS ${LT_CAL_INC_DIRS} PARENT_SCOPE)
```

### Provide Some Information About the CAL
All currently supported CFPs are listed in the [Supported Cryptographic Functionality Providers](../compatibility/cfps/index.md) section. Add the new CFP there and provide some information about it and the CAL (see other sections for inspiration).