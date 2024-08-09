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

add_executable(main main.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/platform/unix/ts_unix_l1.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/platform/unix/ts_unix_random.c
)


###########################################################################
#                                                                         #
#   Add executable and set definitions                                    #
#                                                                         #
###########################################################################

if(USE_TREZOR_CRYPTO)
    target_compile_definitions(main PRIVATE USE_TREZOR_CRYPTO)
endif()


###########################################################################
#                                                                         #
#   Add libtropic                                                         #
#                                                                         #
###########################################################################

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Customize trezor_crypto library compilation:
target_compile_definitions(trezor_crypto PRIVATE USE_INSECURE_PRNG)

target_link_options(main PRIVATE -Wl,--gc-sections)# -Wl,--strip-all)
target_link_libraries(main PRIVATE tropic)
