cmake_minimum_required(VERSION 3.21.0)
include (FetchContent)

###########################################################################
#                                                                         #
#   Set up projects and paths                                             #
#                                                                         #
###########################################################################
project(libtropic_hello_world
        VERSION 0.1.0
        DESCRIPTION "Libtropic Hello World example on Linux using USB devkit driver."
        LANGUAGES C)

set(PATH_LIBTROPIC ../../../../)

###########################################################################
#                                                                         #
#   Configuration                                                         #
#                                                                         #
###########################################################################
# Override path to USB devkit device
set(LT_USB_DEVKIT_PATH "/dev/ttyACM0" CACHE STRING "Path to the USB device representing the USB devkit serial port.")
message(STATUS "Using serial port: ${LT_USB_DEVKIT_PATH}. You can change it by passing -DLT_USB_DEVKIT_PATH=<path> to cmake.")

# Select pairing keys written during manufacturing into your TROPIC01
set(LT_SH0_KEYS "prod0" CACHE STRING "Choose which pairing keys in slot 0 will be used in this example")
set_property(CACHE LT_SH0_KEYS PROPERTY STRINGS "eng_sample" "prod0")

# These are internal macros for selecting SH0 keys (examples/tests)
set(LT_USE_SH0_ENG_SAMPLE 0 CACHE INTERNAL "")
set(LT_USE_SH0_PROD0      0 CACHE INTERNAL "")

# Define SH0 macros based on selected string (examples/tests)
if(LT_SH0_KEYS STREQUAL "eng_sample")
    message(STATUS "Using Engineering sample keys in examples/tests")
    set(LT_USE_SH0_ENG_SAMPLE 1)
    set(LT_USE_SH0_PROD0      0)
elseif(LT_SH0_KEYS STREQUAL "prod0")
    message(STATUS "Using Production 0 keys in examples/tests")
    set(LT_USE_SH0_ENG_SAMPLE 0)
    set(LT_USE_SH0_PROD0      1)
else()
    get_property(lt_sh0_keys_choices CACHE LT_SH0_KEYS PROPERTY STRINGS)
    message(FATAL_ERROR "Incorrect SH0 keys for examples/tests specified: '${LT_SH0_KEYS}'\nAvailable SH0 keys: ${lt_sh0_keys_choices}")
endif()

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

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)

# ------------------------------------------------------------------------
# External dependencies
# ------------------------------------------------------------------------

# MbedTLS v4.0.0
set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls_v4 test building.")
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbedtls_v4 examples building.")
FetchContent_Declare(
    mbedtls_v4
    URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2
    URL_HASH SHA256=2f3a47f7b3a541ddef450e4867eeecb7ce2ef7776093f3a11d6d43ead6bf2827
)
FetchContent_MakeAvailable(mbedtls_v4)
target_link_libraries(tropic PUBLIC mbedtls)

###########################################################################
#                                                                         #
#   Set up sources and compilation                                        #
#                                                                         #
###########################################################################
# Add MbedTLS v4 CAL
add_subdirectory("${PATH_LIBTROPIC}cal/mbedtls_v4" "mbedtls_v4_cal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

# Add SPI Linux HAL
add_subdirectory("${PATH_LIBTROPIC}hal/posix/usb_dongle" "posix_usb_dongle")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

# Add sources of this example
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Define executable, pass defines, and link dependencies.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE LT_USB_DEVKIT_PATH=\"${LT_USB_DEVKIT_PATH}\")
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE LT_USE_SH0_ENG_SAMPLE=${LT_USE_SH0_ENG_SAMPLE})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE LT_USE_SH0_PROD0=${LT_USE_SH0_PROD0})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE tropic)