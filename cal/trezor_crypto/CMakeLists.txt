cmake_minimum_required(VERSION 3.21.0)

set(LT_CAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_common.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_trezor_crypto_x25519.c
)

set(LT_CAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# export generic names for parent to consume
set(LT_CAL_SRCS ${LT_CAL_SRCS} PARENT_SCOPE)
set(LT_CAL_INC_DIRS ${LT_CAL_INC_DIRS} PARENT_SCOPE)