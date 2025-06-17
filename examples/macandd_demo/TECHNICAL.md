# Technical Documentation: MAC-and-Destroy Implementation

## Overview

This document provides detailed technical information about the MAC-and-Destroy (M&D) PIN verification system implementation using the TROPIC01 security chip.

## Cryptographic Foundation

### MAC-and-Destroy Algorithm

The MAC-and-Destroy feature implements a secure authentication mechanism with the following properties:

1. **Finite Attempts**: Only a predetermined number of authentication attempts are possible
2. **Progressive Destruction**: Each failed attempt permanently destroys cryptographic material
3. **Irreversible Process**: Once slots are destroyed, they cannot be recovered without re-initialization
4. **Hardware Enforcement**: The TROPIC01 chip enforces these constraints in hardware

### Key Derivation Functions

The system uses HMAC-SHA256 for all key derivation operations:

```c
// Master secret generation
lt_hmac_sha256(s, 32, "0", 1, nvm.t);       // Tag for verification

// Slot initialization key
lt_hmac_sha256(s, 32, "1", 1, u);           // Initialization value

// PIN-based verification key
lt_hmac_sha256(zero_key, 32, PIN||A, len, v); // PIN verification key

// Encryption key derivation
lt_hmac_sha256(w, 32, PIN||A, len, k_i);    // Slot-specific encryption key

// Final secret derivation
lt_hmac_sha256(s, 32, "2", 1, secret);      // Released secret
```

### Cryptographic Flow

#### PIN Setup Phase (`lt_PIN_set`)

1. **Secret Generation**: Generate random 32-byte master secret `s`
2. **Tag Computation**: `t = KDF(s, "0")` for later verification
3. **Initialization Key**: `u = KDF(s, "1")` for slot initialization
4. **PIN Key**: `v = KDF(0, PIN||A)` where 0 is zero key
5. **Slot Processing**: For each slot `i`:
   - Initialize slot with `u`
   - Overwrite with `v` to get `w`
   - Re-initialize with `u`
   - Compute `k_i = KDF(w, PIN||A)`
   - Encrypt: `c_i = s ⊕ k_i`

#### PIN Verification Phase (`lt_PIN_check`)

1. **Attempt Decrement**: Reduce remaining attempts counter
2. **PIN Key**: `v' = KDF(0, PIN'||A)` for attempted PIN
3. **Slot Access**: `w' = MACANDD(i, v')` - destroys slot `i`
4. **Key Derivation**: `k'_i = KDF(w', PIN'||A)`
5. **Decryption**: `s' = c_i ⊕ k'_i`
6. **Verification**: `t' = KDF(s', "0")`, check if `t' == t`
7. **Success Handling**: If verified, re-initialize remaining slots

## Memory Architecture

### Non-Volatile Memory Structure

```c
struct lt_macandd_nvm_t {
    uint8_t i;                    // Remaining attempts counter
    uint8_t ci[ROUNDS*32];        // Encrypted secrets (32 bytes per slot)
    uint8_t t[32];               // Verification tag
} __attribute__((__packed__));
```

### Memory Layout

| Offset | Size | Description |
|--------|------|-------------|
| 0x00 | 1 byte | Attempts counter (`i`) |
| 0x01 | ROUNDS×32 bytes | Encrypted secrets array (`ci`) |
| 0x01+ROUNDS×32 | 32 bytes | Verification tag (`t`) |

### R Memory Usage

- **Total Size**: 444 bytes (single slot configuration)
- **Slot Overhead**: 32 bytes per MAC-and-Destroy round
- **Metadata**: ~12 bytes for counters and tags
- **Maximum Rounds**: Limited by available R memory

## Security Analysis

### Attack Resistance

#### Brute Force Protection
- **Time Complexity**: O(1) per attempt (constant time verification)
- **Space Complexity**: Each attempt consumes irreplaceable cryptographic material
- **Attempt Limitation**: Hardware-enforced finite attempts (typically 3-12)

#### Side Channel Resistance
- **Timing Attacks**: Constant-time MAC operations in hardware
- **Power Analysis**: TROPIC01 hardware countermeasures
- **Fault Injection**: Hardware-based fault detection

#### Cryptographic Strength
- **Key Material**: 256-bit secrets with HMAC-SHA256
- **Randomness**: Hardware random number generator
- **Key Derivation**: Standards-compliant HMAC-based KDF

### Security Assumptions

1. **Hardware Integrity**: TROPIC01 chip operates correctly
2. **Key Storage**: Pairing keys are securely stored
3. **Communication**: Local communication channel (localhost)
4. **Implementation**: Correct implementation of MAC-and-Destroy protocol

## Performance Characteristics

### Timing Analysis

| Operation | Typical Time | Hardware Dependency |
|-----------|--------------|-------------------|
| PIN Setup | ~500ms | TROPIC01 slot operations |
| PIN Verification | ~100ms | Single MAC operation |
| Slot Initialization | ~50ms per slot | Hardware MAC calculation |
| Key Derivation | ~10ms | Software HMAC-SHA256 |

### Resource Utilization

#### Memory Usage
- **Stack**: ~2KB during operation
- **Heap**: Minimal allocation
- **NVM**: Proportional to attempt count
- **Temporary**: 32-byte buffers for keys

#### CPU Usage
- **Cryptographic Operations**: Hardware-accelerated
- **Network I/O**: Minimal overhead
- **State Management**: Low complexity

## Implementation Details

### Error Handling

The system implements comprehensive error handling:

```c
typedef enum {
    LT_OK = 0x00,           // Success
    LT_FAIL = 0x01,         // General failure
    LT_PARAM_ERR = 0x02,    // Parameter error
    LT_HOST_NO_SESSION = 0x03 // No active session
} lt_ret_t;
```

### State Machine

```
[INIT] → [SETUP] → [READY] → [VERIFY] → [SUCCESS/FAILURE]
   ↓         ↓        ↓         ↓           ↓
[ERROR] ←─────────────────────────────────┘
```

1. **INIT**: Hardware initialization
2. **SETUP**: PIN configuration and slot initialization
3. **READY**: Waiting for verification attempts
4. **VERIFY**: Processing PIN attempt
5. **SUCCESS/FAILURE**: Result processing
6. **ERROR**: Error state requiring reset

### Communication Protocol

#### Message Format

All messages are ASCII-encoded for simplicity:

```
PIN_ATTEMPT  := [0-9]{4}
SET_PIN      := "SET_PIN:" [0-9]{4}
SET_ATTEMPTS := "SET_ATTEMPTS:" [0-9]{1,2}
STATUS_MSG   := "STATUS:" TYPE ":" CURRENT ":" MAX
```

#### Response Codes

| Code | Value | Meaning |
|------|-------|---------|
| LT_OK | 0x00 | Operation successful |
| LT_FAIL | 0x01 | Operation failed |

## Testing and Validation

### Test Scenarios

1. **Successful Authentication**
   - Correct PIN on first attempt
   - Secret derivation verification
   - Slot re-initialization

2. **Failed Attempts**
   - Progressive slot destruction
   - Attempt counter decrement
   - Final lockout behavior

3. **Configuration Changes**
   - PIN modification
   - Attempt limit adjustment
   - State synchronization

4. **Error Conditions**
   - Invalid PIN format
   - Communication failures
   - Hardware errors

### Validation Metrics

- **Correctness**: All test scenarios pass
- **Security**: No information leakage
- **Performance**: Within timing requirements
- **Reliability**: Consistent behavior

## Future Enhancements

### Potential Improvements

1. **Multi-Client Support**: Support multiple simultaneous connections
2. **Encrypted Communication**: TLS/SSL for network security
3. **Audit Logging**: Comprehensive security event logging
4. **Dynamic PIN Length**: Support for variable-length PINs
5. **Biometric Integration**: Additional authentication factors

### Scalability Considerations

- **Slot Scaling**: Optimize for larger attempt counts
- **Performance**: Hardware acceleration for batch operations
- **Memory**: Efficient storage for large-scale deployments

## References

1. TROPIC01 Datasheet and Application Notes
2. HMAC-SHA256 Specification (RFC 2104, RFC 6234)
3. MAC-and-Destroy Cryptographic Principles
4. libtropic API Documentation

---

**Note**: This is a technical reference document. For implementation details, refer to the source code and official TROPIC01 documentation. 