cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Define project                                                        #
#                                                                         #
###########################################################################
project(libtropic_functional_tests
        DESCRIPTION "Functional tests common wrapper"
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################
if (NOT PATH_LIBTROPIC)
    message(FATAL_ERROR "Path to Libtropic not set!")
endif()

if (NOT PATH_DEPS)
    message(FATAL_ERROR "Path to dependencies not set!")
endif()

include(${PATH_LIBTROPIC}/cmake/strict_compile_flags.cmake)
include(${PATH_LIBTROPIC}/cmake/static_asan_flags.cmake)

# Check whether compiling standalone (e.g. as a library) or as a child project (= has parent scope)
# and save result to HAS_PARENT_SCOPE.
get_directory_property(HAS_PARENT_SCOPE PARENT_DIRECTORY)

if (NOT HAS_PARENT_SCOPE)
    message(FATAL_ERROR "Functional tests can be compiled only as a part of a platform project!")
endif()

###########################################################################
#                                                                         #
#   Options and user configuration                                        #
#                                                                         #
###########################################################################

# Strict compilation flags
option(LT_STRICT_COMPILATION "Enable strict compilation flags" ON)

# Enable ASAN
option(LT_ASAN "Enable static AddressSanitizer (ASan)" OFF)

# Select CAL
set(LT_CAL "" CACHE STRING "Set a CAL (Crypto Abstraction Layer)")
set_property(CACHE LT_CAL PROPERTY STRINGS "trezor_crypto" "mbedtls_v4")

# Select pairing keys written during manufacturing into your TROPIC01
set(LT_SH0_KEYS "prod0" CACHE STRING "Choose which pairing keys in slot 0 will be used in this example")
set_property(CACHE LT_SH0_KEYS PROPERTY STRINGS "eng_sample" "prod0")

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

set(LT_LOG_LVL "Info" CACHE STRING "Set log level, default INFO for tests.")

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

if(LT_STRICT_COMPILATION)
    message(STATUS "Enabling strict compilation flags.")
    # Enable strict compile flags for Libtropic library
    target_link_libraries(tropic PRIVATE libtropic::strict_comp_flags)
endif()

# Propagate SH0 macros
target_compile_definitions(tropic PUBLIC
    LT_USE_SH0_ENG_SAMPLE=${LT_USE_SH0_ENG_SAMPLE}
    LT_USE_SH0_PROD0=${LT_USE_SH0_PROD0}
)

###########################################################################
#                                                                         #
#   Crypto backend handling                                               #
#                                                                         #
#   Note: The libtropic consumer does not have to do this in his          #
#   application, we do this to quickly switch crypto backends.            #
#   Refer to the libtropic documentation for more info about what should  #
#   be done by the consumer.                                              #
#                                                                         #
###########################################################################

# Developers who integrate Libtropic do not have to use these variables
# It's used in main.c to switch crypto contexts without manual changes
set(LT_USE_TREZOR_CRYPTO 0 CACHE INTERNAL "")
set(LT_USE_MBEDTLS_V4 0 CACHE INTERNAL "")

# Handle LT_CAL
if(LT_CAL STREQUAL "trezor_crypto")
    message(STATUS "Crypto provider set to trezor_crypto")
    add_subdirectory("${PATH_LIBTROPIC}/cal/trezor_crypto" "trezor_crypto_cal")
    
    # Developers who integrate Libtropic do not have to use this variable
    # It's used in main.c to switch crypto contexts without manual changes
    set(LT_USE_TREZOR_CRYPTO 1)

    add_subdirectory("${PATH_LIBTROPIC}/vendor/trezor_crypto/" "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR USE_INSECURE_PRNG)
    target_link_libraries(tropic PUBLIC trezor_crypto)
elseif(LT_CAL STREQUAL "mbedtls_v4")
    message(STATUS "Crypto provider set to mbedtls_v4")
    add_subdirectory("${PATH_LIBTROPIC}/cal/mbedtls_v4" "mbedtls_v4_cal")

    # Developers who integrate Libtropic do not have to use this variable
    # It's used in main.c to switch crypto contexts without manual changes
    set(LT_USE_MBEDTLS_V4 1)

    set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls_v4 test building.")
    set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbedtls_v4 examples building.")
    add_subdirectory("${PATH_DEPS}/mbedtls_v4/" "mbedtls_v4")

    target_link_libraries(tropic PUBLIC mbedtls)
else()
    get_property(lt_cal_choices CACHE LT_CAL PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect CAL set to LT_CAL!\nSupported CALs: ${lt_cal_choices}")
endif()

target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

###########################################################################
#                                                                         #
#   Special dependencies for tests                                        #
#                                                                         #
###########################################################################

# micro-ecc library is used for ECDSA signature verification
add_library(micro_ecc_lib STATIC
    ${PATH_DEPS}/micro-ecc/uECC.c
)

target_include_directories(micro_ecc_lib PUBLIC # PUBLIC as it is also used in tropic target
    ${PATH_DEPS}/micro-ecc
)

# ed25519 library is used for EdDSA signature verification
add_library(ed25519_lib STATIC
    ${PATH_DEPS}/ed25519/src/verify.c
    ${PATH_DEPS}/ed25519/src/add_scalar.c
    ${PATH_DEPS}/ed25519/src/fe.c
    ${PATH_DEPS}/ed25519/src/ge.c
    ${PATH_DEPS}/ed25519/src/key_exchange.c
    ${PATH_DEPS}/ed25519/src/keypair.c
    ${PATH_DEPS}/ed25519/src/sc.c
    ${PATH_DEPS}/ed25519/src/seed.c
    ${PATH_DEPS}/ed25519/src/sha512.c
    ${PATH_DEPS}/ed25519/src/sign.c
)

target_include_directories(ed25519_lib PUBLIC # PUBLIC as it is also used in tropic target
    ${PATH_DEPS}/ed25519/src/
)

###########################################################################
#                                                                         #
#   Test sources                                                          #
#                                                                         #
###########################################################################

# LIBTROPIC FUNCTIONAL TESTS                                              
# Define test function names and source files which contain               
# implementation.                                                         
#                                                                         
# This is the ONLY place where tests should be defined. The list is then  
# pulled by platform-specific implementation and CTest is configured      
# automatically.                                                          

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
    lt_test_rev_param_check
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

# Export test list to parent project (usually platform-specific implementation)
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
        "  ${test_name}(__lt_handle__);\n"
    )
endforeach()

configure_file(
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_registry_template.c.inc
    ${CMAKE_CURRENT_BINARY_DIR}/lt_test_registry.c.inc
    @ONLY
)

set(TEST_SRCS
    # Special file with stuff common for testing.
    ${PATH_LIBTROPIC}/tests/common/lt_test_common.c

    # Files which include test functions definitions.
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_ecdsa_sign.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_eddsa_sign.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_ire_pairing_key_slots.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_ire_write_i_config.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_ping.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_r_mem.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_erase_r_config.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_handshake_req.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_mcounter.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_param_check.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_get_info_req_app.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_get_info_req_bootloader.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_read_i_config.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_read_r_config.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_resend_req.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_sleep_req.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_startup_req.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_write_r_config.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_ecc_key_generate.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_ecc_key_store.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_random_value_get.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_mac_and_destroy.c
    ${PATH_LIBTROPIC}/tests/functional/src/lt_test_rev_get_log_req.c
)

set(TEST_DIRS
    ${PATH_LIBTROPIC}/tests/functional/src
    ${PATH_LIBTROPIC}/tests/common/
)

add_library(libtropic_functional_tests ${TEST_SRCS})

# Include Libtropic library
target_link_libraries(libtropic_functional_tests PUBLIC tropic)
# Include also Libtropic private files (used by tests)
target_include_directories(libtropic_functional_tests PRIVATE ${PATH_LIBTROPIC}/src)

# So we can include preprocessed test registry (lt_test_registry.c.inc).
target_include_directories(libtropic_functional_tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Test-specific include dirs
target_include_directories(libtropic_functional_tests PUBLIC ${TEST_DIRS})
# Link dependencies
target_link_libraries(libtropic_functional_tests PRIVATE ed25519_lib)
target_link_libraries(libtropic_functional_tests PRIVATE micro_ecc_lib)

# Propagate CAL macros
target_compile_definitions(libtropic_functional_tests PUBLIC
    LT_USE_TREZOR_CRYPTO=${LT_USE_TREZOR_CRYPTO}
    LT_USE_MBEDTLS_V4=${LT_USE_MBEDTLS_V4}
)

# Turn on ASAN for entire project
if(LT_ASAN)
    message(STATUS "AddressSanitizer (ASan) is enabled for the entire project.")
    add_compile_options(${LT_ASAN_COMPILE_FLAGS})
    add_link_options(${LT_ASAN_LINK_FLAGS})
endif()