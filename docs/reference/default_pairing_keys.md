# Default Pairing Keys for a Secure Channel Handshake
To establish a Secure Channel Session with TROPIC01, one of the Pairing Key slots has to be written with an X25519 public key (refered to as $S_{HiPUB}$, where $i$ is the i-th slot). The entity, which is about to establish the Secure Channel Session with TROPIC01, has to own the corresponding X25519 private key (refered to as $S_{HiPRIV}$).

At the time of manufacturing, Tropic Square configures the Pairing Key slot 0 of every TROPIC01 with $S_{H0PUB}$, generated from $S_{H0PRIV}$, which is provided to customers. The customer can then establish a Secure Channel Session with Pairing Key slot 0, configure TROPIC01, write their own X25519 public key to slot 1, 2 or 3 and invalidate the slot 0 (which is recommended for security reasons).

!!! info "More Information About Pairing Keys"
    For more information, refer to the [TROPIC01 datasheet](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#documentation).

## Default Pairing Keys in Libtropic
Libtropic provides both of these default $S_{H0PUB}$ and $S_{H0PRIV}$ keys by including `libtropic_common.h`:

1. arrays `sh0pub_prod0` and `sh0priv_prod0` - production keys found in the majority of distributed TROPIC01 chips (see [Available Parts](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#available-parts) table in the TROPIC01 GitHub repository for P/N values),
2. arrays `sh0pub_eng_sample` and `sh0priv_eng_sample` - keys found in engineering (pre-production) samples of TROPIC01 with P/N `TROPIC01-ES`.

### Establishing Your First Secure Channel Session
To establish a Secure Channel Session with your new TROPIC01, do the following:

1. Get P/N of your TROPIC01 — refer to [FAQ](../faq.md#what-is-the-part-number-pn-of-my-tropic01).
2. Establish the Secure Channel Session:

    There are two options, depending on what you want to do — choose one:

    1. You want to run our **examples** (see [Tutorials](../tutorials/index.md)) that establish a Secure Channel Session:
        1. Your P/N is `TROPIC01-ES` -> Set `LT_SH0_KEYS` CMake option to `"eng_sample"`.
        2. Your P/N is **not** `TROPIC01-ES` -> nothing has to be done, the production keys are used by default.
    2. You are writing **your own** application -> select the correct key pair arrays according to [Default Pairing Keys in Libtropic](#default-pairing-keys-in-libtropic) and simply pass them to either:
        1. `lt_verify_chip_and_start_secure_session` helper function, or
        2. `lt_session_start` core API function.
        
        Refer to the [API Reference](../doxygen/build/html/index.html) for more information about these functions.