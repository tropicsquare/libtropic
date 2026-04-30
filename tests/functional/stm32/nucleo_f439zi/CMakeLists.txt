cmake_minimum_required(VERSION 3.21.0)
if (${CMAKE_VERSION} VERSION_GREATER "3.27")
    cmake_policy(SET CMP0152 OLD) # Path resolution policy
endif()

include (FetchContent)

###########################################################################
#                                                                         #
#   Set up project and paths                                              #
#                                                                         #
###########################################################################

# Specify stm32 device
set(STM32_DEVICE "STM32F439xx")
# Openocd config file for used nucleo board
set(OPENOCD_CFG "/usr/share/openocd/scripts/board/stm32f429discovery.cfg")
# Path to linker script file
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITX_FLASH.ld)
# Path to toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)
# Paths to STM32 drivers.
# We provide a stripped-down snapshot of STM32CubeF4 repo in this example.
set(PATH_TO_STM32CUBEF4 ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32CubeF4/Drivers )

# Path to Libtropic repository root
file(REAL_PATH ../../../../ PATH_LIBTROPIC)
# Path to functional tests sources directory
file(REAL_PATH ../../src/ PATH_FN_TESTS)
# Path to external dependencies
file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../_deps/ PATH_DEPS)

if (NOT EXISTS ${PATH_DEPS})
    message(FATAL_ERROR "Dependencies not installed. Please run download_deps.sh!")
endif()

project(
    libtropic_functional_tests_stm32_nucleo_f439zi
    DESCRIPTION "Functional tests on STM32 Nucleo F439ZI board"
    LANGUAGES C ASM)

###########################################################################
#                                                                         #
#   Configuration                                                         #
#                                                                         #
###########################################################################

if (LT_CAL STREQUAL "openssl")
    message(FATAL_ERROR "OpenSSL is not supported on STM32!")
endif()

# Serial number of the STLink device which will be used for flashing.
# You need to define this only if you have multiple STM32s connected using built-in STLink,
# or if you encounter problems with OpenOCD's autodetection.
if (NOT DEFINED STLINK_SERIAL_NUMBER)
    set(STLINK_SERIAL_NUMBER "")
endif()

# Optional prefix to tests registered to CTest. Useful when running same test against
# different chips to differentiate them by their name in JUnit output.
if (NOT DEFINED CTEST_PREFIX)
    set(CTEST_PREFIX "")
endif()

###########################################################################
#                                                                         #
#   Add Libtropic functional tests and set them up                        #
#                                                                         #
###########################################################################

# MbedTLSv4 requires special configuration
if (LT_CAL STREQUAL "mbedtls_v4")
    # We configure MbedTLS using config file with following configuration:
    # - config.py preset "crypto_baremetal"
    # - following options enabled:
    #   MBEDTLS_PLATFORM_MS_TIME_ALT
    #   MBEDTLS_HAVE_TIME
    #   MBEDTLS_PSA_DRIVER_GET_ENTROPY
    # - following options disabled:
    #   MBEDTLS_PSA_BUILTIN_GET_ENTROPY
    #   MBEDTLS_TEST_HOOKS
    #   MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
    # We need to set both CMake variables and compile definitions for MbedTLS to pick up our config file.
    set(MBEDTLS_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Src/mbedtls_v4/mbedtls_config.h")
    set(TF_PSA_CRYPTO_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Src/mbedtls_v4/crypto_config.h")
    add_compile_definitions(MBEDTLS_CONFIG_FILE="${MBEDTLS_CONFIG_FILE}")
    add_compile_definitions(TF_PSA_CRYPTO_CONFIG_FILE="${TF_PSA_CRYPTO_CONFIG_FILE}")
endif()

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_FN_TESTS} "libtropic_functional_tests")

# Additional configuration for WolfCrypt.
if(LT_CAL STREQUAL "wolfcrypt")
    target_compile_definitions(wolfssl PUBLIC WOLFSSL_USER_SETTINGS)
    # Use BUILD_INTERFACE to strictly limit this path to the build phase
    target_include_directories(wolfssl PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Src/wolfcrypt>
    )
endif()

###########################################################################
#                                                                         #
#   Sources                                                               #
#                                                                         #
###########################################################################

# Add Libtropic STM32 HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/stm32/stm32f4xx" "stm32f4xx_hal")

# Specify other source files
set(SOURCES
    # STM32 vendor HAL, BSP
    ${PATH_TO_STM32CUBEF4}/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c

    # Project sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/startup_stm32f429zitx.s
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c

    # STM32 HAL
    ${LT_HAL_SRCS}
)

# Append special sources.
if (LT_CAL STREQUAL "mbedtls_v4")
    set(SOURCES ${SOURCES}
        # MbedTLS platform-specific implementations
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/mbedtls_v4/mbedtls_platform.c
    )
elseif (LT_CAL STREQUAL "wolfcrypt")
    set(SOURCES ${SOURCES}
        # WolfCrypt platform-specific implementations
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/wolfcrypt/wolfcrypt_platform.c
    )
endif()

# Include path for directories containing header files
set(INCLUDE_DIRS
    # STM32 vendor HAL, BSP
    ${PATH_TO_STM32CUBEF4}/CMSIS/Core/Include/
    ${PATH_TO_STM32CUBEF4}/CMSIS/Device/ST/STM32F4xx/Include/
    ${PATH_TO_STM32CUBEF4}/BSP/STM32F4xx_Nucleo_144/
    ${PATH_TO_STM32CUBEF4}/STM32F4xx_HAL_Driver/Inc/

    # Project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/

    # STM32 HAL
    ${LT_HAL_INC_DIRS}
)

# Enable strict compile flags for main.c and Libtropic HAL sources
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c ${LT_HAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")   
endif()


###########################################################################
#                                                                         #
#   Configure test building                                               #
#                                                                         #
###########################################################################
# Enable CTest.
enable_testing()

# Loop through tests defined in libtropic and prepare environment.
foreach(TEST_NAME IN LISTS LIBTROPIC_TEST_LIST)
    
    # Create a correct macro from test name.
    string(TOUPPER ${TEST_NAME} TEST_MACRO)
    string(REPLACE " " "_" TEST_MACRO ${TEST_MACRO})

    set(EXE_NAME ${TEST_NAME}.elf)

    # Define executable (separate for each test) and link dependencies.
    add_executable(${EXE_NAME} ${SOURCES})
    target_link_libraries(${EXE_NAME} PRIVATE libtropic_functional_tests)

    # Make private headers of Libtropic available
    target_include_directories(${EXE_NAME} PRIVATE "${PATH_LIBTROPIC}/src")

    target_compile_definitions(${EXE_NAME} PRIVATE -D${STM32_DEVICE}) # Configure target MCU (used by STM32 HAL)
    target_include_directories(${EXE_NAME} PRIVATE ${INCLUDE_DIRS})
    # Choose correct test for the binary.
    target_compile_definitions(${EXE_NAME} PRIVATE ${TEST_MACRO})
 
    if(CTEST_PREFIX STREQUAL "")
        set(TEST_NAME_WITH_PREFIX ${TEST_NAME})
    else()
        set(TEST_NAME_WITH_PREFIX ${CTEST_PREFIX}_${TEST_NAME})
    endif()

    # Add CTest entry.
    add_test(NAME ${TEST_NAME_WITH_PREFIX}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME} ${STLINK_SERIAL_NUMBER}
    )

endforeach()
