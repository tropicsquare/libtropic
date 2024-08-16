cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

#set(CMAKE_BUILD_TYPE Debug)

set(PATH_TREZOR_CRYPTO "../../vendor/trezor_crypto/")
set(PATH_LIBTROPIC "../../")

# Libtropic related setup
# Use trezor crypto as a source of backend cryptography code
set(USE_TREZOR_CRYPTO ON)

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(libtropic_unix_example
        VERSION 0.1.0
        DESCRIPTION "Example of libtropic's usage in Unix environment"
        LANGUAGES C)


###########################################################################
#                                                                         #
#   Sources                                                               #
#                                                                         #
###########################################################################

add_executable(integration_tests main.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/ts_port_unix.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}examples/libtropic_example_1.c
)
target_include_directories(integration_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}examples/)

###########################################################################
#                                                                         #
#   Add executable and set definitions                                    #
#                                                                         #
###########################################################################

if(USE_TREZOR_CRYPTO)
    target_compile_definitions(integration_tests PRIVATE USE_TREZOR_CRYPTO)
endif()


###########################################################################
#                                                                         #
#   Add libtropic                                                         #
#                                                                         #
###########################################################################

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -Wall)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
target_compile_options(tropic PRIVATE -Wno-implicit-function-declaration)

# Customize trezor_crypto library compilation:
target_compile_definitions(trezor_crypto PRIVATE USE_INSECURE_PRNG)

target_link_options(integration_tests PRIVATE -Wl,--gc-sections)
target_link_libraries(integration_tests PRIVATE tropic)
