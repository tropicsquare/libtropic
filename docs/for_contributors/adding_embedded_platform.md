# Adding a New Embedded Platform
Libtropic is written to be *platform-independent*, so no changes to the main code base are needed when adding support for a new platform. However, to define how communication on the L1 Layer will work, a new Hardware Abstraction Layer (HAL) must be implemented. Currently available HALs are located in `hal/port/`.

## Guide
To create a new HAL:

1. [Create and Implement the Necessary Files](#create-and-implement-the-necessary-files).
!!! tip
    For inspiration, see the existing HALs inside `hal/port/`.

### Create and Implement the Necessary Files
1. Inside `hal/port/`, create a new directory, for example called `newplatform`.
> [!NOTE]
> The platform directory inside `hal/port/` might already exist, so you do not have to create a new one — just use the existing one (e.g. `stm32`).
2. Inside `hal/port/newplatform`, create two new files, where `<additional_info>` can be a name of the board or the protocol name used (e.g. `spi`, `tcp`, ...):
    - `libtropic_port_newplatform_<additional_info>.h`,
    - `libtropic_port_newplatform_<additional_info>.c`.
3. Inside `libtropic_port_newplatform_<additional_info>.h`, declare:
    1. a new device structure with public and private members in the following way:
```c
typedef struct lt_dev_newplatform_<additional_info> {
    // Public part
    /** @brief @public first public member comment */
    // ...
    /** @brief @public n-th public member comment */

    // Private part
    /** @brief @private first private member comment */
    // ...
    /** @brief @private n-th pivate member comment */
} lt_dev_newplatform_<additional_info>;
```
These members are usually physical pin numbers, SPI handles, or other information needed in the HAL functions that handle the platform-specific interface on the L1 Layer.
    2. additional macros or types you will need in `libtropic_port_newplatform_<additional_info>.c`.
4. Inside `libtropic_port_newplatform_<additional_info>.c`, implement:
    1. all functions declared in `include/libtropic_port.h`. All of the port functions have an instance of `lt_l2_state_t` as one of the parameters, where your device structure from 3.a. will be saved, so you can get it in a following way:
    ```c
    // one of the functions from include/libtropic_port.h
    lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
    {
        lt_dev_newplatform_<additional_info> *device =
            (lt_dev_newplatform_<additional_info> *)(s2->device);
        
        // Your implementation ...
        
        return LT_OK;
    }
    ```
    2. additional `static` functions you might need.