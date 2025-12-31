cmake_minimum_required(VERSION 3.21.0)

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################
project(libtropic_functional_tests_linux_usb
        VERSION 0.1.0
        DESCRIPTION "Functional tests in Linux environment using USB dongle driver"
        LANGUAGES C)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################
file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/_deps/ PATH_DEPS)
file(REAL_PATH ../../../../ PATH_LIBTROPIC)
file(REAL_PATH ../../src PATH_FN_TESTS)

if (NOT EXISTS ${PATH_DEPS})
    message(FATAL_ERROR "Dependencies not installed. Please run download_deps.sh!")
endif()

###########################################################################
#                                                                         #
#   Options and user configuration                                        #
#                                                                         #
###########################################################################

# Optional prefix to tests registered to CTest. Useful when running same test against
# different chips to differentiate them by their name in JUnit output.
if (NOT DEFINED CTEST_PREFIX)
    set(CTEST_PREFIX "")
endif()

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to libtropic's repository root folder
add_subdirectory(${PATH_FN_TESTS} "libtropic_functional_tests")

###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

# Add POSIX USB dongle HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/posix/usb_dongle" "posix_usb_dongle_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Enable strict compile flags for main.c and Libtropic HAL sources
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/main.c ${LT_HAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")   
endif()

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
# Enable CTest.
enable_testing()

# Loop through tests defined in libtropic and prepare environment.
foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    # Create a correct macro from test name.
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})

    set(exe_name ${test_name})

    # Define executable (separate for each test) and link dependencies.
    add_executable(${exe_name} ${SOURCES})
    target_link_libraries(${exe_name} PRIVATE libtropic_functional_tests)

    # Enable test registry using LT_BUILD_TESTS and choose correct test for the binary.
    target_compile_definitions(${exe_name} PRIVATE ${test_macro})

    if(CTEST_PREFIX STREQUAL "")
        set(TEST_NAME_WITH_PREFIX ${test_name})
    else()
        set(TEST_NAME_WITH_PREFIX ${CTEST_PREFIX}_${test_name})
    endif()

    # Add CTest entry.
    add_test(NAME ${TEST_NAME_WITH_PREFIX}
            COMMAND stdbuf -oL -eL ${CMAKE_CURRENT_BINARY_DIR}/${exe_name}
    )
endforeach()