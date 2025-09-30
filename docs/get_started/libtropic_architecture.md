# Libtropic Architecture
Before talking about Libtropic's architecture, it is important to understand the communication between the Host MCU and TROPIC01. Both of these parties communicate via a multi-layer serial protocol, consisting of the following three layers:

1. *Phyiscal Layer (L1)*. A **transfer** is a unit of communication and a 4-wire SPI interface is used.
2. *Data Link Layer (L2)*. A **frame** is a unit of communication, which is organized into multiple fields. During L2 communication, the Host MCU sends an L2 Request frame and TROPIC01 return an L2 Reponse frame. Communication on the L2 Layer is not encrypted and thus used for non-secure information about TROPIC01 and L3 communication set up.
3. *Secure Session Layer (L3)*. A **packet** is a unit of communication. L3 communication requires an established Secure Channel Session. Then, the Host MCU can communicate through sending L3 Command packets and TROPIC01 responds with L3 Result packets. The communication is executed on an encrypted channel (Secure Channel) with strong forward secrecy based on a [Noise Protocol Framework](http://www.noiseprotocol.org/noise.pdf).

For more information about TROPIC01, please refer to [TROPIC01 repository](https://github.com/tropicsquare/tropic01).

Libtropic's architecture is visualized in the following figure:

<figure style="text-align: center;">
<img src="../../img/libtropic-architecture.svg" alt="Libtropic Architecture" width="500"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em;">
    Libtropic Architecture
  </figcaption>
</figure>

Libtropic consists of:

1. *Libtropic Public API*. Macros and data structures are available in `include/libtropic_common.h` and function declarations in `include/libtropic.h` (implemented in `src/libtropic.c`). Interface of these functions is tightly related to TROPIC01's commands, defined in the User API (can be found in the [TROPIC01 repository](https://github.com/tropicsquare/tropic01)). These public functions are used for *Unencrypted Communication*, which is achieved by calling the *Layer 2 API*, and *Encrypted Communication*, achieved by calling the *Layer 3 API* and using one of the available *Cryptography Libraries* for cryptography operations on the Host MCU side.
2. *Helpers*. Functions also declared in `include/libtropic.h`, that either wrap one or more Libtropic API functions to simplify some operations (e.g. the function `lt_verify_and_chip_and_start_secure_session()` for easier Secure Session establishment), or provide some additional functionality (e.g. the function `lt_print_chip_id()` for interpreting TROPIC01's `CHIP_ID` and printing it).
3. *[Examples](examples/index.md)*. Show example usage of Libtropic, using both the *Libtropic API* and *Helpers*.
4. *[Functional Tests](../for_contributors/functional_tests.md)*. Used to verify the libtropic core API, using both the *Libtropic API* and *Helpers*.
5. *L3 Layer API*. Functions called by the *Libtropic Public API* during *Encrypted Communication*.
6. *L2 Layer API*. Functions called by the *Libtropic Public API* during *Unencrypted Communication*.
7. *L1 Layer API*. Functions called by the *L2 Layer API* and wrapping port-specific L1 Layer functions, which are implemented by the HALs (Hardware Abstract Layers) in `hal/port/`. These HALs are initialized using the *Libtropic Public API*. The library can be compiled with support for only one hardware abstraction layer (HAL) at a time, but the HAL allows communication with multiple chips.

!!! tip
    For more information about Libtropic's functions, refer to the [API Reference](../doxygen/build/html/index.html).