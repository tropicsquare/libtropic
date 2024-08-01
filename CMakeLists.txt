cmake_minimum_required(VERSION 3.21.0)


project(libtropic_SDK
        VERSION 0.0.1
        DESCRIPTION "TROPIC01 software development kit"
        HOMEPAGE_URL "www.tropicsquare.com"
        LANGUAGES C)


###########################################################################
#                                                                         #
#   Library setup                                                         #
#                                                                         #
###########################################################################

option(TS_HAL_UNIX "Use TCP and connect to model" OFF)
option(TS_HAL_STM32_UART "Use UART and connect to model" OFF)
option(TS_HAL_STM32_SPI "Use SPI and connect to real chip" OFF)

option(USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" ON)
option(TS_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)

option(DEBUG "Compile with -g -Wextra -Wpedantic -Werror" OFF)
option(BUILD_DOCS "Build documentation" OFF)


###########################################################################
#                                                                         #
#   libtropic source files                                               #
#                                                                         #
###########################################################################

# Enable l1 implementation for a target platform
if (TS_HAL_UNIX)
    message(DEBUG "UNIX TCP HAL selected (for model)")
    add_definitions(-DTS_HAL_UNIX=1)
    # Main program is responsible for adding file from
    # appropriate hal/XXX/XXX/folder into its sources and compile it,
    # For more info check examples.
elseif(TS_HAL_STM32_SPI)
    message(DEBUG "STM32 HAL selected ")
    add_definitions(-DTS_HAL_STM32_SPI=1)
    # TODO get rid of HOST_KEY_* once get info req with cert is working
    add_definitions(-DHOST_KEY_FPGA=1)
    # Main program is responsible for adding file from
    # appropriate hal/XXX/XXX/folder into its sources and compile it,
    # For more info check examples.
elseif(TS_HAL_STM32_UART)
    message(DEBUG "STM32 UART HAL selected ")
    add_definitions(-DTS_HAL_STM32_UART=1)
    # Main program is responsible for adding file from
    # appropriate hal/XXX/XXX/folder into its sources and compile it,
    # For more info check examples.
else()
    message(WARNING "No HAL enabled, add your own implementation for L1 functions !! ")
    set(SDK_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l1.c)
endif()

set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_random.c
)
set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_hkdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ts_random.h
)

add_library(tropic SHARED ${SDK_SRCS} ${SDK_INCS})

target_include_directories(tropic PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_compile_options(tropic PRIVATE -Wall)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
if(DEBUG)
    target_compile_options(tropic PRIVATE -g)
endif()


###########################################################################
#                                                                         #
#   Setting up crypto backend based on option switches                    #
#                                                                         #
###########################################################################

if (USE_TREZOR_CRYPTO)
    # Trezor library
    add_subdirectory(vendor/trezor_crypto/ "trezor_crypto")
    target_link_libraries(tropic PRIVATE trezor_crypto)
    # libtropic wrappers for trezor library
    add_subdirectory(hal/crypto/trezor_crypto)
    target_compile_definitions(tropic PRIVATE USE_TREZOR_CRYPTO)
endif()

if (TS_CRYPTO_MBEDTLS)
    message(FATAL_ERROR "Mbedtls support is not implemented yet")
endif()


###########################################################################
#                                                                         #
#   Building documentation is off by default                              #
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
