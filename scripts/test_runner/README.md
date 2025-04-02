# TEST RUNNER

**It is recommended to get familiar with the process by using `Hello World` example first.**

**Warning: Some functions will result into ireversible changes in TROPIOC01**

This folder contains scripting used to run examples and tests sequentionally on supported platforms.
Available libtropic examples and tests are defined in `libtropic/examples` folder.


Process of running example (or test) function on a platform's devboard:

- Add function call to platform project, compile
- Open serial port for reading results from platform
- Flash compiled elf file on a platform
- Read serial output and compare with expected results

### Add function call and compile

Please note that execution of single functions should be mutually exclusive.
To enable concrete function to run, pass one of following switches during compilation of platform's project:
```
# Tests
-DLT_TEST_REVERSIBLE=1
-DLT_TEST_IREVERSIBLE=1
-DLT_TEST_SAMPLES_1=1

# Examples
-DLT_EX_FW_UPDATE=1
-DLT_EX_HELLO_WORLD=1
-DLT_EX_HW_WALLET=1
```
Because in platform's main code are all function calls guarded by defines, user controls presence of a function by passing a switch.
For more info have a look into `main()` function of one of supported platforms.

If test is being executed on libtropic testing rig, use also following switch to route USART data:

```
-DLT_TESTING_RIG=1
```

So, for example when you want to run `Hello world` example and execute it on testing rig, compile like this:
```
$ cd libtropic-stm32/NUCLEO_F439ZI/
$ mkdir build &&  cd build && cmake -DLT_TESTING_RIG=1 -DLT_EX_HELLO_WORLD=1 .. && make && cd ../
```

### Open serial port for reading results from platform

Serial port of Nucleo F439ZI should enumerate itself as `/dev/ttyUSB1`, but this depends on many things. It might be good idea to test it with Hello World example, because this example does not do any ireversible changes to chip.

### Flash

As of now only `Nucleo F439ZI` is supported.

```
$ ./openocd_test_F439.sh <path_to_built_elf> <test_duration_in_s>
```

For example to flash binary built in previous step, use:

```
$ cd libtropic-stm32/libtropic/scripts/test_runner/
$ ./openocd_test_F439ZI.sh "/home/xxx/Desktop/libtropic-stm32/NUCLEO_F439ZI/build/stm32_example.elf" 3
```

Executing this script will result into target board being flashed, reset and results will be sent over UART.







