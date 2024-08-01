cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(libtropic_unix_example
        VERSION 0.1.0
        DESCRIPTION "Example of libtropic's usage in Unix environment"
        LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

###########################################################################
#                                                                         #
#   Configure libtropic for this exmaple                                  #
#                                                                         #
###########################################################################

# This example is compiled for linux, therefore use unix hal functions:
set(TS_HAL_UNIX ON)
# Use trezor crypto as a source of backend cryptography code
set(USE_TREZOR_CRYPTO ON)


###########################################################################
#                                                                         #
#   Add path to libtropic's repository root folder                        #
#                                                                         #
###########################################################################

add_subdirectory(../../ "libtropic")


###########################################################################
#                                                                         #
#   Building unix example                                                 #
#                                                                         #
###########################################################################

add_executable(main main.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/spi/ts_l1_unix_tcp.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../hal/system/ts_random_unix.c
)

target_compile_definitions(main PRIVATE USE_TREZOR_CRYPTO)
target_link_options(main PRIVATE -Wl,--gc-sections -Wl,--strip-all)
target_link_libraries(main PRIVATE tropic)
