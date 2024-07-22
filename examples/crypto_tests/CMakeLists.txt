cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(crypto_tests 
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Configure libtropic for this exmaple                                  #
#                                                                         #
###########################################################################

# This example is compiled for linux, so use unix hal functions:
#set(TS_HAL_UNIX ON)
# Use trezor crypto as a source of backend cryptography code
set(TS_CRYPTO_TREZOR ON)


###########################################################################
#                                                                         #
#   Add path to libtropic's repository root folder                        #
#                                                                         #
###########################################################################

add_subdirectory(../../ "src")


###########################################################################
#                                                                         #
#   Building crypto tests                                                 #
#                                                                         #
###########################################################################

add_executable(main main.c)
target_compile_definitions(main PRIVATE USE_TS_CRYPTO)
target_link_libraries(main PUBLIC tropic)
