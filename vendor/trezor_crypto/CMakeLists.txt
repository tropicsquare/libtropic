cmake_minimum_required(VERSION 3.21.0)


set(TREZOR_CRYPTO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aesccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aescrypt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aes_modes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aestab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/aeskey.c
    ${CMAKE_CURRENT_SOURCE_DIR}/aes/gf128mul.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hasher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/blake256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/blake2b.c
    ${CMAKE_CURRENT_SOURCE_DIR}/groestl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sha2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sha3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ripemd160.c
    ${CMAKE_CURRENT_SOURCE_DIR}/memzero.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/curve25519-donna-32bit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/curve25519-donna-helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519-donna-impl-base.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519-keccak.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519-sha3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/modm-donna-32bit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/curve25519-donna-scalarmult-base.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519-donna-32bit-tables.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ed25519-donna/ed25519-donna-basepoint-table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rand.c
)

add_library(trezor_crypto ${TREZOR_CRYPTO_SRCS})

target_include_directories(trezor_crypto PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/
                            )
