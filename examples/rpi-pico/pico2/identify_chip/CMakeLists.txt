cmake_minimum_required(VERSION 3.21.0)

# Pico 2 (RP2350).
set(PICO_BOARD pico2 CACHE STRING "Pico SDK board target" FORCE)

if(NOT DEFINED PICO_SDK_PATH)
	message(FATAL_ERROR "PICO_SDK_PATH is not set. Export it before configuring this example.")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(
	libtropic_identify_chip
	DESCRIPTION "Libtropic chip identification example on Raspberry Pi Pico 2"
	LANGUAGES C)

pico_sdk_init()

if(NOT DEFINED PATH_LIBTROPIC)
	set(PATH_LIBTROPIC "../../../../")
endif()

# Add path to libtropic source.
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Add CAL and HAL used by this example.
add_subdirectory("${PATH_LIBTROPIC}cal/trezor_crypto" "trezor_crypto_cal")
add_subdirectory("${PATH_LIBTROPIC}hal/rpi-pico" "rpi_pico_hal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS} ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS} ${LT_HAL_INC_DIRS})
target_link_libraries(tropic PUBLIC pico_stdlib hardware_spi hardware_gpio pico_rand)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE tropic pico_stdlib hardware_spi hardware_gpio pico_rand)
# This ensures picotool can reboot into bootloader.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
	PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=1
	PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
	PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK=0
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
