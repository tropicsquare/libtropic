cmake_minimum_required(VERSION 3.21)

# ---- Trezor Crypto sources (EXPLÍCITOS, sin glob salvaje)
set(TREZOR_CRYPTO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/aes/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/aes/aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/hash/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/hash/hasher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/hmac/hmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/curve25519/curve25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto/rand.c
)

# ---- Tropic CAL glue
set(LT_CAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_x25519.c
    ${TREZOR_CRYPTO_SRCS}
)

set(LT_CAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/trezor_crypto/crypto
)

# ---- export al padre
set(LT_CAL_SRCS ${LT_CAL_SRCS} PARENT_SCOPE)
set(LT_CAL_INC_DIRS ${LT_CAL_INC_DIRS} PARENT_SCOPE)