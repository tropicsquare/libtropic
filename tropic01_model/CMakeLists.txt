cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(tropic01_model
        VERSION 1.0.0
        DESCRIPTION "TROPIC01 model project using libtropic."
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

if(NOT DEFINED PATH_TO_LIBTROPIC)
    set(PATH_TO_LIBTROPIC "libtropic/")
endif()

include(FetchContent)
include(${PATH_TO_LIBTROPIC}cmake/strict_compile_flags.cmake)
include(${PATH_TO_LIBTROPIC}cmake/static_asan_flags.cmake)

###########################################################################
#                                                                         #
#   Options                                                               #
#                                                                         #
###########################################################################

option(LT_STRICT_COMPILATION "Enable strict compilation flags" ON)

option(LT_ASAN "Enable static AddressSanitizer (ASan)" OFF)

# LT_VALGRIND - as Valgrind does not have to be compiled/linked together with libtropic as ASAN, this switch does not do anything if not using the model.
# If using the model, the model's test runner script uses it to execute the test binaries with Valgrind (but only when using CTest).
option(LT_VALGRIND "Enable Valgrind" OFF)

# Select CAL
set(LT_CAL "" CACHE STRING "Set a CAL (Crypto Abstraction Layer)")
set_property(CACHE LT_CAL PROPERTY STRINGS "trezor_crypto" "mbedtls_v4")

set(LAB_BATCH_PKG_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/provisioning_data/2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/"
    CACHE STRING
    "Lab batch package directory to configure the model with"
)

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

if(LT_ASAN)
    message(STATUS "Enabling static AddressSanitizer (ASan).")
    # This applies the flags to all targets
    add_compile_options(${LT_ASAN_COMPILE_FLAGS})
    add_link_options(${LT_ASAN_LINK_FLAGS})
endif()

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

if(LT_STRICT_COMPILATION)
    message(STATUS "Enabling strict compilation flags.")
    target_link_libraries(tropic PRIVATE libtropic::strict_comp_flags)
endif()

set(VALGRIND_ARG "")
if(LT_VALGRIND)
    message(STATUS "Tests will be run with Valgrind (only when using CTest!).")
    set(VALGRIND_ARG --use-valgrind)
endif()

###########################################################################
#                                                                         #
#   Crypto backend handling                                               #
#                                                                         #
#   Note: The libtropic consumer does not have to do this in his          #
#   application, we do this to quickly switch crypto backends.            #
#   Refer to the libtropic documentation for more info about what should  #
#   be done by the consumer this.                                         #
#                                                                         #
###########################################################################

# Developers who integrate Libtropic do not have to use these variables
# It's used in model's main.c to switch crypto contexts without manual changes
set(LT_USE_TREZOR_CRYPTO 0 CACHE INTERNAL "")
set(LT_USE_MBEDTLS_V4 0 CACHE INTERNAL "")

# Handle LT_CAL
if(LT_CAL STREQUAL "trezor_crypto")
    message(STATUS "Crypto provider set to trezor_crypto")
    add_subdirectory("${PATH_TO_LIBTROPIC}cal/trezor_crypto")
    
    # Developers who integrate Libtropic do not have to use this variable
    # It's used in model's main.c to switch crypto contexts without manual changes
    set(LT_USE_TREZOR_CRYPTO 1)

    add_subdirectory("${PATH_TO_LIBTROPIC}/vendor/trezor_crypto/" "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE
        AES_VAR
        USE_INSECURE_PRNG
        ed25519_verify=trezor_crypto_ed25519_verify  # Solves name collisions with the ed25519_lib target.
    )
    target_link_libraries(tropic PUBLIC trezor_crypto)
elseif(LT_CAL STREQUAL "mbedtls_v4")
    message(STATUS "Crypto provider set to mbedtls_v4")
    add_subdirectory("${PATH_TO_LIBTROPIC}cal/mbedtls_v4")

    # Developers who integrate Libtropic do not have to use this variable
    # It's used in model's main.c to switch crypto contexts without manual changes
    set(LT_USE_MBEDTLS_V4 1)

    # Fetch MbedTLS v4.0.0
    FetchContent_Declare(
        mbedtls
        URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls_v4 test building.")
    set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbedtls_v4 examples building.")
    FetchContent_MakeAvailable(mbedtls)

    target_link_libraries(tropic PUBLIC mbedtls)
else()
    get_property(lt_cal_choices CACHE LT_CAL PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect CAL set to LT_CAL!\nSupported CALs by the model: ${lt_cal_choices}")
endif()

target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

add_subdirectory("${PATH_TO_LIBTROPIC}hal/posix/tcp")

target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

set(SOURCES
    main.c
)

if(LT_BUILD_TESTS)
    # Download micro-ecc repository used for ECDSA signature verification
    FetchContent_Declare(
        micro_ecc
        GIT_REPOSITORY https://github.com/kmackay/micro-ecc.git
        GIT_TAG        v1.1
    )
    FetchContent_MakeAvailable(micro_ecc)

    add_library(micro_ecc_lib STATIC
        ${micro_ecc_SOURCE_DIR}/uECC.c
    )

    target_include_directories(micro_ecc_lib PRIVATE
        ${micro_ecc_SOURCE_DIR}
    )

    target_include_directories(tropic PRIVATE ${micro_ecc_SOURCE_DIR})
    target_link_libraries(tropic PRIVATE micro_ecc_lib)

    # Download ed25519 repository used for ECDSA signature verification
    FetchContent_Declare(
        ed25519
        GIT_REPOSITORY https://github.com/orlp/ed25519.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(ed25519)

    add_library(ed25519_lib STATIC
        ${ed25519_SOURCE_DIR}/src/verify.c
        ${ed25519_SOURCE_DIR}/src/add_scalar.c
        ${ed25519_SOURCE_DIR}/src/fe.c
        ${ed25519_SOURCE_DIR}/src/ge.c
        ${ed25519_SOURCE_DIR}/src/key_exchange.c
        ${ed25519_SOURCE_DIR}/src/keypair.c
        ${ed25519_SOURCE_DIR}/src/sc.c
        ${ed25519_SOURCE_DIR}/src/seed.c
        ${ed25519_SOURCE_DIR}/src/sha512.c
        ${ed25519_SOURCE_DIR}/src/sign.c
    )

    target_include_directories(ed25519_lib PRIVATE
        ${ed25519_SOURCE_DIR}/src/
    )

    target_include_directories(tropic PRIVATE ${ed25519_SOURCE_DIR}/src)
    target_link_libraries(tropic PRIVATE ed25519_lib)
endif()

###########################################################################
#                                                                         #
#   EXAMPLES CONFIGURATION                                                #
#                                                                         #
###########################################################################

if (LT_BUILD_EXAMPLES)

    # So we can include preprocessed example registry (lt_ex_registry.c.inc).
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libtropic)

    # Remove examples that are not supported by the model
    list(REMOVE_ITEM LIBTROPIC_EXAMPLE_LIST
        lt_ex_show_chip_id_and_fwver
        lt_ex_fw_update
    )

    # Loop through examples defined in libtropic and prepare environment.
    foreach(example_name IN LISTS LIBTROPIC_EXAMPLE_LIST)

        # Create a correct macro from example name.
        string(TOUPPER ${example_name} example_macro)
        string(REPLACE " " "_" example_macro ${example_macro})

        set(exe_name ${example_name})

        # Define executable (separate for each example) and link dependencies.
        add_executable(${exe_name} ${SOURCES})
        
        if(LT_STRICT_COMPILATION)
            target_link_libraries(${exe_name} PRIVATE tropic libtropic::strict_comp_flags)
        endif()

        # Enable example registry using LT_BUILD_EXAMPLES and choose correct example for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_EXAMPLES)
        target_compile_definitions(${exe_name} PRIVATE ${example_macro})

        # Developers who integrate Libtropic do not have to use these variables
        # It's used in model's main.c to switch crypto contexts without manual changes
        target_compile_definitions(${exe_name} PRIVATE
            LT_USE_TREZOR_CRYPTO=${LT_USE_TREZOR_CRYPTO}
            LT_USE_MBEDTLS_V4=${LT_USE_MBEDTLS_V4}
        )

    endforeach()

endif()

###########################################################################
#                                                                         #
# FUNCTIONAL TESTS CONFIGURATION                                          #
#                                                                         #
# This section will automatically configure CTest for launching tests     #
# defined in libtropic. Do NOT hardcode any test definitions here.        #
# Define them in libtropic.                                               #
#                                                                         #
# To build functional tests, use -DLT_BUILD_TESTS=1 in cmake invocation.  #
#                                                                         #
###########################################################################

if(LT_BUILD_TESTS)
    # Enable CTest.
    enable_testing()

    # Resolve libtropic absolute path once
    get_filename_component(ABSOLUTE_PATH_TO_LIBTROPIC ${PATH_TO_LIBTROPIC} ABSOLUTE)

    # So we can include preprocessed test registry (lt_test_registry.c.inc).
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libtropic)

    # Set some paths
    set(MODEL_CFG_PATH "${CMAKE_CURRENT_BINARY_DIR}/model_cfg.yml")
    set(RUN_LOGS_DIR "${CMAKE_CURRENT_BINARY_DIR}/run_logs/")

    # Create configuration for the model
    add_custom_command(
        OUTPUT ${MODEL_CFG_PATH}
        COMMAND ${CMAKE_COMMAND} -E env
                PYTHONPATH=${PYTHONPATH}:${ABSOLUTE_PATH_TO_LIBTROPIC}/tropic01_model/
                python3 -m create_model_cfg
                --pkg-dir ${LAB_BATCH_PKG_DIR}
                --model-cfg ${MODEL_CFG_PATH}
        COMMENT "Creating configuration for the model"
        VERBATIM
    )
    # Wrap it in a custom target so we can create a dependency
    add_custom_target(generate_model_cfg DEPENDS ${MODEL_CFG_PATH})

    # Remove tests we don't want to run against model
    list(REMOVE_ITEM LIBTROPIC_TEST_LIST
        lt_test_rev_startup_req
        lt_test_rev_get_info_req_bootloader
    )

    # Loop through tests defined in libtropic and prepare environment.
    foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    
        # Create a correct macro from test name.
        string(TOUPPER ${test_name} test_macro)
        string(REPLACE " " "_" test_macro ${test_macro})

        set(exe_name ${test_name})

        # Define executable (separate for each test) and link dependencies.
        add_executable(${exe_name} ${SOURCES})

        if(LT_STRICT_COMPILATION)
            target_link_libraries(${exe_name} PRIVATE tropic libtropic::strict_comp_flags)
        endif()

        # Enable test registry using LT_BUILD_TESTS and choose correct test for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_TESTS)
        target_compile_definitions(${exe_name} PRIVATE ${test_macro})

        # Developers who integrate Libtropic do not have to use this variable
        # It's used in model's main.c to switch crypto contexts without manual changes
        target_compile_definitions(${exe_name} PRIVATE
            LT_USE_TREZOR_CRYPTO=${LT_USE_TREZOR_CRYPTO}
            LT_USE_MBEDTLS_V4=${LT_USE_MBEDTLS_V4}
        )

        # Make sure model configuration exists before building this test
        add_dependencies(${exe_name} generate_model_cfg)

        # Define the test command
        set(TEST_COMMAND
            "python3" "-m" "model_runner"
            "-e" "${CMAKE_CURRENT_BINARY_DIR}/${exe_name}"
            "-c" "${MODEL_CFG_PATH}"
            ${VALGRIND_ARG}
            "-o" "${RUN_LOGS_DIR}"
        )
        # Convert TEST_COMMAND into a space-separated string
        list(JOIN TEST_COMMAND " " TEST_COMMAND)

        # Variable for cleanup command which will remove previous log files from ASAN.
        # ASAN always puts PID number at the end of the file, so running the same test
        # multiple times always generates new ASAN log file, but we only want the latest one.
        set(ASAN_FILES_CLEANUP_COMMAND "")
        if(LT_ASAN)
            set(ASAN_LOG_PATH "${RUN_LOGS_DIR}${test_name}_asan_report.log")
            set(ASAN_FILES_CLEANUP_COMMAND "rm -f ${ASAN_LOG_PATH}* && ")
        endif()

        # Add CTest entry.
        add_test(NAME ${test_name}
                 COMMAND sh -c "${ASAN_FILES_CLEANUP_COMMAND}${TEST_COMMAND}"
        )

        # Prepare environment variables for the test run. Start with PYTHONPATH.
        set(TEST_ENVIRONMENT "PYTHONPATH=${PYTHONPATH}:${ABSOLUTE_PATH_TO_LIBTROPIC}/scripts/")

        # If ASan is enabled, define a unique log path and add it to the environment.
        if(LT_ASAN)
            list(APPEND TEST_ENVIRONMENT "ASAN_OPTIONS=log_path=${ASAN_LOG_PATH}")
            list(APPEND TEST_ENVIRONMENT "UBSAN_OPTIONS=log_path=${ASAN_LOG_PATH}")
        endif()

        # Set the final combined environment for the test.
        set_tests_properties(${test_name} PROPERTIES
            ENVIRONMENT "${TEST_ENVIRONMENT}"
        )
    endforeach()
endif()
