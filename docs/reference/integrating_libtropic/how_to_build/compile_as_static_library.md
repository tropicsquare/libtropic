# Compile as a Static Library
Apart from building Libtropic during your project's build process, you can build Libtropic separately as a static library (also known as a static archive on Linux) and link it later.

!!! warning "HAL and CAL Files Handling"
    The Libtropic static library does not contain HALs (`libtropic/hal/`) or CALs (`libtropic/cal/`). The consumer must provide these:
    
    1. If CMake is used, inspiration can be taken from the steps in the [Add to an Existing Project](./adding_to_project.md) section.
    2. In other cases, the HAL and CAL files will have to be added manually.

## Compilation
!!! example "Compiling Libtropic as a static library on Unix-like system"
    Create a `build/` directory and switch to it:
    ```bash { .copy }
    mkdir build/
    cd build/
    ```
    Build Libtropic:
    ```bash { .copy }
    cmake ..
    make
    ```

To cross-compile Libtropic as a static library on a Unix-like system, you need a toolchain configuration file and a linker script. Both should be provided by the vendor of your platform. See an example of the toolchain configuration and linker script e.g. in `examples/stm32/hello_world/`.

Follow these steps after acquiring both the toolchain configuration and the linker script:

!!! example "Cross compiling Libtropic as a static library on a Unix-like system"
    Create a `build/` directory and switch to it:
    ```bash { .copy }
    mkdir build/
    cd build/
    ```
    Build Libtropic:
    ```bash { .copy }
    cmake -DCMAKE_TOOLCHAIN_FILE=<ABSOLUTE PATH>/toolchain.cmake \
          -DLINKER_SCRIPT=<ABSOLUTE PATH>/linker_script.ld ..
    make
    ```

## Linking in a CMake Project
To link the compiled static library to your application, use [`target_link_libraries`](https://cmake.org/cmake/help/latest/command/target_link_libraries.html). For example:

!!! example "Linking a static library in a CMake project"
    ```cmake { .copy }
    add_executable(my_app source1.c source2.c etc.c)
    target_link_libraries(my_app <absolute path to library file>)
    ```

!!! info "Other Linking Options"
    There are other options for linking the library (e.g., imported targets). Refer to the [CMake documentation](https://cmake.org/cmake/help/latest/index.html) for more information.

## Linking in a Make Project
If you are using `make` with a Makefile, you can include the static library by adding the following lines to your Makefile:

!!! example "Linking Libtropic static library in a Make project"
    ```makefile { .copy }
    LDFLAGS += -L<directory where the static library file is located>
    LDLIBS  += -ltropic
    ```

!!! info "Linking External Libraries"
    Refer to the [GNU Make documentation](https://www.gnu.org/software/make/manual/html_node/index.html) for more information about linking external libraries.