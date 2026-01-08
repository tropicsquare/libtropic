# 1. First Steps with Libtropic

Hello and welcome to Libtropic SDK! In this first tutorial, we will run our first examples.

Before proceeding, make sure you have activated the virtual environment you [installed](index.md#install-dependencies-and-prepare-the-repository) the TROPIC01 Model in:

!!! example "Activating the Virtual Environment"
    === ":fontawesome-brands-linux: Linux"
        To activate the environment, run:

        ```bash
        source scripts/tropic01_model/.venv/bin/activate
        ```

        !!! info "Is the Virtual Environment Active?"
            If the virtual environment is activated, you will see '`(.venv)`' prefix in front of the prompt. For example:

            ```bash
            (.venv) me@computer:~/libtropic$
            ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

You can start and terminate the model as following:

!!! example "Using the TROPIC01 Model"
    === ":fontawesome-brands-linux: Linux"
        Open a new console (or a new tab in your console emulator). Make sure you have the virtual environment activated. Type:
        
        ```bash
        model_server tcp -c scripts/tropic01_model/model_cfg.yml
        ``` 

        This will start a new TROPIC01 Model server. You can inspect the output after running examples if you are interested.
        
        To terminate the server, press ++ctrl+c++ in the console.

        !!! warning
            Make sure you are running exactly one TROPIC01 Model server instance!

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## *Hello, World!* Example
At first, let's run the *Hello, World!* example. You can find this example in the `examples/tropic01_model/hello_world/` directory.

!!! example "Build and Run the Example"
    === ":fontawesome-brands-linux: Linux"
        Build the example:
        ```bash
        cd examples/tropic01_model/hello_world
        mkdir build
        cd build
        cmake ..
        make -j
        ```

        Run the example:
        ```bash
        ./libtropic_hello_world
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

Now, you should see an output similar to this:

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

If you see the output, congratulations! 🎉 You used Libtropic to send a *Ping* command with a message to the TROPIC01 Model!

Continue with the next tutorial, where we discuss the functions used in this example and understand the basics of Libtropic's and TROPIC01's architectures.

[Next tutorial :material-arrow-right:](./understanding_libtropic.md){ .md-button }