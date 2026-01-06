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
include(${PATH_TO_LIBTROPIC}cmake/strict_compile_flags.cmake)
include(${PATH_TO_LIBTROPIC}cmake/static_asan_flags.cmake)

###########################################################################
#                                                                         #
#   Options                                                               #
#                                                                         #
###########################################################################

option(LT_STRICT_COMPILATION "Enable strict compilation flags" ON)
option(LT_VALGRIND "Run tests with Valgrind" OFF)
option(LT_ASAN "Enable static AddressSanitizer (ASan)" OFF)

if (${LT_ASAN} AND ${LT_VALGRIND})
    message(WARNING "Using Valgrind with ASan enabled may lead to unexpected behavior.")
endif()

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
if (NOT LT_LOG_LVL)
    message(STATUS "No logging level specified, defaulting to INFO for tests.")
    set(LT_LOG_LVL "Info" CACHE BOOL "Set log level to INFO." FORCE)
endif()
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

if(LT_STRICT_COMPILATION)
    message(STATUS "Enabling strict compilation flags.")
    target_link_libraries(tropic PRIVATE libtropic::strict_comp_flags)
endif()

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
if(LT_STRICT_COMPILATION)
    target_link_libraries(libtropic_functional_mock_tests_objs PRIVATE libtropic::strict_comp_flags)
endif()
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${PATH_TO_LIBTROPIC}/src)
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common)
target_include_directories(libtropic_functional_mock_tests_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/helpers)

set(LIBTROPIC_MOCK_TEST_LIST
    lt_test_mock_attrs
    lt_test_mock_invalid_in_crc
    lt_test_mock_hardware_fail
)

###########################################################################
#                                                                         #
# FUNCTIONAL MOCK TESTS CONFIGURATION                                     #
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
    set(TEST_SPECIFIC_MAIN main.${test_name}.c)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c.in
        ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SPECIFIC_MAIN}
        @ONLY
    )

    # Define executable (separate for each test) and link dependencies.
    add_executable(${exe_name} 
        ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SPECIFIC_MAIN}
    )
    target_link_libraries(${exe_name} PUBLIC libtropic_functional_mock_tests_objs)
    
    if(LT_STRICT_COMPILATION)
        target_link_libraries(${exe_name} PRIVATE libtropic::strict_comp_flags)
    endif()

    # Include this (./) directory to include lt_functional_mock_tests.h with test functions declarations.
    target_include_directories(${exe_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Add CTest entry.
    if (LT_VALGRIND)
        add_test(NAME ${test_name} COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=1 ./${exe_name})
    else()
        add_test(NAME ${test_name} COMMAND ./${exe_name})
    endif()
endforeach()