# 1. Your First Steps with Libtropic

Hello and welcome to Libtropic SDK! In the first tutorial, we will compile our first examples.

Before proceeding, make sure you have activated the virtual environment you installed the TROPIC01 Model in:

!!! example "Activating the virtual environment"
    === ":fontawesome-brands-linux: Linux"
        If the virtual environment is activated, you will see "(.venv)" prefix in front of the prompt. For example:

        ```bash
        (.venv) me@computer:~/libtropic$
        ```

        To activate the environment, run:

        ```bash
        source scripts/tropic01_model/.venv/bin/activate
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

Each examples requires a fresh start of the model. You can start and terminate the model as following:

!!! example "Using the model"
    === ":fontawesome-brands-linux: Linux"
        Open a new console (or a new tab in your console emulator). Make sure you have the virtual environment activated. Type:
        
        ```bash
        model_server tcp -c scripts/tropic01_model/model_cfg.yml
        ``` 

        This will start a new TROPIC01 Model server. You can inspect the output after running examples if you are interested.
        
        To terminate the server, press ++ctrl+c++ in the console.

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## *Hello, World!* Example
At first, let's see the *Hello, World!* example. You can find this example at `examples/model/hello_world`.

!!! example "Compiling and running the example"
    === ":fontawesome-brands-linux: Linux"
        ```bash
        cd examples/model/hello_world
        mkdir build
        cd build
        cmake ..
        make -j
        ./libtropic_hello_world
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

You should see an output similar to this:

```
======================================
==== TROPIC01 Hello World Example ====
======================================
PRNG initialized with seed=14758818
Initializing handle...OK
Sending reboot request...OK
Starting Secure Session with key slot 0...OK
Sending Ping command...
        --> Message sent to TROPIC01: 'This is Hello World message from TROPIC01!!'
        <-- Message received from TROPIC01: 'This is Hello World message from TROPIC01!!'
Aborting Secure Session...OK
Deinitializing handle...OK
```

If you see the output, congratulations! 🎉 You used Libtropic to send a ping with a message to a TROPIC01 Model!

Continue with the next tutorial, where we discuss the functions used in this example and understand the basics of Libtropic's and TROPIC01's architectures.

[Next tutorial :material-arrow-right:](./understanding_libtropic.md){ .md-button }