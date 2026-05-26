cmake_minimum_required(VERSION 3.21.0)

# Set the Pico board (RP2350).
set(PICO_BOARD "pico2")

# Pico SDK must be imported before project() so toolchain and language rules are set correctly.
if(NOT DEFINED ENV{PICO_SDK_PATH})
	message(FATAL_ERROR "PICO_SDK_PATH is not set. Please set the PICO_SDK_PATH environment variable, e.g. 'export PICO_SDK_PATH=<path_to_pico_sdk>'.")
else()
	set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(libtropic_functional_tests_rpi_pico
		DESCRIPTION "Functional tests for Raspberry Pi Pico 2 using Pico SDK."
		LANGUAGES C CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)