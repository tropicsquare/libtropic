# MbedTLS PSA Crypto Backend for Libtropic

This directory contains the MbedTLS 4.0 PSA Crypto API backend implementation for Libtropic.

## Overview

This implementation provides the cryptographic HAL layer using the **PSA Cryptography API** from MbedTLS 4.0. The PSA (Platform Security Architecture) Crypto API is the modern, standardized interface for cryptographic operations in MbedTLS 4.x.

## Files

- `lt_crypto_macros.h` - Defines the context types for AES-GCM and SHA-256 operations using PSA types
- `lt_mbedtls_aesgcm.c` - AES-GCM encryption and decryption using PSA AEAD operations
- `lt_mbedtls_ecdsa.c` - ECDSA signature verification on NIST P-256 curve using PSA
- `lt_mbedtls_ed25519.c` - Ed25519 signature verification using PSA
- `lt_mbedtls_sha256.c` - SHA-256 hashing using PSA hash operations
- `lt_mbedtls_hmac_sha256.c` - HMAC-SHA256 using PSA MAC operations
- `lt_mbedtls_x25519.c` - Curve25519 (X25519) key agreement using PSA

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
- Public keys are imported as 64 bytes (32 bytes X, 32 bytes Y coordinates)
- Signatures are 64 bytes (32 bytes R, 32 bytes S)
- Message is hashed with SHA-256 before verification

### Ed25519
- Uses `psa_verify_message()` for Ed25519 signature verification
- Public keys are 32 bytes
- Signatures are 64 bytes
- Pure EdDSA (no pre-hashing)

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

The PSA Crypto subsystem is automatically initialized on first use in each module. The `psa_crypto_init()` function is called once per module to set up the PSA cryptography subsystem.

## Differences from Legacy MbedTLS API

This implementation uses PSA Crypto API exclusively, which differs from the legacy MbedTLS 3.x implementation in several ways:

1. **Key Management**: Keys are imported into PSA key store and referenced by key ID
2. **Context Types**: PSA uses opaque context structures (`psa_hash_operation_t`, etc.)
3. **Error Handling**: Uses `psa_status_t` return codes converted to libtropic return codes
4. **API Design**: Higher-level, more abstract operations (e.g., `psa_aead_encrypt` vs manual GCM operations)

## Integration

To use this backend, set `LT_CRYPTO=mbedtls` when configuring CMake (after CMakeLists.txt is updated according to the documentation). Ensure MbedTLS 4.0 submodule is initialized with TF-PSA-Crypto.
