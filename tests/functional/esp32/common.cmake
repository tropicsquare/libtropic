if (${CMAKE_VERSION} VERSION_GREATER "3.27")
    cmake_policy(SET CMP0152 OLD) # Path resolution policy
endif()

file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../_deps/ PATH_DEPS)
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

set(LT_TARGET_UART "/dev/ttyUSB0" CACHE STRING "Path to the UART file descriptor; used for flashing tests and getting logs.")
set(LT_BAUD_FLASH "921600" CACHE STRING "Baud rate for flashing the test binary.")
set(LT_BAUD_MONITOR "115200" CACHE STRING "Baud rate for serial monitor during testing.")

set(LT_CAL_LINK_DEPS OFF)

# ESP-IDF does not compile with strict flags.
set(LT_STRICT_COMPILATION OFF)

###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

# Trigger IDF build system for the selected target
idf_build_process(${TARGET})

###########################################################################
#                                                                         #
#   Add libtropic library and set it up                                   #
#                                                                         #
###########################################################################

# Add path to libtropic's functional tests
add_subdirectory(${PATH_FN_TESTS} "libtropic_functional_tests")

###########################################################################
#                                                                         #
#   Crypto backend handling                                               #
#                                                                         #
###########################################################################

# Handle CAL
if(LT_CAL STREQUAL "mbedtls_v4")
    target_link_libraries(tropic PUBLIC idf::mbedtls)
else()
    message(FATAL_ERROR "Unsupported CAL ${LT_CAL} for ESP-IDF builds!")
endif()


###########################################################################
#                                                                         #
#   SOURCES                                                               #
#   Define project sources.                                               #
#                                                                         #
###########################################################################

# Add ESP-IDF HAL
add_subdirectory("${PATH_LIBTROPIC}/hal/esp-idf" "esp_idf_hal")
target_sources(tropic PRIVATE ${LT_HAL_SRCS})
target_include_directories(tropic PUBLIC ${LT_HAL_INC_DIRS})
target_link_libraries(tropic PUBLIC idf::freertos idf::spi_flash idf::esp_driver_spi idf::esp_driver_gpio)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Enable strict compile flags for main.c and Libtropic HAL sources
if(LT_STRICT_COMPILATION)
    set_source_files_properties(${SOURCES} ${LT_HAL_SRCS} PROPERTIES COMPILE_OPTIONS "${LT_STRICT_COMPILATION_FLAGS}")   
endif()

###########################################################################
#                                                                         #
# FUNCTIONAL TESTS CONFIGURATION                                          #
#                                                                         #
# This section will automatically configure CTest for launching tests     #
# defined in Libtropic. Do NOT hardcode any test definitions here.        #
# Define them in common CMakeLists.txt for functional tests.              #
#                                                                         #
###########################################################################
# Enable CTest.
enable_testing()

# Create one executable per test in a single configure (single-tree mode).
# Guard to ensure ldgen target creation runs only once for this directory.
set(__IDF_LDGEN_FIRST_EXE 1)
foreach(test_name IN LISTS LIBTROPIC_TEST_LIST)
    string(TOUPPER ${test_name} test_macro)
    string(REPLACE " " "_" test_macro ${test_macro})

    set(exe_name ${test_name}.elf)

    add_executable(${exe_name} ${SOURCES})
    target_link_libraries(${exe_name} PRIVATE libtropic_functional_tests)

    # Make private headers of Libtropic available
    target_include_directories(${exe_name} PRIVATE "${PATH_LIBTROPIC}/src")

    # Place all per-test generated outputs into a per-test subdirectory to avoid collisions
    set_target_properties(${exe_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
    )
    # Only call idf_build_executable() once to avoid creating duplicate
    # ldgen custom targets (they use a fixed name per-directory). For the
    # first executable let IDF create the ldgen target; for subsequent
    # executables mimic idf_build_executable() behaviour except for the
    # ldgen creation step.
    if(__IDF_LDGEN_FIRST_EXE)
        idf_build_executable(${exe_name})
        set(__IDF_LDGEN_FIRST_EXE 0)
    else()
        # Append global link options and link dependencies collected by IDF
        idf_build_get_property(link_options LINK_OPTIONS)
        if(link_options)
            set_property(TARGET ${exe_name} APPEND PROPERTY LINK_OPTIONS "${link_options}")
        endif()
        idf_build_get_property(link_depends __LINK_DEPENDS)
        if(link_depends)
            set_property(TARGET ${exe_name} APPEND PROPERTY LINK_DEPENDS "${link_depends}")
        endif()

        # Set executable-related properties used by generator expressions
        get_filename_component(elf_name ${exe_name} NAME_WLE)
        get_target_property(elf_dir ${exe_name} BINARY_DIR)
        idf_build_set_property(EXECUTABLE_NAME ${elf_name})
        idf_build_set_property(EXECUTABLE ${exe_name})
        idf_build_set_property(EXECUTABLE_DIR "${elf_dir}")

        # Ensure the build target depends on IDF build target as idf_build_executable does
        # Also ensure the executable depends on ldgen output targets created by
        # the first idf_build_executable() call so the generated linker scripts
        # are available when building this executable.
        idf_build_get_property(ldgen_outputs __LDGEN_OUTPUTS)
        if(ldgen_outputs)
            foreach(ldout ${ldgen_outputs})
                get_filename_component(ldout_name ${ldout} NAME)
                set(_ldgen_target __ldgen_output_${ldout_name})
                if(TARGET ${_ldgen_target})
                    add_dependencies(${exe_name} ${_ldgen_target})
                endif()
            endforeach()
        endif()
        add_dependencies(${exe_name} __idf_build_target)
    endif()

    # Per-executable binary and flash args generation (use IDF esptool helpers)
    idf_build_get_property(build_dir BUILD_DIR)
    partition_table_get_partition_info(app_partition_offset "--partition-boot-default" "offset")
    get_filename_component(elf_name ${exe_name} NAME_WLE)
    set(test_build_dir "${build_dir}/${test_name}")
    file(MAKE_DIRECTORY "${test_build_dir}")
    set(_bin_out "${test_build_dir}/${elf_name}.bin")
    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        add_custom_command(TARGET ${exe_name} POST_BUILD
            COMMAND ${ESPTOOLPY} --chip ${TARGET} elf2image -o "${_bin_out}" "$<TARGET_FILE:${exe_name}>"
            VERBATIM
            WORKING_DIRECTORY ${test_build_dir}
        )
        add_custom_target(gen_project_binary_${test_name} DEPENDS ${exe_name})

        # Generate a per-test flash args file (flash_<test>_args) so esptool
        # can flash this specific test image. We avoid calling esptool_py
        # helper targets to prevent duplicate GENERATED files across
        # multiple invocations in this directory.
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        # Compose flash args content: flash sub args line followed by images
        set(flash_args_content "${sub_args}\n")
        # Determine offsets from IDF variables where available, otherwise use
        # commonly used defaults.
        if(NOT DEFINED BOOTLOADER_OFFSET)
            set(BOOTLOADER_OFFSET 0x1000)
        endif()
        if(NOT DEFINED PARTITION_TABLE_OFFSET)
            set(PARTITION_TABLE_OFFSET 0x8000)
        endif()
        # bootloader, app, partition table paths relative to build dir
        string(APPEND flash_args_content "${BOOTLOADER_OFFSET} ../bootloader/bootloader.bin\n")
        string(APPEND flash_args_content "${app_partition_offset} ${elf_name}.bin\n")
        string(APPEND flash_args_content "${PARTITION_TABLE_OFFSET} ../partition_table/partition-table.bin\n")
        file(WRITE "${test_build_dir}/flash_project_args" "${flash_args_content}")
    endif()

    # Choose correct test for the binary.
    target_compile_definitions(${exe_name} PRIVATE ${test_macro})

    if(CTEST_PREFIX STREQUAL "")
        set(TEST_NAME_WITH_PREFIX ${test_name})
    else()
        set(TEST_NAME_WITH_PREFIX ${CTEST_PREFIX}_${test_name})
    endif()

    # Use the unified run_test helper; pass top-level binary dir so the script
    # can locate the generated ELF and artifacts.
    add_test(NAME ${TEST_NAME_WITH_PREFIX}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../run_test.sh
            ${CMAKE_CURRENT_BINARY_DIR}/${test_name}/
            ${LT_TARGET_UART}
            ${TARGET}
            ${LT_BAUD_FLASH}
            ${LT_BAUD_MONITOR}
    )
endforeach()
