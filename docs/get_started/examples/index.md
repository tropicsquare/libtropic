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

It is recommended to try the examples in one of our [platform repositories](../integrating_libtropic/integration_examples.md) using a real TROPIC01 chip on one of our supported platforms, or on a [TROPIC01 model](../../other/tropic01_model.md) directly on your computer.  
Both the platform repositories and the model contain detailed guides on example compilation.

!!! note
    During the build process, you may encounter issues with pairing keys. Although the default (production) key is present in the majority of distributed TROPIC01 chips, the first chip revisions (engineering samples) might contain a different key. This key must be selected manually during the build using the following CMake switch: `-DLT_SH0_PRIV_PATH=<path to sh0_priv_engineering_sample01.pem>`.  
    Check out the platform repositories or the TROPIC01 model for more information.

## Should I Use the Model or a Real Chip?
All of the examples are compatible with the TROPIC01 chip, and the majority of them are also compatible with the TROPIC01 model. It is highly recommended to try the model first, especially for irreversible examples, as they make irreversible changes to the real chip.

Some examples are not compatible with the model (such as the firmware update example). Any such incompatibility is always noted in the example description in this documentation.

## Where Do I Start?
We recommend starting with [Reversible Examples](reversible_examples.md), specifically `lt_ex_hello_world.c`.  
This example demonstrates the usage of basic libtropic API functions. After understanding this example, you can dive into more complex ones.