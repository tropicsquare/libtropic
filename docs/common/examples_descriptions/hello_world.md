This example demonstrates the basic libtropic API and can be used to verify that the chip works correctly.
In this example, you will learn about the following functions:

- `lt_init()`: function used to initialize context for communication with the TROPIC01,
- `lt_verify_chip_and_start_secure_session()`: helper function to start Secure Session and allow L3 communication,
- `lt_ping()`: L3 command to verify communication with the TROPIC01,
- `lt_session_abort()`: L3 command to abort Secure Session,
- `lt_deinit()`: function used to deinitialize context.