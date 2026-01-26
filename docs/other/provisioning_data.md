# Provisioning Data
The `scripts/tropic01_model/provisioning_data` directory contains so-called **lab batch packages**, which are data used for provisioning TROPIC01 chips in the Tropic Square lab for testing purposes. In Libtropic, these lab batch packages are used for configuring the [TROPIC01 Model](../tutorials/model/index.md).
> [!NOTE]
> Lab batch packages in this directory are stripped — they contain only the data needed by libtropic.

## Lab Batch Package Contents
For example, the `2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/` lab batch package:

1. `cert_chain/`: All certificates for the Certificate Store (excluding TROPIC01's eSE device certificate, which is originally not part of the lab batch package and can be found outside the `cert_chain/` directory).
2. `i_config/`, `r_config/`: **Currently unused.** Contains fields that should be written.
3. `sh0_key_pair/`: Contains the public and private key for pairing key slot 0 (SH0PUB, SH0PRIV).
4. `tropic01_ese_certificate.pem`: TROPIC01's eSE device certificate.
5. `tropic01_ese_private_key.pem`: TROPIC01's eSE device private key (STPRIV).