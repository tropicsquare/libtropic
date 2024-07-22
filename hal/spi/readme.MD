# Platform support

This folder contains l1 implementations for supported platforms.

By default **libtropic** contains weak definitions for function related to transport l1 layer. User can either provide own implementation for data transfers, or use one of a few which are provided for supported platforms:

  * Unix
  * STM32

For more insight into how compilation works check cmake file in examples folder.