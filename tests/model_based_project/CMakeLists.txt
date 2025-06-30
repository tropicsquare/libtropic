cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

#set(CMAKE_BUILD_TYPE Debug)

set(PATH_LIBTROPIC "../../")


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
#   Sources defined by this project + definition of transport layer       #
#   from hal/port/ folder                                                 #
#                                                                         #
###########################################################################

add_executable(run main.c
            ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix_tcp.c
)


###########################################################################
#                                                                         #
#   Define what examples (or tests) will be compiled into main().         #
#   In this example libtropic library is compiled with examples           #
#   and tests functions exposed. In production it is recommended to       #
#   compile only core part of libtropic to save some code space.          #
#                                                                         #
#   This is controlled by options LT_BUILD_EXAMPLES and LT_BUILD_TESTS    #
#                                                                         #
###########################################################################

# Following two lines should be placed in next section,
# but let's keep them here for clarity:
set(LT_BUILD_EXAMPLES ON)
set(LT_BUILD_TESTS ON)

# -DLT_EX_HW_WALLET=1
if(LT_EX_HW_WALLET)
    target_compile_definitions(run PRIVATE LT_EX_HW_WALLET)
endif()
# -DLT_EX_HELLO_WORLD=1
if(LT_EX_HELLO_WORLD)
    target_compile_definitions(run PRIVATE LT_EX_HELLO_WORLD)
endif()
# -DLT_EX_HELLO_WORLD_SEPARATE_API=1
if(LT_EX_HELLO_WORLD_SEPARATE_API)
    target_compile_definitions(run PRIVATE LT_EX_HELLO_WORLD_SEPARATE_API)
endif()
# -DLT_EX_MACANDD=1
if(LT_EX_MACANDD)
    target_compile_definitions(run PRIVATE LT_EX_MACANDD)
endif()

# -DLT_TEST_CHIP_ID=1
if(LT_TEST_CHIP_ID)
    target_compile_definitions(run PRIVATE LT_TEST_CHIP_ID)
endif()
# -DLT_TEST_WRITE_PAIRING_KEYS=1
if(LT_TEST_WRITE_PAIRING_KEYS)
    target_compile_definitions(run PRIVATE LT_TEST_WRITE_PAIRING_KEYS)
endif()
# -DLT_TEST_WRITE_R_CONFIG=1
if(LT_TEST_WRITE_R_CONFIG)
    target_compile_definitions(run PRIVATE LT_TEST_WRITE_R_CONFIG)
endif()
# --DLT_TEST_PING=1
if(LT_TEST_PING)
    target_compile_definitions(run PRIVATE LT_TEST_PING)
endif()
# --DLT_TEST_ENGINEERING_SAMPLE_01_PING=1
if(LT_TEST_ENGINEERING_SAMPLE_01_PING)
    target_compile_definitions(run PRIVATE LT_TEST_ENGINEERING_SAMPLE_01_PING)
endif()
# --DLT_TEST_ECC_EDDSA=1
if(LT_TEST_ECC_EDDSA)
    target_compile_definitions(run PRIVATE LT_TEST_ECC_EDDSA)
endif()
# --DLT_TEST_R_MEM=1
if(LT_TEST_R_MEM)
    target_compile_definitions(run PRIVATE LT_TEST_R_MEM)
endif()
# --DLT_TEST_ERASE_R_CONFIG=1
if(LT_TEST_ERASE_R_CONFIG)
    target_compile_definitions(run PRIVATE LT_TEST_ERASE_R_CONFIG)
endif()

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Use trezor crypto as a source of backend cryptography code
set(LT_USE_TREZOR_CRYPTO ON)

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

target_link_options(run PRIVATE -Wl,--gc-sections)
target_link_libraries(run PRIVATE tropic)
