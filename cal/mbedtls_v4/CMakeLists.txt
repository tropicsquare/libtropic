cmake_minimum_required(VERSION 3.21.0)

set(_VENDOR_ED25519_PATH "../../vendor/ed25519")

set(LT_CAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_common.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lt_mbedtls_v4_x25519.c
    # ed25519 source files (not part of mbedtls)
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/add_scalar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/fe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/ge.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/key_exchange.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/keypair.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/sc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/seed.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/sha512.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/sign.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/verify.c
)

set(LT_CAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${_VENDOR_ED25519_PATH}/src/
)

# export generic names for parent to consume
set(LT_CAL_SRCS ${LT_CAL_SRCS} PARENT_SCOPE)
set(LT_CAL_INC_DIRS ${LT_CAL_INC_DIRS} PARENT_SCOPE)