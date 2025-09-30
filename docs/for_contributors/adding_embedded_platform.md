# Adding a New Embedded Platform
Libtropic is written as *platform-independent*, so no changes to the main code base are needed when adding a support for a new platform. However, to define how the communication on the L1 Layer will work, a new HAL (Hardware Abstract Layer) has to be implemented. Currently available HALs are located in `hal/port/`.

## Guide
To create a new HAL:

1. [Create and Implement the Necessary Files](#create-and-implement-the-necessary-files),
2. [Use the New HAL](#use-the-new-hal).
!!! tip
    For inspiration, see existing HALs inside `hal/port/`.

### Create and Implement the Necessary Files
1. Inside `hal/port/`, create a new directory, for example called `newplatform`.
> [!NOTE]
> The platform directory inside `hal/port/` might already exist, so you do not have to create a new one, just use the existing one (e.g. `stm32`).
2. Inside `hal/port/newplatform`, create two new files, where `<additional_info>` can be a name of the board or the protocol name used (e.g. `spi`, `tcp`, ...):
    - `lt_port_newplatform_<additional_info>.h`,
    - `lt_port_newplatform_<additional_info>.c`.
3. Inside `lt_port_newplatform_<additional_info>.h`, declare:
    1. a new device structure with public and private members in a following way:
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
These members are usually physical pin numbers, SPI handles or other information needed in the HAL functions, handling the platform-specific interface on the L1 Layer.
    2. additional macros or types you will need in `lt_port_newplatform_<additional_info>.c`.
4. Inside `lt_port_newplatform_<additional_info>.c`, implement:
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

### Use the New HAL

1. Add the new HAL files inside your `CMakeLists.txt` or `Makefile` - see [Adding to an Existing Project](../get_started/integrating_libtropic/adding_to_project.md) section for an inspiration.
2. Initialize and use the new HAL in your code:
```c
#include "lt_port_newplatform_<additional_info>.h"
#include "libtropic_common.h"

int main(void) {
    lt_handle_t h = {0}; // Libtropic's handle.
    lt_dev_newplatform_<additional_info> my_device;
    // Initialize the device structure.
    my_device.first_member = "some value for the first member";
    my_device.nth_member = "some value for the n-th member";
    // Save a pointer to `my_device` inside the libtropic's handle,
    // so you can get it in the port functions.
    h.l2.device = &my_device;

    // Initialize the handle.
    lt_ret_t ret = lt_init(h);
    if (LT_OK != ret) {
        lt_deinit(h);
        return -1;
    }

    // Do your stuff ...

    // Deinitialize the handle after you are done.
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        return -1;
    }

    return 0;
}
```