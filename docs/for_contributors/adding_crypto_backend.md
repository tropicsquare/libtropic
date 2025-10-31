# Adding a New Cryptographic Backend
Because Libtropic is designed to run on the Host MCU, it requires certain cryptographic capabilities - for example, to decrypt incoming L3 packets from TROPIC01. To enable this, Libtropic defines an interface that abstracts the functionality of a chosen cryptographic backend. This interface-based design makes it easy to integrate and support additional cryptographic backends in the future.

## Requirements
The new cryptographic backend has to support the following schemes:

- **AES-GCM**
    - encryption
    - decryption
- **ECDSA**
    - signing
- **ED25519**
    - signing
- **SHA256**
    - hashing
- **Curve25519**
    - multiplication on both arbitrary and base point

## Guide
To add a new cryptographic backend (let's say `mycrypto`):

1. [Create and Implement the HAL C Files](#create-and-implement-the-hal-c-files),
2. [Create and Implement the HAL CMakeLists.txt](#create-and-implement-the-hal-cmakeliststxt),
3. [Edit the Crypto CMakeLists.txt](#edit-the-crypto-cmakeliststxt).

After these steps, the sources and include directories of the new cryptographic backend should be available in consumer's `CMakeLists.txt` by calling:

1. `set(LT_CRYPTO_HAL "mycrypto")`,
2. `add_subdirectory("<path_to_libtropic>/hal/crypto/")`.

By doing this, the CMake variables `LT_CRYPTO_HAL_SRCS` and `LT_CRYPTO_HAL_INC_DIRS` will become available to the consumer.

### Create and Implement the HAL C Files
1. Inside `hal/crypto/`, create a new directory called `mycrypto`.
2. Inside `hal/crypto/mycrypto/`, create the following source files:
    - `lt_mycrypto_aesgcm.c`,
    - `lt_mycrypto_ecdsa.c`,
    - `lt_mycrypto_ed25519.c`,
    - `lt_mycrypto_sha256.c`,
    - `lt_mycrypto_hmac_sha256.c`,
    - `lt_mycrypto_x25519.c`.
3. In each of the source files, implement all required functions - they are declared in respective headers inside the `src/` directory:
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_ecdsa.h`: ECDSA functions,
    - `lt_ed25519.h`: ED25519 functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_hmac_sha256.h`: HMAC SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    Look into each header - the exact purpose of every function is described in its comment. Copy the function declarations
    from the headers to the source files and implement the functions.
    !!! example
        To implement Curve25519 functions, copy declarations from `lt_x25519.h` to `lt_mycrypto_x25519.c` and provide implementations.
        You can use existing ports inside `hal/crypto/` for inspiration.

4. Inside `hal/crypto/mycrypto/`, create a file `lt_mycrypto.h`. This file should declare the **context structure** for `mycrypto`:
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

    !!! warning "Important"
        This structure has to include all contexts the functions in the crypto HAL might need. This structure will then be defined in the user's application and assigned to `lt_handle_t`'s `crypto_ctx` void pointer - see [TODO](todo.md) for more information about this.

1. Additionally, other source files and headers can be created for the needs of the implementation.

### Create and Implement the HAL CMakeLists.txt
Inside `hal/crypto/mycrypto/`, create a `CMakeLists.txt` with the following contents:
```cmake
set(LT_CRYPTO_HAL_SRCS
    # Source files of the HAL
)

set(LT_CRYPTO_HAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Other include directories if needed
)

# export generic names for parent to consume
set(LT_CRYPTO_HAL_SRCS ${LT_CRYPTO_HAL_SRCS} PARENT_SCOPE)
set(LT_CRYPTO_HAL_INC_DIRS ${LT_CRYPTO_HAL_INC_DIRS} PARENT_SCOPE)
```

### Edit the Crypto CMakeLists.txt
In `hal/crypto/`, there is a `CMakeLists.txt` which handles selecting one of the existing crypto HALs - at the top of the file, add the new string value `"mycrypto"` to `LT_CRYPTO_HAL` values, so `mycrypto` HAL can be selected by the consumer:
```cmake

set(LT_CRYPTO_HAL "" CACHE STRING "Choose HAL files of the specified cryptography provider")
# Add the new string below ---------------------------------------
#                                                                |
#                                                                v
set_property(CACHE LT_CRYPTO_HAL PROPERTY STRINGS "cryptoXY" "mycrypto")
```