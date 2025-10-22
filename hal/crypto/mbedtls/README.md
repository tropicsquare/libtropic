# MbedTLS Crypto Backend for Libtropic

This directory contains the MbedTLS cryptographic backend implementation for Libtropic.

## Overview

This implementation provides the cryptographic HAL layer using the MbedTLS library. It implements all required cryptographic functions specified in the Libtropic SDK.

## Files

- `lt_crypto_macros.h` - Defines the context types for AES-GCM and SHA-256 operations
- `lt_mbedtls_aesgcm.c` - AES-GCM encryption and decryption functions
- `lt_mbedtls_ecdsa.c` - ECDSA signature verification on NIST P-256 curve
- `lt_mbedtls_ed25519.c` - Ed25519 signature verification (requires PSA Crypto API)
- `lt_mbedtls_sha256.c` - SHA-256 hashing functions
- `lt_mbedtls_hmac_sha256.c` - HMAC-SHA256 function
- `lt_mbedtls_x25519.c` - Curve25519 scalar multiplication functions

## Requirements

This implementation requires MbedTLS 3.x with the following features enabled:
- `MBEDTLS_GCM_C` - GCM mode support
- `MBEDTLS_AES_C` - AES cipher
- `MBEDTLS_SHA256_C` - SHA-256 hash
- `MBEDTLS_MD_C` - Message digest wrapper
- `MBEDTLS_ECDSA_C` - ECDSA support
- `MBEDTLS_ECP_C` - Elliptic curve support
- `MBEDTLS_ECP_DP_SECP256R1_ENABLED` - NIST P-256 curve
- `MBEDTLS_ECP_DP_CURVE25519_ENABLED` - Curve25519
- `MBEDTLS_ECDH_C` - ECDH support
- `MBEDTLS_PSA_CRYPTO_C` - PSA Crypto API (for Ed25519)

For MbedTLS 2.x, the Ed25519 implementation may need to be adapted or replaced with an alternative library.

## Implementation Notes

### AES-GCM
Uses `mbedtls_gcm_context` for state management. The implementation supports:
- Key sizes: 128, 192, and 256 bits
- Authenticated encryption and decryption
- Additional authenticated data (AAD)

### ECDSA
- Verifies signatures on the NIST P-256 (secp256r1) curve
- Public keys are provided as 64 bytes (32 bytes X, 32 bytes Y)
- Signatures are 64 bytes (32 bytes R, 32 bytes S)
- Message is hashed with SHA-256 before verification

### Ed25519
- Uses PSA Crypto API for Ed25519 operations
- Verifies Ed25519 signatures
- Public keys are 32 bytes
- Signatures are 64 bytes

### SHA-256
Uses `mbedtls_sha256_context` with streaming interface:
1. Initialize with `lt_sha256_init()`
2. Start hashing with `lt_sha256_start()`
3. Add data with `lt_sha256_update()` (can be called multiple times)
4. Finalize with `lt_sha256_finish()`

### HMAC-SHA256
Single-call HMAC computation using `mbedtls_md_hmac()`.

### X25519
- Implements Diffie-Hellman key exchange on Curve25519
- `lt_X25519()`: Computes shared secret from private and public keys
- `lt_X25519_scalarmult()`: Computes public key from private key (scalar multiplication with base point)

## Integration

To use this backend, set `LT_CRYPTO=mbedtls` when configuring CMake (after CMakeLists.txt is updated according to the documentation).
