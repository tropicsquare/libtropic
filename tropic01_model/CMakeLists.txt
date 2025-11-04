cmake_minimum_required(VERSION 3.21.0)

include(FetchContent)

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
#   Options                                                               #
#                                                                         #
###########################################################################

# LT_VALGRIND - as Valgrind does not have to be compiled/linked together with libtropic as ASAN, this switch does not do anything if not using the model.
# If using the model, the model's test runner script uses it to execute the test binaries with Valgrind (but only when using CTest).
option(LT_VALGRIND "Enable Valgrind" OFF)

# Enables compilation with coverage enabled and O0 for functional tests.
option(LT_TEST_COVERAGE "Enable coverage collection for functional tests" OFF)

# Crypto provider
set(LT_CRYPTO "" CACHE STRING "Set a cryptography provider")

set(LAB_BATCH_PKG_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/provisioning_data/2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/"
    CACHE STRING
    "Lab batch package directory to configure the model with"
)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

if(NOT DEFINED PATH_TO_LIBTROPIC)
    set(PATH_TO_LIBTROPIC "libtropic/")
endif()

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

if(LT_ASAN)
    message(STATUS "AddressSanitizer (ASan) is enabled for the entire project.")
    add_compile_options(${LT_ASAN_COMPILE_FLAGS})
    add_link_options(${LT_ASAN_LINK_FLAGS})
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

add_subdirectory("${PATH_TO_LIBTROPIC}cal/${LT_CRYPTO}")

# Developers who integrate Libtropic do not have to use these variables
# It's used in model's main.c to switch crypto contexts without manual changes
set(LT_USE_TREZOR_CRYPTO 0 CACHE INTERNAL "")
set(LT_USE_MBEDTLS_V4 0 CACHE INTERNAL "")

# Handle LT_CRYPTO
if(LT_CRYPTO STREQUAL "trezor_crypto")
    
    message(STATUS "Crypto provider set to trezor_crypto")
    
    # Developers who integrate Libtropic do not have to use this variable
    # It's used in model's main.c to switch crypto contexts without manual changes
    set(LT_USE_TREZOR_CRYPTO 1)

    add_subdirectory("${PATH_TO_LIBTROPIC}/vendor/trezor_crypto/" "trezor_crypto")
    target_compile_definitions(trezor_crypto PRIVATE AES_VAR USE_INSECURE_PRNG)
    target_link_libraries(tropic PUBLIC trezor_crypto)

elseif(LT_CRYPTO STREQUAL "mbedtls_v4")

    message(STATUS "Crypto provider set to mbedtls_v4")

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
    
endif()

target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

set(SOURCES
    main.c
    ${PATH_TO_LIBTROPIC}hal/port/unix/libtropic_port_unix_tcp.c
)

include_directories(
    ${PATH_TO_LIBTROPIC}hal/port/unix
)

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
        target_link_libraries(${exe_name} PRIVATE tropic libtropic::strict_comp_flags)

        # Enable example registry using LT_BUILD_EXAMPLES and choose correct example for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_EXAMPLES)
        target_compile_definitions(${exe_name} PRIVATE ${example_macro})

        # People who integrate libtropic don't have to use these variables
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

    if(LT_TEST_COVERAGE)
        message(STATUS "Enabling converage and compiling with -O0!")
    endif()

    # Loop through tests defined in libtropic and prepare environment.
    foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    
        # Create a correct macro from test name.
        string(TOUPPER ${test_name} test_macro)
        string(REPLACE " " "_" test_macro ${test_macro})

        set(exe_name ${test_name})

        # Define executable (separate for each test) and link dependencies.
        add_executable(${exe_name} ${SOURCES})
        target_link_libraries(${exe_name} PRIVATE tropic libtropic::strict_comp_flags)

        # Enable test registry using LT_BUILD_TESTS and choose correct test for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_TESTS)
        target_compile_definitions(${exe_name} PRIVATE ${test_macro})

        # People who integrate libtropic don't have to use these variables
        # It's used in model's main.c to switch crypto contexts without manual changes
        target_compile_definitions(${exe_name} PRIVATE
            LT_USE_TREZOR_CRYPTO=${LT_USE_TREZOR_CRYPTO}
            LT_USE_MBEDTLS_V4=${LT_USE_MBEDTLS_V4}
        )

        if (LT_TEST_COVERAGE)
            # Add coverage-related flags from common config.
            target_link_libraries(${exe_name} PRIVATE libtropic::coverage_flags)
        endif()

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
