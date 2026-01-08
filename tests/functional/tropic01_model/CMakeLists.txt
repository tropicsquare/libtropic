cmake_minimum_required(VERSION 3.21.0)
cmake_policy(SET CMP0152 OLD) # Path resolution policy

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################
project(libtropic_functional_tests_linux_model
        DESCRIPTION "Functional tests in Linux environment on model"
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################
file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/_deps/ PATH_DEPS)
file(REAL_PATH ../../../ PATH_LIBTROPIC)
file(REAL_PATH ../src PATH_FN_TESTS)

if (NOT EXISTS ${PATH_DEPS})
    message(FATAL_ERROR "Dependencies not installed. Please run download_deps.sh!")
endif()

if (NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    message(FATAL_ERROR "We support running functional tests on Linux only!")
endif()

###########################################################################
#                                                                         #
#   Options and user configuration                                        #
#                                                                         #
###########################################################################

# Optional prefix to tests registered to CTest. Useful when running same test against
# different chips to differentiate them by their name in JUnit output.
if (NOT DEFINED CTEST_PREFIX)
    set(CTEST_PREFIX "")
endif()

# This option will pass --use-valgrind to the model's test runner script 
# which will execute the test binaries with Valgrind.
option(LT_VALGRIND "Enable Valgrind" OFF)

set(VALGRIND_ARG "")
if(LT_VALGRIND)
    message(STATUS "Tests will be run with Valgrind.")
    set(VALGRIND_ARG --use-valgrind)
endif()

set(MODEL_CFG_PATH "${PATH_LIBTROPIC}/scripts/tropic01_model/model_cfg.yml" CACHE STRING "Path to model configuration.")
set(RUN_LOGS_DIR "${CMAKE_CURRENT_BINARY_DIR}/run_logs/" CACHE STRING "Path to logging directory.")

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to Libtropic repository root directory.
add_subdirectory(${PATH_FN_TESTS} "libtropic_functional_tests")

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

# Add POSIX TCP HAL for communication with the model.
add_subdirectory("${PATH_LIBTROPIC}/hal/posix/tcp" "posix_tcp_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Enable strict compile flags for main.c and Libtropic HAL sources.
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/main.c ${LT_HAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")   
endif()

###########################################################################
#                                                                         #
# FUNCTIONAL TESTS CONFIGURATION                                          #
#                                                                         #
# This section will automatically configure CTest for launching tests     #
# defined in Libtropic. Do NOT hardcode any test definitions here.        #
# Define them in common CMakeLists.txt for functional tests.              #
#                                                                         #
###########################################################################
# Enable CTest.
enable_testing()

# Remove tests we don't want to run against model.
list(REMOVE_ITEM LIBTROPIC_TEST_LIST
    lt_test_rev_startup_req
    lt_test_rev_get_info_req_bootloader
    lt_test_rev_resend_req  # This test is not run only because the model returns CHIP_STATUS.START=0 when in Maintenance Mode.
    lt_test_rev_get_log_req # This test is not run only because the model returns CHIP_STATUS.START=0 when in Maintenance Mode.
)

# Loop through tests defined in Libtropic and prepare environment.
foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    # Create a correct macro from test name.
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})

    set(exe_name ${test_name})

    # Define executable (separate for each test) and link dependencies.
    add_executable(${exe_name} ${SOURCES})
    target_link_libraries(${exe_name} PRIVATE libtropic_functional_tests)

    # Choose correct test for the binary.
    target_compile_definitions(${exe_name} PRIVATE ${test_macro})

    if(CTEST_PREFIX STREQUAL "")
        set(TEST_NAME_WITH_PREFIX ${test_name})
    else()
        set(TEST_NAME_WITH_PREFIX ${CTEST_PREFIX}_${test_name})
    endif()

    # Define the test command.
    set(TEST_COMMAND
        "python3" "-m" "model_runner"
        "-e" "${CMAKE_CURRENT_BINARY_DIR}/${exe_name}"
        "-c" "${MODEL_CFG_PATH}"
        ${VALGRIND_ARG}
        "-o" "${RUN_LOGS_DIR}"
    )
    # Convert TEST_COMMAND into a space-separated string.
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
    add_test(NAME ${TEST_NAME_WITH_PREFIX}
                COMMAND sh -c "${ASAN_FILES_CLEANUP_COMMAND}${TEST_COMMAND}"
    )

    # Prepare environment variables for the test run. Start with PYTHONPATH.
    set(TEST_ENVIRONMENT "PYTHONPATH=${PYTHONPATH}:${PATH_LIBTROPIC}/scripts/tropic01_model")

    # If ASan is enabled, define a unique log path and add it to the environment.
    if(LT_ASAN)
        list(APPEND TEST_ENVIRONMENT "ASAN_OPTIONS=log_path=${ASAN_LOG_PATH}")
        list(APPEND TEST_ENVIRONMENT "UBSAN_OPTIONS=log_path=${ASAN_LOG_PATH}")
    endif()

    # Set the final combined environment for the test.
    set_tests_properties(${TEST_NAME_WITH_PREFIX} PROPERTIES
        ENVIRONMENT "${TEST_ENVIRONMENT}"
    )
endforeach()