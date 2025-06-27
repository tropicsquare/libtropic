cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Setup                                                                 #
#                                                                         #
###########################################################################

option(LT_USE_TREZOR_CRYPTO "Use trezor_crypto as a cryptography provider" OFF)
option(LT_CRYPTO_MBEDTLS "Use mbedtls as a cryptography provider" OFF)
option(LT_BUILD_DOCS "Build documentation" OFF)
option(LT_BUILD_EXAMPLES "Compile example code as part of libtropic library" OFF)
option(LT_BUILD_TESTS "Compile functional tests' code as part of libtropic library" OFF)
option(LT_ENABLE_FW_UPDATE "Enable firmware update functions and compile firmware update in a form of byte array" OFF)
# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only basic libtropic API.
option(LT_HELPERS "Compile helper function" ON)
# Enable usage of INT pin during communication. Instead of polling for response,
# host will be notified by INT pin when response is ready.
option(LT_USE_INT_PIN "Use INT pin instead of polling for TROPIC01's response" OFF)
option(LT_SEPARATE_L3_BUFF "Define L3 buffer separately out of the handle" OFF)
option(LT_PRINT_SPI_DATA "Print SPI communication to console, used to debug low level communication" OFF)
option(LT_USE_ASSERT "Insert assert() into LT_ASSERT/LT_ASSERT_COND" OFF)

set(LT_SH0_PRIV_PATH "" CACHE FILEPATH "Path to file with SH0 private key in PEM or DER format")

# Check if cryptography provider is defined
if(
    (NOT LT_BUILD_DOCS)        AND
    (NOT LT_USE_TREZOR_CRYPTO) AND
    (NOT LT_CRYPTO_MBEDTLS)
)
    message(FATAL_ERROR "No cryptography provider is defined.")
endif()

set(LT_SH0_PRIV_PATH "" CACHE FILEPATH "Path to file with SH0 private key in PEM or DER format")
set(LT_SH0_PUB_PATH "" CACHE FILEPATH "Path to file with SH0 public key in PEM or DER format")

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
#        VERSION unreleased
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

###########################################################################
# Libtropic core functions                                                #
###########################################################################
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/keys/keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.c
)

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lt_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lt_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.h
)
set(SDK_DIRS_PRIV ${SDK_DIRS_PRIV}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
###########################################################################
# Libtropic examples:                                                     #
# Collect files to be compiled when "examples" are enabled                #
###########################################################################
if(LT_BUILD_EXAMPLES)
    set(SDK_SRCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hello_world.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hello_world_separate_API.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hw_wallet.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_macandd.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    )
endif()

###########################################################################
# LIBTROPIC FUNCTIONAL TESTS                                              #
# Define test function names and source files which contain               #
# implementation.                                                         #
#                                                                         #
# This is the ONLY place where tests should be defined. The list is then  #
# pulled by platform-specific implementation and CTest is configured      #
# automatically.                                                          #
###########################################################################
# Define a list of tests. The names should equal to respective functions which implement the test,
# this name is also by CTest to identify the test and also used to create a macro (uppercase format)
# which is used for test selection.
set(LIBTROPIC_TEST_LIST
    lt_test_chip_id
    lt_test_write_pairing_keys
    lt_test_write_r_config
    lt_test_ping
    lt_test_ecc_eddsa
    lt_test_r_mem
    lt_test_erase_r_config
    lt_test_dump_r_config
    lt_test_dump_i_config
    lt_test_dump_cert_store
)

# Export test list to parent project (usually platform-specific implementation).
set(LIBTROPIC_TEST_LIST ${LIBTROPIC_TEST_LIST} PARENT_SCOPE)

# Create a test registry based on the test list above. This test registry is then included
# in platform-specific main.c.
set(LIBTROPIC_TEST_FUNCTIONS_CONTENT "")
foreach(test_name ${LIBTROPIC_TEST_LIST})
    # Create a correct macro from test name.
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})
    string(APPEND LIBTROPIC_TEST_FUNCTIONS_CONTENT
        "#elif defined(${test_macro})\n"
        "  ${test_name}();\n"
    )
endforeach()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_registry_template.c.inc
    ${CMAKE_CURRENT_BINARY_DIR}/lt_test_registry.c.inc
    @ONLY
)

# Add source files which actually implement the tests.
# This could probably also be automatically generated by the test list, but I wanted to leave
# a bit of flexibility for a case where one would want to implement more tests in one file etc.
if(LT_BUILD_TESTS)
    set(SDK_SRCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_chip_id.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_write_pairing_keys.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_write_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_ping.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_ecc_eddsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_r_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_erase_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_dump_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_dump_i_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_dump_cert_store.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/
    )
endif()

# Only add key generation logic if examples or tests are being compiled
if (LT_BUILD_EXAMPLES OR LT_BUILD_TESTS)

    if (LT_SH0_PRIV_PATH STREQUAL "")
        message(FATAL_ERROR "LT_SH0_PRIV_PATH must be set when LT_BUILD_EXAMPLES or LT_BUILD_TESTS is enabled.")
    endif()

    set(SH0_PRIV_RAW_FILE "${CMAKE_BINARY_DIR}/sh0_priv.bin")
    set(SH0_PUB_RAW_FILE "${CMAKE_BINARY_DIR}/sh0_pub.bin")
    set(KEYS_C_FILE "${CMAKE_BINARY_DIR}/sh0_keys.c")
    set(PYTHON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_x25519_key_data.py")

    add_custom_command(
        OUTPUT ${SH0_PRIV_RAW_FILE} ${SH0_PUB_RAW_FILE}
        COMMAND python3 ${PYTHON_SCRIPT} --priv-key ${LT_SH0_PRIV_PATH} --priv-out ${SH0_PRIV_RAW_FILE} --pub-out ${SH0_PUB_RAW_FILE}
        DEPENDS "${PYTHON_SCRIPT}" "${LT_SH0_PRIV_PATH}"
        COMMENT "Extracting raw data from X25519 private key"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${KEYS_C_FILE}"
        COMMAND ${CMAKE_COMMAND}
                -DSH0_PRIV_RAW_PATH=${SH0_PRIV_RAW_FILE}
                -DSH0_PUB_RAW_PATH=${SH0_PUB_RAW_FILE}
                -DOUTPUT_C_FILE=${KEYS_C_FILE}
                -P "${CMAKE_CURRENT_SOURCE_DIR}/generate_sh0_keys.cmake"
        DEPENDS "${SH0_PRIV_RAW_FILE}" "${SH0_PUB_RAW_FILE}"
        COMMENT "Generating sh0_keys.c from DER files"
        VERBATIM
    )


    # Ensure the tropic target depends on the generated file
    list(APPEND SDK_SRCS "${KEYS_C_FILE}")
endif()

###########################################################################
# Libtropic fw update:                                                    #
# Collect files to be compiled when "fw update" is enabled                #
###########################################################################

if(LT_ENABLE_FW_UPDATE)
    set(SDK_INCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_fw_update.c
        ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/boot_v_1_0_1/fw_v_0_3_1/fw_CPU_0_3_1.h
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/boot_v_1_0_1/fw_v_0_3_1/
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

if(LT_PRINT_SPI_DATA)
    target_compile_definitions(tropic PRIVATE LT_PRINT_SPI_DATA)
endif()

if(LT_USE_TREZOR_CRYPTO)
    add_subdirectory(vendor/trezor_crypto/ "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR USE_INSECURE_PRNG)
    target_link_libraries(tropic PRIVATE trezor_crypto)
    target_compile_definitions(tropic PRIVATE LT_USE_TREZOR_CRYPTO)
endif()

if(LT_HELPERS)
    target_compile_definitions(tropic PUBLIC LT_HELPERS)
endif()

if(LT_USE_INT_PIN)
    target_compile_definitions(tropic PRIVATE LT_USE_INT_PIN)
endif()

if(LT_SEPARATE_L3_BUFF)
    target_compile_definitions(tropic PRIVATE LT_SEPARATE_L3_BUFF)
endif()

if(LT_USE_ASSERT)
    target_compile_definitions(tropic PUBLIC LT_USE_ASSERT)
endif()