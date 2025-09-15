# Adding a New Cryptographic Backend
As the libtropic was designed with extensibility in mind, it is possible to add a new backend
for cryptographic operations.

The new backend has to support operations with:

- AES (GCM)
    - encryption
    - decryption
- ED25519
    - signing
- SHA256
    - hashing
- Curve25519
    - multiplication on both arbitrary and base point

## Guide
To add a new backend (let's say mycrypto), you need to:

1. Create a new directory in `hal/crypto` called `mycrypto`.
2. Create 4 source files in this directory:
    - `lt_crypto_mycrypto_aesgcm.c`
    - `lt_crypto_mycrypto_ed25519.c`
    - `lt_crypto_mycrypto_sha256.c`
    - `lt_crypto_mycrypto_x25519.c`
3. In each of the source files, implement all required functions. The functions are declared in respective headers in `src/`:
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_ed25519.h`: ED25519 functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    - Look into each of the headers. Purpose of every function is described in its comment. Copy function declarations
    from the headers to the sources and implement the functions. For exmple, to implement Curve25519 functions,
    I will copy declarations from the `lt_x25519.h` to `lt_crypto_mycrypto_x25519.c` and provide implementations.
    You can use existing ports for inspiration.

4. Edit main CMakeLists --> see following section.
5. Done. The new crypto backend can be enabled by passing `-DLT_CRYPTO_MYCRYPTO` to the CMake.

### Editing CMakeLists.txt
In the Setup section, add new option:
```cmake
option(LT_CRYPTO_MYCRYPTO "Use mycrypto as a cryptography provider" OFF)
```

At the bottom of the Setup section, there are checks for definition of cryptographic backend. Add your new backend to the checks:

```cmake
# Check if cryptography provider is defined
# Check if cryptography provider is defined
if(
    (NOT BUILD_DOCS)         AND
    (NOT USE_TREZOR_CRYPTO)  AND
    (NOT LT_CRYPTO_MBEDTLS)  AND
    (NOT LT_CRYPTO_MYCRYPTO)     # <-- YOUR NEW OPTION HERE
)
    message(FATAL_ERROR "No cryptography provider is defined.")
endif()
```

In the Collect files section, add the new implementation files to compilation. Use this template,
modify it accordingly, and paste it above the `add new crypto sources above this line` comment:

```cmake
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_x25519.c
)
```

At last, in the Compile and link section, compile and link dependencies to your new crypto backend:

```cmake
if(LT_CRYPTO_MYCRYPTO)
    target_compile_definitions(tropic PRIVATE LT_CRYPTO_MYCRYPTO)
    add_subdirectory(<edit me>) # Edit or remove.
    target_link_libraries(tropic PRIVATE <edit me>) # Edit or remove.
    # Add more lines as neccessary.
endif()
```