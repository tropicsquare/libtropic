cmake_minimum_required(VERSION 3.21.0)

# Wrappers for trezor_crypto library.
set(TS_CRYPTO_TREZOR
    ${CMAKE_CURRENT_SOURCE_DIR}/ts_trezor_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ts_trezor_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ts_trezor_x25519.c
)

target_sources(tropic PRIVATE ${TS_CRYPTO_TREZOR})
