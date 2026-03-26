cmake_minimum_required(VERSION 3.22)
include (FetchContent)

###########################################################################
#                                                                         #
#   Set up project, paths, configuration                                  #
#                                                                         #
###########################################################################
# Path to toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/gcc-arm-none-eabi.cmake)

# Path to STM32 drivers.
# We provide a stripped-down snapshot of STM32CubeU5 repo in this example.
set(PATH_TO_STM32CUBEU5 ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32CubeU5/Drivers)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Set path to Libtropic
set(PATH_LIBTROPIC ../../../../..)
include(${PATH_LIBTROPIC}/cmake/strict_compile_flags.cmake)

# Enable strict compilation flags
set(LT_STRICT_COMPILATION ON CACHE BOOL "Enable strict compilation")

# Path to protobuf schema used for USB DevKit command/response exchange.
set(PATH_PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../protobuf)

# Set the project name
set(CMAKE_PROJECT_NAME tropic01_util)

# Default to Release build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

project(
    ${CMAKE_PROJECT_NAME}
    DESCRIPTION "TROPIC01 Utility application for TROPIC01 USB DevKit using Libtropic."
    LANGUAGES C ASM)

# Select MCU family for STM32 CMSIS/HAL headers.
add_compile_definitions(STM32U535xx)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Also generate .bin
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)

###########################################################################
#                                                                         #
#   Set up dependencies                                                   #
#                                                                         #
###########################################################################

# ------------------------------------------------------------------------
# Libtropic 
# ------------------------------------------------------------------------
# Add path to Libtropic source
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# ------------------------------------------------------------------------
# MbedTLS v4.0.0
# ------------------------------------------------------------------------
# 1. Download
FetchContent_Declare(
    mbedtls_v4
    URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2
    URL_HASH SHA256=2f3a47f7b3a541ddef450e4867eeecb7ce2ef7776093f3a11d6d43ead6bf2827
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# 2. We configure MbedTLS using config file with following configuration:
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
set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls_v4 test building.")
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbedtls_v4 examples building.")
FetchContent_MakeAvailable(mbedtls_v4)
target_link_libraries(tropic PUBLIC mbedtls)

# ------------------------------------------------------------------------
# TinyUSB 0.20.0
# ------------------------------------------------------------------------
FetchContent_Declare(
    tinyusb
    GIT_REPOSITORY https://github.com/hathach/tinyusb.git
    # TinyUSB release 0.20.0 commit. Upstream does not publish archive SHA256.
    GIT_TAG 3af1bec1a9161ee8dec29487831f7ac7ade9e189
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tinyusb)

# ------------------------------------------------------------------------
# nanopb (protobuf for embedded C)
# ------------------------------------------------------------------------
FetchContent_Declare(
    nanopb
    URL https://github.com/nanopb/nanopb/releases/download/nanopb-0.4.9.1/nanopb-0.4.9.1.tar.gz
    URL_HASH SHA256=882cd8473ad932b24787e676a808e4fb29c12e086d20bcbfbacc66c183094b5c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nanopb)

# Use the official nanopb CMake module and helper function.
list(APPEND CMAKE_MODULE_PATH ${nanopb_SOURCE_DIR}/extra)
find_package(Nanopb REQUIRED)

set(PROTO_FILE ${PATH_PROTOBUF_DIR}/usb_devkit_messages.proto)
nanopb_generate_cpp(TARGET proto ${PROTO_FILE})

# Generate Python protobuf bindings for host-side testing utility.
find_program(PROTOC_EXECUTABLE protoc REQUIRED)
set(PY_PROTO_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tropic01_util_app/protobuf)
set(PY_PROTO_FILE ${PY_PROTO_OUT_DIR}/usb_devkit_messages_pb2.py)
find_program(PROTOC_GEN_MYPY protoc-gen-mypy)
if(NOT PROTOC_GEN_MYPY)
    message(WARNING "protoc plugin 'protoc-gen-mypy' not found. Install mypy-protobuf to generate .pyi stubs for protobuf: pip install mypy-protobuf")
endif()
add_custom_command(
    OUTPUT ${PY_PROTO_FILE}
    COMMAND ${PROTOC_EXECUTABLE}
            --proto_path=${PATH_PROTOBUF_DIR}
            --python_out=${PY_PROTO_OUT_DIR}
            $<$<BOOL:${PROTOC_GEN_MYPY}>:--mypy_out=${PY_PROTO_OUT_DIR}>
            ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating Python protobuf bindings: usb_devkit_messages_pb2.py"
    VERBATIM
)
add_custom_target(proto_py ALL DEPENDS ${PY_PROTO_FILE})

###########################################################################
#                                                                         #
#   Sources                                                               #
#                                                                         #
###########################################################################

# Add MbedTLS v4 CAL
add_subdirectory("${PATH_LIBTROPIC}/cal/mbedtls_v4" "mbedtls_v4_cal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

# Add STM32U5xx HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/stm32/stm32u5xx" "stm32u5xx_hal")

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # STM32
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_crc_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dcache.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rng_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c

    # Project sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32u5xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32u5xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/system_stm32u5xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/startup_stm32u535cctx.s
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/usb_descriptors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/frame_protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/app_cmd_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/pin_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/pin_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/r_mem_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/r_mem_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/r_mem_erase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/raw_cmd_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/get_gpo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/send_spi_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/set_auto_cs_mode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/set_cs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/raw_cmd/set_tr01_pwr.c
    

    # MbedTLS platform-specific implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/mbedtls_v4/mbedtls_platform.c

    # Libtropic STM32 HAL
    ${LT_HAL_SRCS}

    # TinyUSB
    ${tinyusb_SOURCE_DIR}/src/tusb.c
    ${tinyusb_SOURCE_DIR}/src/common/tusb_fifo.c
    ${tinyusb_SOURCE_DIR}/src/device/usbd.c
    ${tinyusb_SOURCE_DIR}/src/device/usbd_control.c
    ${tinyusb_SOURCE_DIR}/src/class/cdc/cdc_device.c
    ${tinyusb_SOURCE_DIR}/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
)

# Enable strict compile flags for Libtropic HAL/CAL sources when requested.
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${LT_HAL_SRCS} ${LT_CAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")

    # Do not enforce prototypes on main.c.
    set(LT_STRICT_COMPILATION_FLAGS_NO_MISSING_PROTOTYPES ${LT_STRICT_COMPILATION_FLAGS})
    list(REMOVE_ITEM LT_STRICT_COMPILATION_FLAGS_NO_MISSING_PROTOTYPES "-Wmissing-prototypes")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS_NO_MISSING_PROTOTYPES}")
endif()

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # STM32
    ${PATH_TO_STM32CUBEU5}/CMSIS/Device/ST/STM32U5xx/Include/
    ${PATH_TO_STM32CUBEU5}/CMSIS/Include/
    ${PATH_TO_STM32CUBEU5}/STM32U5xx_HAL_Driver/Inc/

    # Project includes
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/

    # Libtropic STM32 HAL
    ${LT_HAL_INC_DIRS}

    # TinyUSB
    ${tinyusb_SOURCE_DIR}/src/
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    tropic
    proto
)

# Keep generated host protobuf in sync with schema during firmware builds.
add_dependencies(${CMAKE_PROJECT_NAME} proto_py)