# Provisioning Data
The `provisioning_data/` directory contains so called **lab batch packages**, which are data used for prsovisioning TROPIC01 chips in the Tropic Square lab for testing purposes. These lab batch packages are used here for configuring the [TROPIC01 Model](tropic01_model.md) and for providing the tests and examples public and private keys for the TROPIC01's pairing key slot 0 (SH0PUB, SH0PRIV), so they can establish a secure session with the chip.
> [!NOTE]
> Lab batch packages in this directory are stripped - they contain only data needed in libtropic.

## Lab Batch Package Contents
For example, the `2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/` lab batch package:

1. `cert_chain/`: All certificates for the Certificate Store (excluding TROPIC01's eSE device certificate, which is originally not part of the lab batch package - can be found outside the `cert_chain/` directory).
2. `i_config/`, `r_config/`: Contains fields which should be written - currently not used.
3. `sh0_key_pair/`: Contains public and private key for the pairing key slot 0 (SH0PUB, SH0PRIV).
4. `tropic01_ese_certificate.pem`: TROPIC01's eSE device certificate.
5. `tropic01_ese_private_key.pem`: TROPIC01's eSE device private key (STPRIV).