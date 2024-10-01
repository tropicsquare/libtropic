# Integration tests

This code is meant to be compiled under Unix and all tests here are running against TROPIC01's emulation model, that means no chip or external hardware is needed.

## Model of TROPIC01

To start a model, follow readme in [its](https://github.com/tropicsquare/ts-tvl) repository.

Don't forget to provide model with config file, keys and chip's certificate as is described [here](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#configuration-file)!



## Compile integration tests

Keep the model running in its own terminal, open a new terminal and compile integration tests:

```
$ cd tests/integration_tests/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Compilation shouldn't take long, for compiling in debug mode replace `cmake ..` with `cmake -DCMAKE_BUILD_TYPE=Debug ..` (this will allow to step through with a debugger)


## Run it all together

While model is still running in first terminal, execute integration tests:

```
./build/integration_test
```

## How it works?

Both processes will talk to each other through TCP socket 127.0.0.1:28992.

*Note: During tests against model, SPI layer between libtropic and model is emulated through TCP connection.
Model responses are exactly the same as from physical TROPIC01 chip.*

*Note2: Restarting the model is equivalent to plugging a fresh TROPIC01 straight out factory*

