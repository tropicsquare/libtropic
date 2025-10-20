cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Setup                                                                 #
#                                                                         #
###########################################################################

include(cmake/strict_compile_flags.cmake)
include(cmake/coverage_flags.cmake)

option(LT_BUILD_EXAMPLES "Compile example code as part of libtropic library" OFF)
option(LT_BUILD_TESTS "Compile functional tests' code as part of libtropic library" OFF)
# This switch controls if helper utilities are compiled in. In most cases this should be ON,
# examples and tests need to have helpers utilities compiled.
# Switch it off to compile only basic libtropic API.
option(LT_HELPERS "Compile helper function" ON)
# Enable usage of INT pin during communication. Instead of polling for response,
# host will be notified by INT pin when response is ready.
option(LT_USE_INT_PIN "Use INT pin instead of polling for TROPIC01's response" OFF)
option(LT_SEPARATE_L3_BUFF "Define L3 buffer separately out of the handle" OFF)
option(LT_PRINT_SPI_DATA "Print SPI communication to console, used to debug low level communication" OFF)
option(LT_STRICT_COMP_FLAGS "Enable strict compilation flags for libtropic" OFF)
option(LT_ASAN "Enable AddressSanitizer (ASan)" OFF)

# Select pairing keys written during manufacturing into your TROPIC01
set(LT_SH0_KEYS "prod0" CACHE STRING "Choose which pairing keys in slot 0 will be used in examples/tests")
set_property(CACHE LT_SH0_KEYS PROPERTY STRINGS "eng_sample" "prod0")

if(LT_BUILD_TESTS OR LT_BUILD_EXAMPLES)
    # These are internal macros for selecting SH0 keys (examples/tests)
    set(LT_USE_SH0_ENG_SAMPLE 0 CACHE INTERNAL "")
    set(LT_USE_SH0_PROD0      0 CACHE INTERNAL "")

    # Define SH0 macros based on selected string (examples/tests)
    if(LT_SH0_KEYS STREQUAL "eng_sample")
        message(STATUS "Using Engineering sample keys in examples/tests")
        set(LT_USE_SH0_ENG_SAMPLE 1)
        set(LT_USE_SH0_PROD0      0)
    elseif(LT_SH0_KEYS STREQUAL "prod0")
        message(STATUS "Using Production 0 keys in examples/tests")
        set(LT_USE_SH0_ENG_SAMPLE 0)
        set(LT_USE_SH0_PROD0      1)
    else()
        get_property(lt_sh0_keys_choices CACHE LT_SH0_KEYS PROPERTY STRINGS)
        message(FATAL_ERROR "Incorrect SH0 keys for examples/tests specified: '${LT_SH0_KEYS}'\nAvailable SH0 keys: ${lt_sh0_keys_choices}")
    endif()
else()
    message(STATUS "Skipping SH0 key selection (examples/tests not enabled)")
endif()

# TROPIC01 silicon revision: useful for firmware update and functional tests
# (as some behavior) differ between revisions.
set(LT_SILICON_REV "ACAB" CACHE STRING "Set TROPIC01 silicon revision")
set_property(CACHE LT_SILICON_REV PROPERTY STRINGS "ABAB" "ACAB")

# Used for compiling the correct FW update files
# Available versions can be seen in the compatibility table in the main README.md.
# Always use bundled versions from "TROPIC01_fw_update_files" folder!
set(LT_CPU_FW_VERSION "1_0_1" CACHE STRING "Set TROPIC01 FW version")
set_property(CACHE LT_CPU_FW_VERSION PROPERTY STRINGS "1_0_0" "1_0_1")
# Validate specified version
get_property(lt_cpu_fw_version_choices CACHE LT_CPU_FW_VERSION PROPERTY STRINGS)
set(cpu_fw_valid FALSE)
foreach(cpu_fw_choice IN LISTS lt_cpu_fw_version_choices)
    if(LT_CPU_FW_VERSION STREQUAL ${cpu_fw_choice})
        set(cpu_fw_valid TRUE)
        break()
    endif()
endforeach()
if (NOT cpu_fw_valid)
    message(FATAL_ERROR "Invalid TROPIC01 FW version: '${LT_CPU_FW_VERSION}'\nAvailable FW versions: ${lt_cpu_fw_version_choices}")
endif()

# Crypto provider
set(LT_CRYPTO "" CACHE STRING "Set a cryptography provider")
# set_property(CACHE LT_CRYPTO PROPERTY STRINGS "trezor_crypto" "mbedtls")  # uncomment this after mbedtls is ready
set_property(CACHE LT_CRYPTO PROPERTY STRINGS "trezor_crypto")              # delete this after mbedtls is ready

# Logging options
set(LT_LOG_LVL "" CACHE STRING "Set log level")
set_property(CACHE LT_LOG_LVL PROPERTY STRINGS "None" "Error" "Warning" "Info" "Debug")

# Use INFO logging level when building tests/examples
if(LT_BUILD_TESTS OR LT_BUILD_EXAMPLES)
    if(NOT LT_LOG_LVL)
        message(STATUS "No logging level specified, defaulting to INFO for tests/examples.")
        set(LT_LOG_LVL "Info" CACHE BOOL "Set log level to INFO." FORCE)
    endif()
endif()

# These are internal macros for enabling the logging macros
# Default: all logging disabled
set(LT_LOG_ENABLE_INFO  0 CACHE INTERNAL "")
set(LT_LOG_ENABLE_WARN  0 CACHE INTERNAL "")
set(LT_LOG_ENABLE_ERROR 0 CACHE INTERNAL "")
set(LT_LOG_ENABLE_DEBUG 0 CACHE INTERNAL "")

# Set logging ENABLE flags based on selected logging level
if(LT_LOG_LVL STREQUAL "Debug")
    message(STATUS "Log level set to DEBUG")
    set(LT_LOG_ENABLE_DEBUG 1)
    set(LT_LOG_ENABLE_INFO  1)
    set(LT_LOG_ENABLE_WARN  1)
    set(LT_LOG_ENABLE_ERROR 1)

elseif(LT_LOG_LVL STREQUAL "Info")
    message(STATUS "Log level set to INFO")
    set(LT_LOG_ENABLE_DEBUG 0)
    set(LT_LOG_ENABLE_INFO  1)
    set(LT_LOG_ENABLE_WARN  1)
    set(LT_LOG_ENABLE_ERROR 1)

elseif(LT_LOG_LVL STREQUAL "Warning")
    message(STATUS "Log level set to WARNING")
    set(LT_LOG_ENABLE_DEBUG 0)
    set(LT_LOG_ENABLE_INFO  0)
    set(LT_LOG_ENABLE_WARN  1)
    set(LT_LOG_ENABLE_ERROR 1)

elseif(LT_LOG_LVL STREQUAL "Error")
    message(STATUS "Log level set to ERROR")
    set(LT_LOG_ENABLE_DEBUG 0)
    set(LT_LOG_ENABLE_INFO  0)
    set(LT_LOG_ENABLE_WARN  0)
    set(LT_LOG_ENABLE_ERROR 1)
elseif(LT_LOG_LVL STREQUAL "None")
    message(STATUS "Log level set to NONE (logging is disabled)")
elseif(NOT LT_LOG_LVL)
    message(STATUS "No logging level specified, defaulting to NONE (logging is disabled)")
else()
    get_property(lt_log_lvl_choices CACHE LT_LOG_LVL PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect logging level specified: '${LT_LOG_LVL}'\nAvailable logging levels: ${lt_log_lvl_choices}")
endif()

# Define the ASAN flags here so they are not duplicated.
set(LT_ASAN_COMPILE_FLAGS "-fsanitize=address,leak,undefined;-fno-omit-frame-pointer;-g"
    CACHE INTERNAL "Robust compile flags for Sanitizers (ASan, LSan, UBSan)")

set(LT_ASAN_LINK_FLAGS "-fsanitize=address,leak,undefined;-static-libasan"
    CACHE INTERNAL "Link flags for Sanitizers (ASan, LSan, UBSan)")

# Check whether compiling standalone (e.g. as a library) or as a child project (= has parent scope)
# and save result to HAS_PARENT_SCOPE.
get_directory_property(HAS_PARENT_SCOPE PARENT_DIRECTORY)

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
        VERSION 2.0.0
        DESCRIPTION "TROPIC01 software development kit"
        HOMEPAGE_URL "www.tropicsquare.com"
        LANGUAGES C)

###########################################################################
# Libtropic core functions                                                #
###########################################################################
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.c
    # Crypto backend implementation files
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_${LT_CRYPTO}_x25519.c
)

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1_port_wrap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_random.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.h
    # Crypto backend header for required macros
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}/lt_crypto_macros.h
)
set(SDK_DIRS_PRIV ${SDK_DIRS_PRIV}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Add the crypto backend directory to the public include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/${LT_CRYPTO}
)


###########################################################################
# Libtropic fw update:                                                    #
# Compile only selected FW version for dedicated silicon version          #
###########################################################################
if (LT_SILICON_REV STREQUAL "ABAB")
    set(BL_FOLDER boot_v_1_0_1)
elseif (LT_SILICON_REV STREQUAL "ACAB")
    set(BL_FOLDER boot_v_2_0_1)
else()
    get_property(lt_silicon_rev_choices CACHE LT_SILICON_REV PROPERTY STRINGS)    
    message(FATAL_ERROR "Invalid silicon revision: '${LT_SILICON_REV}'\nAvailable silicon revisions: ${lt_silicon_rev_choices}")
endif()

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_VERSION}/fw_CPU.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_VERSION}/fw_SPECT.h
)

set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_VERSION}/
)


###########################################################################
# Libtropic examples:                                                     #
# Collect files to be compiled when "examples" are enabled                #
###########################################################################
set(LIBTROPIC_EXAMPLE_LIST
    lt_ex_show_chip_id_and_fwver
    lt_ex_hello_world_separate_API
    lt_ex_hello_world
    lt_ex_hardware_wallet
    lt_ex_macandd
    lt_ex_fw_update
)

# Export example list to parent project (usually platform-specific implementation) if parent project exists.
if (HAS_PARENT_SCOPE)
    set(LIBTROPIC_EXAMPLE_LIST ${LIBTROPIC_EXAMPLE_LIST} PARENT_SCOPE)
endif()

if(LT_BUILD_EXAMPLES)
    set(SDK_SRCS ${SDK_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_fw_update.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_show_chip_id_and_fwver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hello_world_separate_API.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hello_world.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_hw_wallet.c
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_macandd.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    )
endif()

# Create an example registry based on the example list above. This example registry is then included
# in platform-specific main.c.
set(LIBTROPIC_EXAMPLE_FUNCTIONS_CONTENT "")
foreach(example_name ${LIBTROPIC_EXAMPLE_LIST})
    # Create a correct macro from example name.
    string(TOUPPER ${example_name} example_macro)
    string(REPLACE " " "_" example_macro ${example_macro})
    string(APPEND LIBTROPIC_EXAMPLE_FUNCTIONS_CONTENT
        "#elif defined(${example_macro})\n"
        "__lt_ex_return_val__ = ${example_name}(&__lt_handle__);\n"
    )
endforeach()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/lt_ex_registry_template.c.inc
    ${CMAKE_CURRENT_BINARY_DIR}/lt_ex_registry.c.inc
    @ONLY
)

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
    lt_test_rev_ecdsa_sign
    lt_test_rev_eddsa_sign
    lt_test_ire_pairing_key_slots
    lt_test_ire_write_i_config
    lt_test_rev_ping
    lt_test_rev_r_mem
    lt_test_rev_erase_r_config
    lt_test_rev_handshake_req
    lt_test_rev_mcounter
    lt_test_rev_get_info_req_app
    lt_test_rev_get_info_req_bootloader
    lt_test_rev_read_i_config
    lt_test_rev_read_r_config
    lt_test_rev_resend_req
    lt_test_rev_sleep_req
    lt_test_rev_startup_req
    lt_test_rev_write_r_config
    lt_test_rev_ecc_key_generate
    lt_test_rev_ecc_key_store
    lt_test_rev_random_value_get
    lt_test_rev_mac_and_destroy
    lt_test_rev_get_log_req
)

# Export test list to parent project (usually platform-specific implementation) if parent project exists.
if (HAS_PARENT_SCOPE)
    set(LIBTROPIC_TEST_LIST ${LIBTROPIC_TEST_LIST} PARENT_SCOPE)
endif()

# Create a test registry based on the test list above. This test registry is then included
# in platform-specific main.c.
set(LIBTROPIC_TEST_FUNCTIONS_CONTENT "")
foreach(test_name ${LIBTROPIC_TEST_LIST})
    # Create a correct macro from test name.
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})
    string(APPEND LIBTROPIC_TEST_FUNCTIONS_CONTENT
        "#elif defined(${test_macro})\n"
        "  ${test_name}(&__lt_handle__);\n"
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
        # Special file with stuff common for testing.
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_common.c

        # Files which include test functions definitions.
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_ecdsa_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_eddsa_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_ire_pairing_key_slots.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_ire_write_i_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_ping.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_r_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_erase_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_handshake_req.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_mcounter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_get_info_req_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_get_info_req_bootloader.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_read_i_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_read_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_resend_req.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_sleep_req.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_startup_req.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_write_r_config.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_ecc_key_generate.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_ecc_key_store.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_random_value_get.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_mac_and_destroy.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/lt_test_rev_get_log_req.c
    )
    set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/functional/
    )
endif()

add_library(tropic ${SDK_SRCS} ${SDK_INCS})

target_include_directories(tropic PRIVATE ${SDK_DIRS_PRIV})
target_include_directories(tropic PUBLIC ${SDK_DIRS_PUB})

# Compiler flags: These are an internal implementation detail.
# They are needed to build the library's own object files correctly.
# 'PRIVATE' applies them only to the 'tropic' target itself.
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
# Linker flags: This is a usage requirement for any consumer.
# The final executable that links against 'tropic' needs this flag.
# 'INTERFACE' propagates this requirement to the consumer without
# applying it to the library target itself.
target_link_options(tropic INTERFACE -Wl,--gc-sections)

target_compile_definitions(tropic PRIVATE "$<$<CONFIG:DEBUG>:LT_REDUNDANT_ARG_CHECK>")

# Propagate SH0 macros (examples/tests)
if(LT_BUILD_TESTS OR LT_BUILD_EXAMPLES)
    target_compile_definitions(tropic PUBLIC
        LT_USE_SH0_ENG_SAMPLE=${LT_USE_SH0_ENG_SAMPLE}
        LT_USE_SH0_PROD0=${LT_USE_SH0_PROD0}
    )
endif()

# Propagate logging settings
target_compile_definitions(tropic PUBLIC
    LT_LOG_ENABLE_DEBUG=${LT_LOG_ENABLE_DEBUG}
    LT_LOG_ENABLE_ERROR=${LT_LOG_ENABLE_ERROR}
    LT_LOG_ENABLE_WARN=${LT_LOG_ENABLE_WARN}
    LT_LOG_ENABLE_INFO=${LT_LOG_ENABLE_INFO}
)

###########################################################################
#                                                                         #
#   Compile and link                                                      #
#                                                                         #
###########################################################################

# Conditionally apply strict compilation flags
if(LT_STRICT_COMP_FLAGS OR LT_BUILD_EXAMPLES OR LT_BUILD_TESTS)

    # 1. Apply the compiler options immediately.
    # This is the primary action of this block.
    target_link_libraries(tropic PRIVATE libtropic::strict_comp_flags)

    # 2. Handle the cache variable for user visibility.
    # If the flag wasn't already ON, it means tests/examples implicitly enabled it.
    # We update the cache so the user sees the actual state in cmake-gui.
    if(NOT LT_STRICT_COMP_FLAGS)
        message(STATUS "Enabling strict compilation flags for libtropic because LT_BUILD_EXAMPLES or LT_BUILD_TESTS is ON.")
        set(LT_STRICT_COMP_FLAGS ON CACHE BOOL "Enable strict compilation flags for libtropic" FORCE)
    endif()

endif()

if(LT_TEST_COVERAGE)
    message(STATUS "Enabling coverage collection for libtropic")
    target_link_libraries(tropic PRIVATE libtropic::coverage_flags)
endif()

###########################################################################
#                                                                         #
#   LT_CRYPTO handling                                                    #
#                                                                         #
###########################################################################

# Handle LT_CRYPTO
if(LT_CRYPTO STREQUAL "trezor_crypto")
    
    message(STATUS "Crypto provider set to trezor_crypto")

    add_subdirectory(vendor/trezor_crypto/ "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR USE_INSECURE_PRNG)
    target_link_libraries(tropic PUBLIC trezor_crypto)

# elseif(LT_CRYPTO STREQUAL "mbedtls")

#     message(STATUS "Crypto provider set to mbedtls")

else()

    get_property(lt_crypto_choices CACHE LT_CRYPTO PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect cryptographic provider set to LT_CRYPTO!\nAvailable providers: ${lt_crypto_choices}")
    
endif()

###########################################################################
#                                                                         #
#   Other options handling                                                #
#                                                                         #
###########################################################################

if (LT_SILICON_REV STREQUAL "ABAB")
    target_compile_definitions(tropic PRIVATE ABAB)
elseif (LT_SILICON_REV STREQUAL "ACAB")
    target_compile_definitions(tropic PRIVATE ACAB)
endif()

if(LT_PRINT_SPI_DATA)
    target_compile_definitions(tropic PRIVATE LT_PRINT_SPI_DATA)
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
