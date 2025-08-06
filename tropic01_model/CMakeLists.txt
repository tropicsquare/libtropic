cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

if(NOT DEFINED PATH_TO_LIBTROPIC)
    set(PATH_TO_LIBTROPIC "../")
endif()

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(tropic01_model
        VERSION 0.1.0
        DESCRIPTION "Port of libtropic to be used against TROPIC01 model."
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Use trezor crypto as a source of backend cryptography code
set(LT_USE_TREZOR_CRYPTO ON)

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

set(SOURCES
    main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_TO_LIBTROPIC}hal/port/unix/lt_port_unix_tcp.c
)

###########################################################################
#                                                                         #
#   EXAMPLES CONFIGURATION                                                #
#                                                                         #
###########################################################################

if (LT_BUILD_EXAMPLES)

    # So we can include preprocessed example registry (lt_ex_registry.c.inc).
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libtropic)

    # Loop through examples defined in libtropic and prepare environment.
    foreach(example_name IN LISTS LIBTROPIC_EXAMPLE_LIST)

        # Create a correct macro from example name.
        string(TOUPPER ${example_name} example_macro)
        string(REPLACE " " "_" example_macro ${example_macro})

        set(exe_name ${example_name})

        # Define executable (separate for each example) and link dependencies.
        add_executable(${exe_name} ${SOURCES})
        target_link_libraries(${exe_name} PRIVATE tropic)

        # Enable example registry using LT_BUILD_EXAMPLES and choose correct example for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_EXAMPLES)
        target_compile_definitions(${exe_name} PRIVATE ${example_macro})

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
    set(LAB_BATCH_PKG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${PATH_TO_LIBTROPIC}/provisioning_data/${DEFAULT_LAB_BATCH_PKG_DIR}")
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
    # Wrap it in a custom target so we can depend on it
    add_custom_target(generate_model_cfg DEPENDS ${MODEL_CFG_PATH})

    # Remove tests we don't want to run against model
    list(REMOVE_ITEM LIBTROPIC_TEST_LIST
        lt_test_rev_startup_req
    )

    # Loop through tests defined in libtropic and prepare environment.
    foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    
        # Create a correct macro from test name.
        string(TOUPPER ${test_name} test_macro)
        string(REPLACE " " "_" test_macro ${test_macro})

        set(exe_name ${test_name})

        # Define executable (separate for each test) and link dependencies.
        add_executable(${exe_name} ${SOURCES})
        target_link_libraries(${exe_name} PRIVATE tropic)

        # Enable test registry using LT_BUILD_TESTS and choose correct test for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_TESTS)
        target_compile_definitions(${exe_name} PRIVATE ${test_macro})

        # Make sure model configuration exists before building this test
        add_dependencies(${exe_name} generate_model_cfg)

        # Add CTest entry.
        add_test(NAME ${test_name}
                COMMAND python3 -m model_test_runner
                        -t ${CMAKE_CURRENT_BINARY_DIR}/${exe_name}
                        -c ${MODEL_CFG_PATH}
                        -o ${RUN_LOGS_DIR}
        )

        # Add scripts/ to PYTHONPATH, so we can run model_test_runner.py as a module
        set_tests_properties(${test_name} PROPERTIES
            ENVIRONMENT "PYTHONPATH=${PYTHONPATH}:${ABSOLUTE_PATH_TO_LIBTROPIC}/scripts/"
        )
    endforeach()
endif()
