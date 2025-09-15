# Adding a New Cryptographic Backend
As libtropic was designed with extensibility in mind, it is possible to add a new backend for the cryptographic operations.

## Requirements
The new cryptographic backend has to support the following schemes:

- **AES (GCM)**
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
!!! bug
    Is this all? What about the whole certificate chain verification?

## Guide
To add a new cryptographic backend (let's say `mycrypto`), [Create and Implement the Necessary Files](#create-and-implement-the-necessary-files) first and then [Edit the Main CMakeLists.txt](#edit-the-main-cmakeliststxt).

### Create and Implement the Necessary Files
1. Inside `hal/crypto/`, create a new directory called `mycrypto`.
2. Inside `hal/crypto/mycrypto/`, create 5 new source files:
    - `lt_crypto_mycrypto_aesgcm.c`,
    - `lt_crypto_mycrypto_ecdsa.c`,
    - `lt_crypto_mycrypto_ed25519.c`,
    - `lt_crypto_mycrypto_sha256.c`,
    - `lt_crypto_mycrypto_x25519.c`.
3. In each of the source files, implement all required functions - they are declared in respective headers inside the `src/` directory:
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_ed25519.h`: ED25519 functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    Look into each header - the exact purpose of every function is described in its comment. Copy the function declarations
    from the headers to the source files and implement the functions.
    !!! example
        To implement Curve25519 functions, I will copy declarations from the `lt_x25519.h` to `lt_crypto_mycrypto_x25519.c` and provide implementations.
        You can use existing ports inside `hal/crypto/` for inspiration.

### Edit the Main CMakeLists.txt
!!! bug
    We have options `LT_USE_TREZOR_CRYPTO` and `LT_CRYPTO_MBEDTLS`. We should unite the naming.

1. In the `Setup` section of the `CMakeLists.txt` file at the top, add a new option for the added cryptographic backend `mycrypto`:
```cmake
option(LT_CRYPTO_MYCRYPTO "Use mycrypto as a cryptography provider" OFF)
```
1. In the end of the `Setup` section, there are checks for definition of the cryptographic backend, because one of the backends has to be always defined. Add a check for the new option `LT_CRYPTO_MYCRYPTO`:
```cmake
# Check if cryptography provider is defined
# Check if cryptography provider is defined
if(
    (NOT LT_CRYPTO_<crypto_backend1>)  AND
    (NOT LT_CRYPTO_<crypto_backend2>)  AND
    (NOT LT_CRYPTO_MYCRYPTO)     # <-- YOUR NEW OPTION HERE
)
    message(FATAL_ERROR "No cryptography provider is defined.")
endif()
```
1. In the `Collect files` section, add the new source files to the compilation process. Use the following template, modify it accordingly, and paste it above the `# --- add new crypto sources above this line ---` comment:
```cmake
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_x25519.c
)
```
1. In the `Compile and link` section, compile and link dependencies to your new crypto backend:
```cmake
if(LT_CRYPTO_MYCRYPTO)
    add_subdirectory(vendor/mycrypto/ "mycrypto")
    target_compile_definitions(mycrypto PRIVATE <edit_me>) # Edit or remove.
    target_compile_definitions(tropic PRIVATE LT_CRYPTO_MYCRYPTO)
    target_link_libraries(tropic PRIVATE mycrypto)
    # Add more lines if necessary.
endif()
```