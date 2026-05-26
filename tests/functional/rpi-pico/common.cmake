cmake_minimum_required(VERSION 3.21.0)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW) # Timestamp extraction policy
endif()

if (${CMAKE_VERSION} VERSION_GREATER "3.27")
    cmake_policy(SET CMP0152 OLD) # Path resolution policy
endif()

file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../_deps/ PATH_DEPS)
file(REAL_PATH ../../../../ PATH_LIBTROPIC)
file(REAL_PATH ../../src PATH_FN_TESTS)

if (NOT EXISTS ${PATH_DEPS})
    message(FATAL_ERROR "Dependencies not installed. Please run download_deps.sh!")
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

# Serial number of the Pico board which will be used for flashing.
# You need to define this only if you have multiple Picos connected.
if (NOT DEFINED PICO_SERIAL_NUMBER)
    set(PICO_SERIAL_NUMBER "")
endif()

set(LT_CAL_LINK_DEPS OFF)
set(LT_STRICT_COMPILATION OFF)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

# Let pico_mbedtls provide MBEDTLS_CONFIG_FILE once (avoid duplicate -D defines).
# PICO_BOARD variable is used by Pico SDK and must be set in the Picos CMakeLists.txt.
set(PICO_MBEDTLS_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_config.h")

pico_sdk_init()

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to libtropic's functional tests
add_subdirectory(${PATH_FN_TESTS} "libtropic_functional_tests")

###########################################################################
#                                                                         #
#   Crypto backend handling                                               #
#                                                                         #
###########################################################################

# Handle CAL
if(LT_CAL STREQUAL "mbedtls_v4")
    target_link_libraries(tropic PUBLIC pico_mbedtls)
elseif(NOT LT_CAL STREQUAL "wolfcrypt")
    message(FATAL_ERROR "Unsupported CAL ${LT_CAL} for Pico testing!")
endif()

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

# Add RPi Pico HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/rpi-pico" "rpi_pico_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})
# Add other Pico SDK deps
target_link_libraries(tropic PUBLIC pico_stdlib hardware_spi hardware_gpio pico_rand)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Enable strict compile flags for main.c and Libtropic HAL sources
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${SOURCES} ${LT_HAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")   
endif()

###########################################################################
#                                                                         #
# FUNCTIONAL TESTS CONFIGURATION                                          #
#                                                                         #
# This section will automatically configure CTest for launching tests     #
# defined in libtropic. Do NOT hardcode any test definitions here.        #
# Define them in common CMakeLists.txt for functional tests.              #
#                                                                         #
###########################################################################
# Enable CTest.
enable_testing()

# Loop through tests defined in libtropic and prepare environment.
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
    target_compile_definitions(${exe_name} PRIVATE
        # These ensure picotool can reboot into bootloader.	
        PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=1
        PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
        PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK=0
    )

    pico_enable_stdio_usb(${exe_name} 1)
    pico_enable_stdio_uart(${exe_name} 0)
    pico_add_extra_outputs(${exe_name})

    # Some Mbed TLS PSA helper sources are not exposed via the pico_mbedtls target
    # in some SDK versions, so we include them directly.
    # This issue is fixed by this PR: https://github.com/raspberrypi/pico-sdk/pull/2711
    # However, the fix is not part of any release version yet.
    
    if(LT_CAL STREQUAL "mbedtls_v4" AND DEFINED PICO_SDK_PATH)
        set(PICO_MBEDTLS_LIB_DIR "${PICO_SDK_PATH}/lib/mbedtls/library")
        if(EXISTS ${PICO_MBEDTLS_LIB_DIR}/psa_util.c)
            target_sources(${exe_name} PRIVATE
                ${PICO_MBEDTLS_LIB_DIR}/psa_util.c
                ${PICO_MBEDTLS_LIB_DIR}/psa_crypto_ffdh.c
                ${PICO_MBEDTLS_LIB_DIR}/psa_crypto_driver_wrappers_no_static.c
            )
        endif()
    endif()

    if(CTEST_PREFIX STREQUAL "")
        set(TEST_NAME_WITH_PREFIX ${test_name})
    else()
        set(TEST_NAME_WITH_PREFIX ${CTEST_PREFIX}_${test_name})
    endif()

    # Add CTest entry.
    add_test(NAME ${TEST_NAME_WITH_PREFIX}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../run_test.sh ${CMAKE_CURRENT_BINARY_DIR} ${test_name} ${PICO_SERIAL_NUMBER}
    )
endforeach()