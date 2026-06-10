cmake_minimum_required(VERSION 3.21.0)

# Set the Pico board (RP2040).
set(PICO_BOARD "pico")

# Pico SDK must be imported before project() so toolchain and language rules are set correctly.
if(NOT DEFINED ENV{PICO_SDK_PATH})
	message(FATAL_ERROR "PICO_SDK_PATH is not set. Please set the PICO_SDK_PATH environment variable, e.g. 'export PICO_SDK_PATH=<path_to_pico_sdk>'.")
else()
	set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(libtropic_functional_tests_rpi_pico
		DESCRIPTION "Functional tests for Raspberry Pi Pico using Pico SDK."
		LANGUAGES C CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	message(WARNING "Detected CMAKE_BUILD_TYPE=Release. Forcing -O1 for pico_mbedtls to bypass the RP2040 optimization bug: https://github.com/raspberrypi/pico-sdk/issues/2633")
	target_compile_options(pico_mbedtls INTERFACE "-O1")
endif()