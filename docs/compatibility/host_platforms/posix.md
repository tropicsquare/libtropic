# POSIX
We provide the following ports, which should be compatible with most POSIX compliant operating systems:

- [TCP](#tcp)
- [TROPIC01 USB Devkit](#tropic01-usb-devkit)

HALs for these ports are available in the `libtropic/hal/posix/` directory.

## TCP
We use this port with the [TROPIC01 Python Model](../../tutorials/model/index.md), which acts as a server, to which libtropic connects via the specified TCP port. See our [TROPIC01 Model Tutorials](../../tutorials/model/index.md) to quickly get started.

!!! warning "Disclaimer"
    The TCP HAL is implemented with consideration of the following:

    1. It is primarily targeted for use with the [TROPIC01 Python Model](../../tutorials/model/index.md).
    2. To ensure reproducibility of randomized functional tests, the [rand](https://en.cppreference.com/w/c/numeric/random/rand) function is used in the `lt_port_random_bytes` function with a known random seed instead of more cryptographically secure solutions.

!!! failure "Interrupt Pin Support"
    The TCP HAL does not support TROPIC01's interrupt pin.

## TROPIC01 USB Devkit
Libtropic communicates with our USB Devkits using the USB protocol. See our [TROPIC01 USB Devkit Tutorial](../../tutorials/linux/usb_devkit/index.md) to quickly get started.

!!! bug "Raspberry Pi 4 Issues"
    When testing with Raspberry Pi 4, we have encountered [issues with its USB](https://github.com/raspberrypi/linux/issues/3259#), which seems to lose some of the USB packets sent to it.

    !!! success "Raspberry Pi 5"
        Fortunately, Raspberry Pi 5 fixes these issues and the USB Devkit works without any issues.

!!! failure "Interrupt Pin Support"
    The USB Devkit port does not support TROPIC01's interrupt pin.