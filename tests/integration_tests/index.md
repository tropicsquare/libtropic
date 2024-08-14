# Integration tests

All tests here are running against model, that means no chip or external hardware is needed.

## Model of TROPIC01

To start a model, follow readme in its repository here [here](https://github.com/tropicsquare/ts-tvl).

Keep it running in a separate terminal, while executing integration tests. They will talk to each other through TCP socket.

*Note: During tests against model, SPI layer between libtropic and model is emulated through TCP connection.
Model responses are exactly the same as from physical TROPIC01 chip.*

## Compile and run integration tests

Once the model is running, compile integration tests:

```
$ cd tests/integration_tests/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Compilation shouldn't take long, then run it like this:

```
./main
```
