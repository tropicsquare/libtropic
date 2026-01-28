# MbedTLS
Currently, we support the [MbedTLS v4.0.0](https://github.com/Mbed-TLS/mbedtls/tree/v4.0.0) and use the [PSA Crypto API](https://mbed-tls.readthedocs.io/en/latest/getting_started/psa/), which differs from the legacy MbedTLS API in several ways:

1. **Key Management**: Keys are imported into PSA key store and referenced by key ID.
2. **Context Types**: PSA uses opaque context structures (`psa_hash_operation_t`, etc.).
3. **Error Handling**: Uses `psa_status_t` return codes.
4. **API Design**: Higher-level, more abstract operations (e.g., `psa_aead_encrypt` vs manual GCM operations).

CAL files of this port are available in the `libtropic/cal/mbedtls_v4/` directory.

## Initialization and Deinitialization
Libtropic does not handle initialization and deinitialization of MbedTLS, this is the user's responsibility. Specifically, it is assumed that:
    
1. [psa_crypto_init()](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__initialization/#_CPPv415psa_crypto_initv) is called before the instance of the CAL is handed to Libtropic's `lt_init()`. See the [Libtropic Bare-Bone Example](../../reference/integrating_libtropic/how_to_use/index.md#libtropic-bare-bone-example) for more information about the CAL instance.
2. [mbedtls_psa_crypto_free()](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__extra_8h/#_CPPv423mbedtls_psa_crypto_freev) is called in the user's application cleanup logic. Although freeing the MbedTLS's resources is not required by Libtropic, it **cannot** be called sooner than the last call of Libtropic's `lt_deinit()`, otherwise all Secure Channel Session related commands will return with errors.

## Configuration
PSA Crypto must be configured with the following features:

- `PSA_WANT_ALG_GCM` - AES-GCM authenticated encryption.
- `PSA_WANT_ALG_SHA_256` - SHA-256 hashing.
- `PSA_WANT_ALG_HMAC` - HMAC operations.
- `PSA_WANT_ALG_ECDH` - X25519 key agreement.
- `PSA_WANT_ECC_MONTGOMERY_255` - Curve25519 support.
- `PSA_WANT_KEY_TYPE_AES` - AES key support.
- `PSA_WANT_KEY_TYPE_HMAC` - HMAC key support.
- `PSA_WANT_KEY_TYPE_ECC_KEY_PAIR` - ECC key pair support.
- `PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY` - ECC public key support.

## Implementation Notes
### Including PSA Crypto Headers
The MbedTLS headers contain some redundant declarations, see [this issue on GitHub](https://github.com/Mbed-TLS/mbedtls/issues/10376). As the errors are present in headers, not in the implementation files (.c), our strict compilation flags catch those problems, even though we restrict compilation with strict flags only to our own code. To keep ability to use this flag without triggering compilation errors due problems with PSA Crypto, we have to wrap `#include` like following:
```c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
```

The pragmas will disable this flag only for the PSA Crypto code.

### Macros
MbedTLS does not define macros for all sizes we need, sometimes they define macros only inside their implementation files ad-hoc. As such, we opted to use some of our macros.