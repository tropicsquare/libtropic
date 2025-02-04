cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

#set(CMAKE_BUILD_TYPE Debug)

set(PATH_TREZOR_CRYPTO "../../vendor/trezor_crypto/")
set(PATH_LIBTROPIC "../../../")


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
#   Sources defined by this project                                       #
#                                                                         #
###########################################################################

add_executable(integration_tests main.c
            ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix.c
)


###########################################################################
#                                                                         #
#   Define what examples will be compiled into main().                    #
#   If any of examples is enabled, libtropic is compiled with             #
#   examples exposed.                                                     #
#                                                                         #
###########################################################################

# -DLT_EX_HW_WALLET=1
if(LT_EX_HW_WALLET)
    set(LT_ADD_EXAMPLES ON)
    target_compile_definitions(integration_tests PRIVATE LT_EX_HW_WALLET)
endif()
# -DLT_EX_HELLO_WORLD=1
if(LT_EX_HELLO_WORLD)
    set(LT_ADD_EXAMPLES ON)
    target_compile_definitions(integration_tests PRIVATE LT_EX_HELLO_WORLD)
endif()
# -DLT_EX_TEST_REVERSIBLE=1
if(LT_EX_TEST_REVERSIBLE)
    set(LT_ADD_EXAMPLES ON)
    target_compile_definitions(integration_tests PRIVATE LT_EX_TEST_REVERSIBLE)
endif()
# --DLT_EX_TEST_IREVERSIBLE=1
if(LT_EX_TEST_IREVERSIBLE)
    set(LT_ADD_EXAMPLES ON)
    target_compile_definitions(integration_tests PRIVATE LT_EX_TEST_IREVERSIBLE)
endif()

###########################################################################
#                                                                         #
#   Add libtropic and set it up                                           #
#                                                                         #
###########################################################################

# Use trezor crypto as a source of backend cryptography code
set(USE_TREZOR_CRYPTO ON)

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_LIBTROPIC} "libtropic")

# Customize libtropic's compilation
target_compile_options(tropic PRIVATE -Wall)
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)
target_compile_options(tropic PRIVATE -Wno-implicit-function-declaration)


###########################################################################
#                                                                         #
#   Link executable                                                       #
#                                                                         #
###########################################################################

target_link_options(integration_tests PRIVATE -Wl,--gc-sections)
target_link_libraries(integration_tests PRIVATE tropic)
