# Examples
The `examples/` directory contains multiple examples demonstrating how to use libtropic.  
**Please read this entire page before diving into the examples!**

## Example Categories
Some examples may cause irreversible changes to the chip, so they are organized into two categories:

- [Reversible Examples](reversible_examples.md)
- [Irreversible Examples](irreversible_examples.md)

!!! note
    Irreversible changes include writing to I-config, writing or invalidating pairing keys, or performing a firmware update.

## Building
Examples can be compiled using the `LT_BUILD_EXAMPLES` flag. You can enable the flag in two ways:

- During compilation by passing `-DLT_BUILD_EXAMPLES=1` to `cmake`, or
- in your project's `CMakeLists.txt`: `set(LT_BUILD_EXAMPLES ON)`.

It is recommended to try the examples in one of our [platform repositories](../integrating_libtropic/integration_examples.md) using a real TROPIC01 chip on one of our supported platforms, or on a [TROPIC01 model](../../other/tropic01_model/index.md) directly on your computer.  
Both the platform repositories and the model contain detailed guides on example compilation.

!!! warning
    You may encounter issues with examples that establish a Secure Session - refer to [Establishing Your First Secure Channel Session](../default_pairing_keys.md#establishing-your-first-secure-channel-session) section for more information.

??? tip "Advanced Tip: Running an Example With Your Own Pairing Key"
    If you have already written your own public key to one of the available slots and want to execute one of the examples (that uses a Secure Session), define the arrays for your private and public key as global and after `#include libtropic_examples.h`, do the following:
    ```c
    #undef LT_EX_SH0_PRIV
    #define LT_EX_SH0_PRIV <var_name_with_your_private_pairing_key>

    #undef LT_EX_SH0_PUB
    #define LT_EX_SH0_PUB <var_name_with_your_public_pairing_key>
    ```

## Should I Use the Model or a Real Chip?
All of the examples are compatible with the TROPIC01 chip, and the majority of them are also compatible with the TROPIC01 model. It is highly recommended to try the model first, especially for irreversible examples, as they make irreversible changes to the real chip.

Some examples are not compatible with the model (such as the firmware update example). Any such incompatibility is always noted in the example description in this documentation.

## Where Do I Start?
We recommend starting with [Reversible Examples](reversible_examples.md), specifically `lt_ex_hello_world.c`.  
This example demonstrates the usage of basic libtropic API functions. After understanding this example, you can dive into more complex ones.