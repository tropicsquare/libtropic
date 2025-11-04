# How To Use
## Which Headers To Include?
To start using Libtropic in your application, the following headers have to be included:

1. `libtropic_common.h`: Declarations of the commonly used macros, structures, enums and other. 
2. `libtropic.h`: Main API function declarations.
3. `libtropic_port_<port_name>.h`: Declares the `lt_dev_<port_name>_t` device structure, specific to the Host platform (see the `libtropic/hal/port/` directory for the existing platform HALs). An `lt_dev_<port_name>_t` variable has to be declared and passed to an instance of `lt_handle_t` (see the example below). 
4. `libtropic_<cfp_name>.h`: Declares the `lt_ctx_<cfp_name>_t` context structure, specific to the used CFP (Cryptographic Functionality Provider) - see the `libtropic/cal/` directory for the existing CALs (Crypto Abstraction Layers). An `lt_ctx_<cfp_name>_t` variable has to be declared and passed to an instance of `lt_handle_t` (see the example below).
5. Based on the needed functionality, additional headers from `libtropic/include/`. Refer to the [API Reference](../../../doxygen/build/html/index.html) for more details.

!!! note
    The headers `libtropic_port_<port_name>.h` and `libtropic_<cfp_name>.h` should only be needed when initializing the instance of `lt_handle_t` - this is the case for the example below.

## Libtropic Bare-Bone Example
!!! tip
    For inspiration on what to do in the step 8 of the example below, refer to the [Examples](../../examples/index.md) section.

    We recommend checking out the [Integration Examples](../integration_examples.md) section for complete examples on how to use Libtropic on some of the supported platforms.

The following bare-bone example shows how to initialize Libtropic, so it can be used to communicate with TROPIC01:
```c
#include "libtropic_common.h"
#include "libtropic.h"
#include "libtropic_port_<port_name>.h"
#include "libtropic_<cfp_name>.h"

int main(void) {
    // 1. Declare a handle variable.
    // The handle is a context for the whole communication between libtropic
    // and TROPIC01.
    lt_handle_t h;

    // 2. Declare a device structure.
    // The device structure provides libtropic with the device-specific
    // information.
    lt_dev_<port_name>_t my_device;

    // 3. Initialize the device structure.
    // The members of the device structure are specific to the device - each
    // device requires different members to be initialized.
    my_device.first_member = "some value for the first member";
    my_device.nth_member = "some value for the n-th member";

    // 4. Save a pointer to the device structure inside the handle.
    // Libtropic will then pass this structure to the HAL functions.
    h.l2.device = &my_device;

    // 5. Declare a context structure for the CFP (Cryptographic Functionality
    //    Provider).
    // The context structure provides libtropic with the memory location where
    // it can save contexts of cryptographic functions.
    // None of its members have to be initialized.
    lt_ctx_<cfp_name>_t my_crypto_ctx;

    // 6. Save a pointer to the context structure inside the handle.
    // Libtropic will then pass this structure to the CAL functions.
    h.l3.crypto_ctx = &my_crypto_ctx;

    // 7. Initialize the handle.
    // This should be done only once for a specific handle.
    // If you need to initialize the specific handle again, call lt_deinit()
    // first.
    lt_ret_t ret = lt_init(h);
    if (LT_OK != ret) {
        lt_deinit(h);
        return -1;
    }

    // 8. Do your stuff.

    // 9. Deinitialize the handle.
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        return -1;
    }

    return 0;
}
```