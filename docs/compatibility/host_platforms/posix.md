# POSIX
We provide the following ports, which should be compatible with most POSIX compliant operating systems:

- [TCP](#tcp)
- [Tropic Square TS1302 USB Devkit](#tropic-square-ts1302-usb-devkit)

HALs for these ports are available in the `libtropic/hal/posix/` directory.

Libtropic example usage with **some** of these ports is currently available in our [libtropic-linux](https://github.com/tropicsquare/libtropic-linux) repository. Other operating systems were not tested.

## TCP
We use this port with the [TROPIC01 Python Model](../../tutorials/model/index.md), which acts as a server, to which libtropic connects via the specified TCP port.

!!! warning "Disclaimer"
    The TCP HAL is implemented with consideration of the following:

    1. It is primarily targeted for use with the [TROPIC01 Python Model][TROPIC01 Python Model](../../tutorials/model/index.md).
    2. To ensure reproducibility of randomized functional tests, the [rand](https://en.cppreference.com/w/c/numeric/random/rand) function is used in the `lt_port_random_bytes` function with a known random seed instead of more cryptographically secure solutions.

!!! failure "Interrupt Pin Support"
    The TCP HAL does not support TROPIC01's interrupt pin.

## Tropic Square TS1302 USB Devkit
Libtropic communicates with this devkit using the USB protocol. Refer to the [TS1302 USB Devkit](https://github.com/tropicsquare/tropic01-stm32u5-usb-devkit-hw) GitHub page for more information about it.

!!! bug "Raspberry Pi 4 Issues"
    When testing with Raspberry Pi 4, we have encountered [issues with its USB](https://github.com/raspberrypi/linux/issues/3259#), which seems to lose some of the USB packets sent to it.

    !!! success "Raspberry Pi 5"
        Fortunately, Raspberry Pi 5 fixes these issues and the TS1302 USB Devkit works without any issues.

!!! failure "Interrupt Pin Support"
    The TS1302 USB Devkit port does not support TROPIC01's interrupt pin.