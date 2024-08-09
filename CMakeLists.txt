cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Setup                                                                 #
#                                                                         #
###########################################################################

option(USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(TS_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)
option(BUILD_DOCS "Build documentation" OFF)

###########################################################################
#                                                                         #
#   Building documentation                                                #
#                                                                         #
###########################################################################

if(BUILD_DOCS)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
        message(STATUS "Doxygen found, building docs")
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# Check if cryptography provider is defined
if((NOT USE_TREZOR_CRYPTO) AND (NOT TS_CRYPTO_MBEDTLS) AND (NOT BUILD_DOCS))
    message(FATAL_ERROR "Mbed TLS or trezor_crypto must be enabled!")
endif()

###########################################################################
#                                                                         #
#   Collect files                                                         #
#                                                                         #
###########################################################################

project(libtropic_SDK
        VERSION 0.0.1
        DESCRIPTION "TROPIC01 software development kit"
        HOMEPAGE_URL "www.tropicsquare.com"
        LANGUAGES C)

set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_hkdf.c
)

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_platform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_hkdf.h
)

set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/ts_trezor_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/ts_trezor_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/ts_trezor_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/ts_trezor_x25519.c
)

add_library(tropic ${SDK_SRCS} ${SDK_INCS})

target_include_directories(tropic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(tropic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)


###########################################################################
#                                                                         #
#   Compile and link                                                      #
#                                                                         #
###########################################################################

if(USE_TREZOR_CRYPTO)
    add_subdirectory(vendor/trezor_crypto/ "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR)
    target_link_libraries(tropic PRIVATE trezor_crypto)
    target_compile_definitions(tropic PRIVATE USE_TREZOR_CRYPTO)
endif()

target_compile_options(tropic PRIVATE -Wall)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
target_compile_options(tropic PRIVATE -Wno-implicit-function-declaration)


