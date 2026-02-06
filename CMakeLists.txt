cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Setup                                                                 #
#                                                                         #
###########################################################################

include(CheckSymbolExists)
include(CheckIncludeFiles)
# Used in the tropic01_model/CMakeLists.txt and platform repositories
include(cmake/coverage_flags.cmake)

###########################################################################
#                                                                         #
#   Library configuration                                                 #
#                                                                         #
###########################################################################

# This switch controls if helper utilities are compiled in.
# Switch it off to compile only core libtropic API.
option(LT_HELPERS "Compile helper function" ON)
# Enable usage of INT pin during communication. Instead of polling for response,
# host will be notified by INT pin when response is ready.
option(LT_USE_INT_PIN "Use INT pin instead of polling for TROPIC01's response" OFF)
option(LT_SEPARATE_L3_BUFF "Define L3 buffer separately out of the handle" OFF)
option(LT_PRINT_SPI_DATA "Print SPI communication to console, used to debug low level communication" OFF)

#########################

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

#########################
# TROPIC01 silicon revision: useful for firmware update and functional tests
# (as some behavior) differ between revisions.
set(LT_SILICON_REV "ACAB" CACHE STRING "Set TROPIC01 silicon revision")
set_property(CACHE LT_SILICON_REV PROPERTY STRINGS "ABAB" "ACAB")

# Choose version of RISC-V FW to update to.
# Available versions can be seen in the compatibility table in the main README.md.
# Default value is always the latest version available in the current Libtropic release.
set(LT_CPU_FW_UPDATE_DATA_VER "2_0_0" CACHE STRING "Set TROPIC01 FW version to update to")
set_property(CACHE LT_CPU_FW_UPDATE_DATA_VER PROPERTY STRINGS "1_0_0" "1_0_1" "2_0_0")
# Validate specified version
get_property(lt_cpu_fw_ver_choices CACHE LT_CPU_FW_UPDATE_DATA_VER PROPERTY STRINGS)
set(cpu_fw_valid FALSE)
foreach(cpu_fw_choice IN LISTS lt_cpu_fw_ver_choices)
    if(LT_CPU_FW_UPDATE_DATA_VER STREQUAL ${cpu_fw_choice})
        set(cpu_fw_valid TRUE)
        break()
    endif()
endforeach()
if (NOT cpu_fw_valid)
    message(FATAL_ERROR "Invalid TROPIC01 FW version: '${LT_CPU_FW_UPDATE_DATA_VER}'\nAvailable FW versions: ${lt_cpu_fw_ver_choices}")
endif()

# Logging options
set(LT_LOG_LVL "" CACHE STRING "Set log level")
set_property(CACHE LT_LOG_LVL PROPERTY STRINGS "None" "Error" "Warning" "Info" "Debug")

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
        VERSION 3.1.0
        DESCRIPTION "TROPIC01 software development kit"
        HOMEPAGE_URL "https://tropicsquare.github.io/libtropic/latest/"
        LANGUAGES C)

###########################################################################
# Libtropic core functions                                                #
###########################################################################
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_port_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic_l2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic_l3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libtropic_default_sh0_keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_tr01_attrs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_secure_memzero.c
)

set(SDK_INCS ${SDK_INCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_l2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/libtropic_l3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_crc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_port_wrap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l2_frame_check.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_l3_process.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_hkdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lt_asn1_der.h
)
set(SDK_DIRS_PRIV ${SDK_DIRS_PRIV}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_UPDATE_DATA_VER}/fw_CPU.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_UPDATE_DATA_VER}/fw_SPECT.h
)

set(SDK_DIRS_PUB ${SDK_DIRS_PUB}
    ${CMAKE_CURRENT_SOURCE_DIR}/TROPIC01_fw_update_files/${BL_FOLDER}/fw_v_${LT_CPU_FW_UPDATE_DATA_VER}/
)

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

if(LT_TEST_COVERAGE)
    message(STATUS "Enabling coverage collection for libtropic")
    target_link_libraries(tropic PRIVATE libtropic::coverage_flags)
endif()

# Handle definitions for safe memory zeroing functions
set(SECURE_ZERO_FUNCS
    "memset_explicit"
    "explicit_bzero"
    "explicit_memset"
    "memset_s"
)

# Ensure C11 safe functions are exposed if available
target_compile_definitions(tropic PRIVATE __STDC_WANT_LIB_EXT1__=1)

check_include_files("strings.h" LT_HAVE_STRINGS_H)
if(LT_HAVE_STRINGS_H)
    set(headers_to_check "string.h;strings.h")
    target_compile_definitions(tropic PRIVATE LT_HAVE_STRINGS_H)
else()
    set(headers_to_check "string.h")
endif()

foreach(secure_zero_func ${SECURE_ZERO_FUNCS})
    # Generate a variable name (e.g. LT_HAVE_MEMSET_S)
    string(TOUPPER "${secure_zero_func}" secure_zero_func_upper)
    set(secure_zero_func_macro "LT_HAVE_${secure_zero_func_upper}")

    # Check if the symbol exists
    check_symbol_exists(${secure_zero_func} "${headers_to_check}" ${secure_zero_func_macro})

    # If it exists, pass it to the compiler definitions
    if(${secure_zero_func_macro})
        target_compile_definitions(tropic PRIVATE ${secure_zero_func_macro})
    endif()
endforeach()

###########################################################################
#                                                                         #
#   Other options handling                                                #
#                                                                         #
###########################################################################

if (LT_SILICON_REV STREQUAL "ABAB")
    target_compile_definitions(tropic PUBLIC ABAB)
elseif (LT_SILICON_REV STREQUAL "ACAB")
    target_compile_definitions(tropic PUBLIC ACAB)
endif()

if(LT_PRINT_SPI_DATA)
    target_compile_definitions(tropic PRIVATE LT_PRINT_SPI_DATA)
endif()

if(LT_HELPERS)
    target_compile_definitions(tropic PUBLIC LT_HELPERS)
endif()

if(LT_USE_INT_PIN)
    target_compile_definitions(tropic PUBLIC LT_USE_INT_PIN)
endif()

if(LT_SEPARATE_L3_BUFF)
    target_compile_definitions(tropic PUBLIC LT_SEPARATE_L3_BUFF)
endif()

# Development option incompatible with production chips.
if(LT_RETRIEVE_ALARM_LOG)
    target_compile_definitions(tropic PUBLIC LT_RETRIEVE_ALARM_LOG)
endif()

##################################################
# =========================
# Trezor Crypto (embedded subset)
# =========================

set(TREZOR_CRYPTO_SRC
    # --- AES / GCM ---
    vendor/trezor_crypto/aes/aesgcm.c
    vendor/trezor_crypto/aes/aesccm.c
    vendor/trezor_crypto/aes/aescrypt.c
    vendor/trezor_crypto/aes/aes_modes.c
    vendor/trezor_crypto/aes/aestab.c
    vendor/trezor_crypto/aes/aeskey.c
    vendor/trezor_crypto/aes/gf128mul.c

    vendor/trezor_crypto/hasher.c
    vendor/trezor_crypto/blake256.c
    vendor/trezor_crypto/blake2b.c
    vendor/trezor_crypto/groestl.c
    vendor/trezor_crypto/sha2.c
    vendor/trezor_crypto/sha3.c
    vendor/trezor_crypto/ripemd160.c
    vendor/trezor_crypto/memzero.c
    vendor/trezor_crypto/hmac.c

    # --- Curve25519 ---
    vendor/trezor_crypto/ed25519-donna/curve25519-donna-32bit.c
    vendor/trezor_crypto/ed25519-donna/curve25519-donna-helpers.c
    vendor/trezor_crypto/ed25519-donna/ed25519-donna-impl-base.c
    vendor/trezor_crypto/ed25519-donna/ed25519.c

    # --- Ed25519 ---
    vendor/trezor_crypto/ed25519-donna/ed25519-keccak.c
    vendor/trezor_crypto/ed25519-donna/ed25519-sha3.c
    vendor/trezor_crypto/ed25519-donna/modm-donna-32bit.c
    vendor/trezor_crypto/ed25519-donna/curve25519-donna-scalarmult-base.c

    vendor/trezor_crypto/ed25519-donna/ed25519-donna-32bit-tables.c
    vendor/trezor_crypto/ed25519-donna/ed25519-donna-basepoint-table.c

    vendor/trezor_crypto/rand.c
    vendor/trezor_crypto/ecdsa.c
    vendor/trezor_crypto/nist256p1.c
    vendor/trezor_crypto/bignum.c
    vendor/trezor_crypto/rfc6979.c
    vendor/trezor_crypto/address.c
    vendor/trezor_crypto/base58.c
    vendor/trezor_crypto/secp256k1.c
    vendor/trezor_crypto/hmac_drbg.c
)

target_sources(tropic PRIVATE
    ${TREZOR_CRYPTO_SRC}
)

target_include_directories(tropic PRIVATE
    vendor/trezor_crypto
    vendor/trezor_crypto/aes
    vendor/trezor_crypto/ed25519-donna
)

target_compile_definitions(tropic PRIVATE
    LT_USE_TREZOR_CRYPTO
    AES_VAR
    USE_INSECURE_PRNG
)