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

target_compile_definitions(tropic PUBLIC LT_USE_SH0_PROD0=1) # Temp hack to use sh0 prod keys. This should handled more inteligently using CMake options.

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

# Define executable and link dependencies.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE tropic)