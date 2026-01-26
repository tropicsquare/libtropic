# Libtropic Architecture
Before you learn about Libtropic's architecture, it is important to understand the communication between the host MCU and TROPIC01. Both parties communicate via a multi-layer serial protocol consisting of the following three layers:

1. *Physical Layer (L1)*. A **transfer** is a unit of communication and a 4-wire SPI interface is used.
2. *Data Link Layer (L2)*. A **frame** is a unit of communication that is organized into multiple fields. During L2 communication, the host MCU sends an L2 Request frame and TROPIC01 returns an L2 Response frame. Communication on the L2 Layer is not encrypted and is used for non-secure information about TROPIC01 and for setting up L3 communication.
3. *Secure Session Layer (L3)*. A **packet** is a unit of communication. L3 communication requires an established Secure Channel Session. Once established, the host MCU can communicate by sending L3 Command packets and TROPIC01 responds with L3 Result packets. The communication is executed on an encrypted channel (Secure Channel) with strong forward secrecy based on the [Noise Protocol Framework](http://www.noiseprotocol.org/noise.pdf).

For more information about TROPIC01, please refer to [TROPIC01 repository](https://github.com/tropicsquare/tropic01).

Libtropic's architecture is visualized in the following figure:

<figure style="text-align: center;">
<img src="../../img/libtropic_architecture.svg" alt="Libtropic Architecture" width="750"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em;">
    Libtropic Architecture
  </figcaption>
</figure>

Libtropic consists of:

1. *Libtropic Public API*. Macros and data structures are available in `include/libtropic_common.h` and function declarations are in `include/libtropic.h` (implemented in `src/libtropic.c`). The interface of these functions is tightly related to TROPIC01's commands, defined in the User API (see the [TROPIC01 repository](https://github.com/tropicsquare/tropic01)). These public functions are used for *unencrypted communication* via the *Layer 2 API*, and *encrypted communication* via the *Layer 3 API*.
2. *Helpers*. Functions also declared in `include/libtropic.h` that either wrap one or more libtropic API functions to simplify operations (e.g. `lt_verify_chip_and_start_secure_session()` for easier Secure Session establishment), or provide additional functionality (e.g. `lt_print_chip_id()` for interpreting and printing TROPIC01's `CHIP_ID`).
3. *Examples*. Demonstrate usage of Libtropic in [Tutorials](../tutorials/index.md), using both the *Libtropic API* and *Helpers*.
4. *[Functional Tests](../for_contributors/tests/functional_tests.md)*. Used to verify the libtropic core API using both the *Libtropic API* and *Helpers*.
5. *L3 Layer API*. Functions called by the *Libtropic Public API* during *encrypted communication*. Because the L3 Layer requires cryptographic functionality (for example, to decrypt incoming L3 Result packets from TROPIC01), it uses a *Crypto Abstraction Layer* (CAL) to obtain functionality provided by a *Cryptographic Functionality Provider* (CFP). A CFP can be a cryptographic library (e.g. MbedTLS) or a cryptographic hardware accelerator. The CAL is declared in headers inside `src/` and specific CAL implementations for each CFP exist in `cal/`. It is assumed that only one CAL implementation is used for a given libtropic build.
6. *L2 Layer API*. Functions called by the *Libtropic Public API* during *Unencrypted Communication*.
7. *L1 Layer API*. Functions called by the *L2 Layer API* that wrap port-specific L1 Layer functions implemented by the *Hardware Abstraction Layers* (HAL) in `hal/`. These HALs are initialized using the *Libtropic Public API*. The library can be compiled with support for only one HAL at a time, but a single HAL may support communication with multiple chips.

!!! info "More Information About Libtropic Functions"
    For more information about Libtropic functions, refer to the [API Reference](../doxygen/build/html/index.html).