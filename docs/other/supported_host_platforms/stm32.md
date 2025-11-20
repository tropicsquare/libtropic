# STM32
Currently supported STM32 platforms are:

- [NUCLEO-F439ZI](#nucleo-f439zi)
- [NUCLEO-L432KC](#nucleo-l432kc)

HALs for these ports are available in the `libtropic/hal/stm32/` directory.

Libtropic example usage of these platforms is currently available in our [libtropic-stm32](https://github.com/tropicsquare/libtropic-stm32) repository.

## NUCLEO-F439ZI
Fully working.
!!! failure "Interrupt PIN Support"
    Support for the TROPIC01's interrupt PIN is implemented but not tested.

## NUCLEO-L432KC
!!! failure "Limited Support"
    Consider this platform as not supported. The HAL is implemented, but very outdated. Support for this platform is planned to be removed.