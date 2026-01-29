# Adding a New Host Platform
Libtropic is written to be *platform-independent*, so no changes to the main code base are needed when adding support for a new host platform. However, to define how communication on the L1 Layer will work, a new Hardware Abstraction Layer (HAL) must be implemented. Currently available HALs are located in `hal/`.

## Guide
This guide will walk you through adding support for a new platform. In this guide, we will add a support for a microcontroller called `my_mcu` on a board `my_board`. The directory structure will be different if you create a port e.g., for an operating system. In that case, please get inspired by existing ports (POSIX, Linux).

To add support for a our new platform (`my_board` with `my_mcu`):

1. [Create and Implement the HAL C Files](#create-and-implement-the-hal-c-files),
2. [Create and Implement the HAL CMakeLists.txt](#create-and-implement-the-hal-cmakeliststxt),
3. [Provide Some Information About the HAL](#provide-some-information-about-the-hal).

!!! tip "Get Inspired by Existing HALs"
    For inspiration, see the existing HALs inside `hal/`.

After these steps, the sources and include directories of the new HAL should be available in consumer's `CMakeLists.txt` by calling:
```cmake { .copy }
add_subdirectory("<path_to_libtropic>/hal/my_mcu/my_board")
```

By doing this, the CMake variables `LT_HAL_SRCS` and `LT_HAL_INC_DIRS` will become available to the consumer.

### Create and Implement the HAL C Files
1. Inside `hal/`, create a new directory called `my_mcu`.

    !!! note
        The `my_mcu/` directory inside `hal/` might already exist, so you do not have to create a new one — just use the existing one (e.g. `stm32`).

2. Inside `hal/my_mcu/`, create a directory called `my_board`. This is where the implementation will go.
3. Inside `hal/my_mcu/my_board/`, create the following files:
    - `libtropic_port_my_mcu_my_board.h`,
    - `libtropic_port_my_mcu_my_board.c`.
4. Inside `libtropic_port_my_mcu_my_board.h`, declare:
    1. A new device structure with public and private members in the following way:
```c { .copy }
typedef struct lt_dev_my_mcu_my_board_t {
    // Public part
    /** @brief @public first public member comment */
    // ...
    /** @brief @public n-th public member comment */

    // Private part
    /** @brief @private first private member comment */
    // ...
    /** @brief @private n-th pivate member comment */
} lt_dev_my_mcu_my_board_t;
```

        !!! question "Which Members Are Needed?"
            These members are usually physical pin numbers, SPI handles, or other information needed in the HAL functions that handle the platform-specific interface on the L1 Layer.

    2. Additional macros or types you will need in `libtropic_port_my_mcu_my_board.c`.

5. Inside `libtropic_port_my_mcu_my_board.c`, implement all functions declared in `include/libtropic_port.h`. All of the port functions have an instance of `lt_l2_state_t` as one of the parameters, where your instance of `lt_dev_my_mcu_my_board_t` will be saved, so you can get it in a following way:
```c { .copy }
// one of the functions from include/libtropic_port.h
lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_my_mcu_my_board_t *device =
        (lt_dev_my_mcu_my_board_t *)(s2->device);
    
    // Your implementation ...
    
    return LT_OK;
}
```

    !!! warning "Implementation of `lt_port_random_bytes`"
        This function should use some cryptographically secure mechanism to generate the random bytes. Its speed should not be a concern, as this function is not called often.

6. Additionally, other source files and headers can be created for the needs of the implementation.

### Create and Implement the HAL CMakeLists.txt
Inside `hal/my_mcu/my_board/`, create a `CMakeLists.txt` with the following contents:
```cmake { .copy }
set(LT_HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libtropic_port_my_mcu_my_board.c
    # Other source files if needed
)

set(LT_HAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Other include directories if needed
)

# export generic names for parent to consume
set(LT_HAL_SRCS ${LT_HAL_SRCS} PARENT_SCOPE)
set(LT_HAL_INC_DIRS ${LT_HAL_INC_DIRS} PARENT_SCOPE)
```

### Provide Some Information About the HAL
All currently supported host platforms are listed in the [Supported Host Platforms](../compatibility/host_platforms/index.md) section. Add the new host platform there and provide some information about it and the HAL (see other sections for inspiration).