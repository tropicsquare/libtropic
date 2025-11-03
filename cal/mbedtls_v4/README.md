# MbedTLS PSA Crypto Backend for Libtropic

This directory contains the MbedTLS 4.0 PSA Crypto API backend implementation for Libtropic.

## Overview

This implementation provides the cryptographic HAL layer using the **PSA Cryptography API** from MbedTLS 4.0. The PSA (Platform Security Architecture) Crypto API is the modern, standardized interface for cryptographic operations in MbedTLS 4.x.

## Requirements

This implementation requires **MbedTLS 4.0.0** or later with PSA Crypto API enabled. The following PSA features must be configured in `tf-psa-crypto/include/psa/crypto_config.h`:

- `PSA_WANT_ALG_GCM` - AES-GCM authenticated encryption
- `PSA_WANT_ALG_SHA_256` - SHA-256 hashing
- `PSA_WANT_ALG_HMAC` - HMAC operations
- `PSA_WANT_ALG_ECDSA` - ECDSA signature operations
- `PSA_WANT_ALG_PURE_EDDSA` - Ed25519 signature operations
- `PSA_WANT_ALG_ECDH` - X25519 key agreement
- `PSA_WANT_ECC_SECP_R1_256` - NIST P-256 curve support
- `PSA_WANT_ECC_TWISTED_EDWARDS_255` - Ed25519 curve support
- `PSA_WANT_ECC_MONTGOMERY_255` - Curve25519 support
- `PSA_WANT_KEY_TYPE_AES` - AES key support
- `PSA_WANT_KEY_TYPE_HMAC` - HMAC key support
- `PSA_WANT_KEY_TYPE_ECC_KEY_PAIR` - ECC key pair support
- `PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY` - ECC public key support

## Implementation Notes

### PSA Crypto API

All implementations use the **PSA Cryptography API** instead of legacy MbedTLS internal functions. This provides:
- **Standardized interface**: Consistent with ARM PSA specifications
- **Better security**: Keys are managed through PSA key store with proper access control
- **Future compatibility**: PSA is the primary API for MbedTLS 4.x and beyond
- **Simplified code**: Higher-level abstractions for cryptographic operations

### AES-GCM
Uses PSA AEAD (Authenticated Encryption with Associated Data) operations:
- `psa_aead_encrypt()` for encryption with authentication tag generation
- `psa_aead_decrypt()` for decryption with authentication tag verification
- Keys are imported into PSA key store and managed through key IDs

### ECDSA
- Verifies signatures on the NIST P-256 (secp256r1) curve using `psa_verify_hash()`
- Public keys are imported as 64 bytes (32 bytes X, 32 bytes Y coordinates).
  As the format requires that public keys also contain `0x4` prefix, we add it inside the HAL function.
- Signatures are 64 bytes (32 bytes R, 32 bytes S)
- Message is hashed with SHA-256 before verification

### Ed25519
- As MbedTLS does not support EdDSA yet (no support for Edwards curve), we use a fallback
  implementation provided by a [small library by Orson Peters](https://github.com/orlp/ed25519).

### SHA-256
Uses PSA hash operations with streaming interface:
1. Initialize with `psa_hash_operation_init()`
2. Start hashing with `psa_hash_setup()`
3. Add data with `psa_hash_update()` (can be called multiple times)
4. Finalize with `psa_hash_finish()`

### HMAC-SHA256
Uses `psa_mac_compute()` for single-call HMAC computation with PSA key management.

### X25519
- `lt_X25519()`: Uses `psa_raw_key_agreement()` for ECDH shared secret computation
- `lt_X25519_scalarmult()`: Uses `psa_export_public_key()` to derive public key from private key

### PSA Crypto Initialization

The PSA Crypto subsystem is automatically initialized on first use. In each module, we call
`lt_mbedtls_ensure_psa_crypto_init`, which checks whether the subsystem was initialized and if not,
it initializes the subsystem. Initialization state is kept in the `lt_mbedtls_psa_crypto_initialized`
global variable.

### Including PSA Crypto Headers
The MbedTLS headers contain some redundant declarations, see [this issue on GitHub](https://github.com/Mbed-TLS/mbedtls/issues/10376).
As the errors are present in headers, not in the implementation files (.c), our strict compilation flags catch
those problems, even though we restrict compilation with strict flags only to our own code. To keep
ability to use this flag without triggering compilation errors due problems with PSA Crypto, we have to wrap `#include` like following:

```c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
```

The pragmas will disable this flag only for the PSA Crypto code.

### Macros
MbedTLS does not define macros for all sizes we need, sometimes they define macros only inside their implementation files ad-hoc. As such, I opted to use some of our macros, which is maybe not ideal but still better than using literals.

## Differences from Legacy MbedTLS API

This implementation uses PSA Crypto API exclusively, which differs from the legacy MbedTLS 3.x implementation in several ways:

1. **Key Management**: Keys are imported into PSA key store and referenced by key ID
2. **Context Types**: PSA uses opaque context structures (`psa_hash_operation_t`, etc.)
3. **Error Handling**: Uses `psa_status_t` return codes converted to libtropic return codes
4. **API Design**: Higher-level, more abstract operations (e.g., `psa_aead_encrypt` vs manual GCM operations)
