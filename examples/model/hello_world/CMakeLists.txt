cmake_minimum_required(VERSION 3.21.0)
include (FetchContent)

###########################################################################
#                                                                         #
#   Set up projects and paths                                             #
#                                                                         #
###########################################################################
project(libtropic_hello_world
        DESCRIPTION "Libtropic Hello World example on model."
        LANGUAGES C)

set(PATH_LIBTROPIC ../../../)

###########################################################################
#                                                                         #
#   Configuration                                                         #
#                                                                         #
###########################################################################
if(NOT UNIX)
    message(FATAL_ERROR "Model is compatible with UNIX-like systems only for the moment.")
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
add_subdirectory("${PATH_LIBTROPIC}/cal/mbedtls_v4" "mbedtls_v4_cal")
target_sources(tropic PRIVATE ${LT_CAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_CAL_INC_DIRS})

# Add POSIX TCP HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/posix/tcp" "posix_tcp_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

# Add sources of this example
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Define executable, pass defines, and link dependencies.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE tropic)