cmake_minimum_required(VERSION 3.21.0)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW) # Timestamp extraction policy
endif()

###########################################################################
#                                                                         #
#   Set up projects and paths                                             #
#                                                                         #
###########################################################################
# Pico (RP2040).
set(PICO_BOARD pico CACHE STRING "Pico SDK board target" FORCE)

if(NOT DEFINED ENV{PICO_SDK_PATH})
	message(FATAL_ERROR "PICO_SDK_PATH is not set. Please set the PICO_SDK_PATH environment variable, e.g. 'export PICO_SDK_PATH=<path_to_pico_sdk>'.")
else()
	set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(libtropic_hello_world
		DESCRIPTION "Libtropic Hello World example on Raspberry Pi Pico."
		LANGUAGES C CXX)

# Let pico_mbedtls provide MBEDTLS_CONFIG_FILE once (avoid duplicate -D defines).
set(PICO_MBEDTLS_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_config.h")

pico_sdk_init()

if(NOT DEFINED PATH_LIBTROPIC)
	set(PATH_LIBTROPIC "../../../../")
endif()

###########################################################################
#                                                                         #
#   Configuration                                                         #
#                                                                         #
###########################################################################

# Select pairing keys written during manufacturing into your TROPIC01
set(LT_SH0_KEYS "prod0" CACHE STRING "Choose which pairing keys in slot 0 will be used in this example")
set_property(CACHE LT_SH0_KEYS PROPERTY STRINGS "eng_sample" "prod0")

# These are internal macros for selecting SH0 keys
set(LT_USE_SH0_ENG_SAMPLE 0 CACHE INTERNAL "")
set(LT_USE_SH0_PROD0      0 CACHE INTERNAL "")

# Define SH0 macros based on selected string
if(LT_SH0_KEYS STREQUAL "eng_sample")
    message(STATUS "Using Engineering sample keys")
    set(LT_USE_SH0_ENG_SAMPLE 1)
    set(LT_USE_SH0_PROD0      0)
elseif(LT_SH0_KEYS STREQUAL "prod0")
    message(STATUS "Using Production 0 keys")
    set(LT_USE_SH0_ENG_SAMPLE 0)
    set(LT_USE_SH0_PROD0      1)
else()
    get_property(lt_sh0_keys_choices CACHE LT_SH0_KEYS PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect SH0 keys specified: '${LT_SH0_KEYS}'\nAvailable SH0 keys: ${lt_sh0_keys_choices}")
endif()

###########################################################################
#                                                                         #
#   Set up dependencies                                                   #
#                                                                         #
###########################################################################

# ------------------------------------------------------------------------
# Libtropic 
# ------------------------------------------------------------------------
# Add path to libtropic source.
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

###########################################################################
#                                                                         #
#   Sources                                                               #
#                                                                         #
###########################################################################

# Add MbedTLS v4 CAL
add_subdirectory("${PATH_LIBTROPIC}cal/mbedtls_v4" "mbedtls_v4_cal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

# Add Libtropic Raspberry Pi Pico HAL
add_subdirectory("${PATH_LIBTROPIC}hal/rpi-pico" "rpi_pico_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

# Add other Pico SDK deps
target_link_libraries(tropic PUBLIC pico_stdlib hardware_spi hardware_gpio pico_rand pico_mbedtls)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE tropic pico_stdlib hardware_spi hardware_gpio pico_rand)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
	# These ensure picotool can reboot into bootloader.	
	PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=1
	PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
	PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK=0
	# Pairing keys macros
	LT_USE_SH0_ENG_SAMPLE=${LT_USE_SH0_ENG_SAMPLE}
	LT_USE_SH0_PROD0=${LT_USE_SH0_PROD0}
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
