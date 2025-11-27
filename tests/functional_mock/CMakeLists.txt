cmake_minimum_required(VERSION 3.21)

project(libtropic_functional_mock_tests)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

if(NOT DEFINED PATH_TO_LIBTROPIC)
    set(PATH_TO_LIBTROPIC "${CMAKE_CURRENT_SOURCE_DIR}/../../")
endif()

include(FetchContent)

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to libtropic's repository root folder
set(LT_LOG_LVL "Debug" CACHE BOOL "Set log level to DEBUG." FORCE)
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

###########################################################################
#                                                                         #
#   Crypto backend handling                                               #
#                                                                         #
###########################################################################
message(STATUS "Crypto provider set to mbedtls_v4")
add_subdirectory("${PATH_TO_LIBTROPIC}cal/mbedtls_v4" "cal_mbedtls_v4")

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
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

add_subdirectory("${PATH_TO_LIBTROPIC}hal/mock" "hal_mock")

target_sources(tropic PUBLIC ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

# Collect test sources in this folder following naming convention lt_test_*.c
file(GLOB TEST_SRCS LIST_DIRECTORIES OFF "${CMAKE_CURRENT_LIST_DIR}/lt_test_*.c")
# Add also common test sources
set(TEST_SRCS 
    ${TEST_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/lt_test_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers/lt_mock_helpers.c
)

# Compile all tests once, so they can be linked to individual executables and not compile them multiple times.
add_library(
    libtropic_functional_mock_tests_objs STATIC
    ${TEST_SRCS}
)
target_link_libraries(libtropic_functional_mock_tests_objs PUBLIC tropic)
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${PATH_TO_LIBTROPIC}/src)
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common)
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/helpers)

set(LIBTROPIC_MOCK_TEST_LIST
    lt_test_mock_attrs
)

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
# Enable CTest.
enable_testing()

# Loop through tests defined in libtropic and prepare environment.
foreach(test_name IN LISTS LIBTROPIC_MOCK_TEST_LIST)

    # Create a correct macro from test name.
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})

    set(exe_name ${test_name})
    set(LIBTROPIC_MOCK_TEST_FUNCTION ${test_name})

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/main.in.c
        ${CMAKE_CURRENT_BINARY_DIR}/main.c
        @ONLY
    )

    # Define executable (separate for each test) and link dependencies.
    add_executable(${exe_name} 
        ${CMAKE_CURRENT_BINARY_DIR}/main.c
    )
    target_link_libraries(${exe_name} PUBLIC libtropic_functional_mock_tests_objs)
    # Include this (./) directory to include lt_functional_mock_tests.h with test functions declarations.
    target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Developers who integrate Libtropic do not have to use this variable
    # It's used in model's main.c to switch crypto contexts without manual changes
    # target_compile_definitions(${exe_name} PRIVATE
    #     LT_USE_TREZOR_CRYPTO=${LT_USE_TREZOR_CRYPTO}
    #     LT_USE_MBEDTLS_V4=${LT_USE_MBEDTLS_V4}
    # )

    # Add CTest entry.
    add_test(NAME ${test_name}
                COMMAND ./${exe_name}
    )

endforeach()