cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Setup                                                                 #
#                                                                         #
###########################################################################

option(LT_USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(LT_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)
option(LT_BUILD_DOCS "Build documentation" OFF)
option(LT_EXPERIMENTAL_SPI_UART "Experimental feature for spi slave to serial hw convertor" OFF)
option(LT_ADD_EXAMPLES "Compile example code as part of libtropic library" OFF)
option(LT_ENABLE_FW_UPDATE "Enable firmware update functions and compile firmware update in a form of byte array" OFF)
# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only basic libtropic API.
option(LT_UTILS "Compile helper function" ON)

# Check if cryptography provider is defined
#if(
#    (NOT BUILD_DOCS)        AND
#    (NOT USE_TREZOR_CRYPTO) AND
#    (NOT LT_CRYPTO_MBEDTLS)
#)
#    message(FATAL_ERROR "No cryptography provider is defined.")
#endif()

###########################################################################
#                                                                         #
#   Building documentation                                                #
#                                                                         #
###########################################################################

if(LT_BUILD_DOCS)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
        message(STATUS "Doxygen found, building docs")
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

###########################################################################
#                                                                         #
#   Collect files:                                                        #
#                                                                         #
#   SDK_SRCS:       source files                                          #
#   SDK_INCS:       header files                                          #
#   SDK_DIRS_PRIV:  private directories                                   #
#   SDK_DIRS_PUB:   public directories                                    #
#                                                                         #
###########################################################################

project(libtropic_SDK
        VERSION 0.0.1
        DESCRIPTION "TROPIC01 software development kit"
        HOMEPAGE_URL "www.tropicsquare.com"
        LANGUAGES C)

##########
# CRYPTO #
##########
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/lt_crypto_trezor_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/lt_crypto_trezor_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/lt_crypto_trezor_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/trezor_crypto/lt_crypto_trezor_x25519.c
)

# --- add new crypto sources above this line ---

############################
# LIBTROPIC IMPLEMENTATION #
############################
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.c
)

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.h
)
set(SDK_DIRS_PRIV ${SDK_DIRS_PRIV}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Collect files to be compiled when "examples" are enabled
if(LT_ADD_EXAMPLES)
    set(SDK_SRCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hello_world.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hw_wallet.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_test_reversible.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_test_ireversible.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/keys.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    )
endif()


# Collect files to be compiled when "fw update" is enabled
if(LT_ENABLE_FW_UPDATE)
    set(SDK_INCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/boot_v1.0.1/fw_CPU_0_1_2.h
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_fw_update.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/boot_v1.0.1/
    )
endif()


add_library(tropic ${SDK_SRCS} ${SDK_INCS})

target_include_directories(tropic PRIVATE ${SDK_DIRS_PRIV})
target_include_directories(tropic PUBLIC ${SDK_DIRS_PUB})

target_compile_definitions(tropic PRIVATE "$<$<CONFIG:DEBUG>:LIBT_DEBUG>")

###########################################################################
#                                                                         #
#   Compile and link                                                      #
#                                                                         #
###########################################################################

# This options just add 10ms synchronisation delay before read and write functions.
# Needed for hardware spi slave to uart converter.
if(LT_EXPERIMENTAL_SPI_UART)
    target_compile_definitions(tropic PRIVATE LT_EXPERIMENTAL_SPI_UART=1)
endif()

if(LT_USE_TREZOR_CRYPTO)
    add_subdirectory(vendor/trezor_crypto/ "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR USE_INSECURE_PRNG)
    target_link_libraries(tropic PRIVATE trezor_crypto)
    target_compile_definitions(tropic PRIVATE LT_USE_TREZOR_CRYPTO)
endif()

if(LT_UTILS)
    target_compile_definitions(tropic PUBLIC LT_UTILS)
endif()

