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
#   Define what examples will be compiled into main() and if tests        #
#   should be built. In this example, libtropic library is compiled with  #
#   examples exposed. In production, it is recommended to compile only    #
#   core part of libtropic to save some code space.                       #
#                                                                         #
#   This is controlled by options LT_BUILD_EXAMPLES and LT_BUILD_TESTS    #
#                                                                         #
###########################################################################

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
target_compile_options(tropic PRIVATE -ffunction-sections -fdata-sections)


###########################################################################
#                                                                         #
# FUNCTIONAL TESTS CONFIGURATION                                          #
#                                                                         #
# This section will automatically configure CTest for launching tests     #
# defined in libtropic. Do NOT hardcode any test definitions here.        #
# Define them in libtropic.                                               #
#                                                                         #
# To build functional tests, use -DLT_BUILD_TESTS=1 in cmake invocation.  #
#                                                                         #
###########################################################################

if(LT_BUILD_TESTS)
    # Enable CTest.
    enable_testing()
    
    # So we can include preprocessed test registry (lt_test_registry.c.inc).
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/libtropic)

    # Loop through tests defined in libtropic and prepare environment.
    foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    
        # Create a correct macro from test name.
        string(TOUPPER ${test_name} test_macro)
        string(REPLACE " " "_" test_macro ${test_macro})

        set(exe_name ${test_name})

        # Define executable (separate for each test) and link dependencies.
        add_executable(${exe_name} main.c
            ${CMAKE_CURRENT_SOURCE_DIR}/${PATH_LIBTROPIC}hal/port/unix/lt_port_unix_tcp.c
        )
        target_link_libraries(${exe_name} PRIVATE tropic)

        # Enable test registry using LT_BUILD_TESTS and choose correct test for the binary.
        target_compile_definitions(${exe_name} PRIVATE LT_BUILD_TESTS)
        target_compile_definitions(${exe_name} PRIVATE ${test_macro})

        # Add CTest entry.
        add_test(NAME ${test_name}
                COMMAND python3 ../model_test_runner.py -t ${CMAKE_CURRENT_BINARY_DIR}/${exe_name} -o run_logs/
        )
    endforeach()
endif()

###########################################################################
#                                                                         #
#   Link executable                                                       #
#                                                                         #
###########################################################################

target_link_options(run PRIVATE -Wl,--gc-sections)
target_link_libraries(run PRIVATE tropic)
