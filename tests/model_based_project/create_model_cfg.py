import argparse
import pathlib
import yaml
from cryptography.x509 import load_pem_x509_certificate, load_der_x509_certificate
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend

def parse_byte_string(s: str) -> bytes:
    if s.startswith("b'") and s.endswith("'"):
        s = s[2:-1]
    return s.encode('latin1').decode('unicode_escape').encode('latin1')

def load_cert(cert_path: pathlib.Path) -> bytes:
    """Load x509 certificate and convert to ASN.1 DER-TLV."""
    if cert_path.suffix == ".pem":
        return load_pem_x509_certificate(cert_path.read_bytes()).public_bytes(
            serialization.Encoding.DER
        )
    if cert_path.suffix == ".der":
        return cert_path.read_bytes()
    raise argparse.ArgumentTypeError("Certificate not in DER nor PEM format.")



if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="create_model_cfg.py",
        description="Creates YAML configuration file for the TROPIC01 model from lab_batch_package."
    )

    parser.add_argument(
        "--pkg-dir",
        help="Path to the batch package directory.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "--model-cfg",
        help="Path to the file where to put created YAML model configuration.",
        type=pathlib.Path,
        default="model_cfg.yml"
    )

    # Parse and save arguments
    args = parser.parse_args()
    pkg_dir_path: pathlib.Path = args.pkg_dir
    model_cfg_path: pathlib.Path = args.model_cfg

    # Load batch package YAML
    with pkg_dir_path.joinpath("tropic01_lab_batch_package.yml").open("r") as f:
        pkg_yml = yaml.safe_load(f)

    ese_cert_path = pkg_dir_path.joinpath(pkg_yml["tropic01_ese_certificate"])
    ese_priv_path = pkg_dir_path.joinpath(pkg_yml["tropic01_ese_private_key"])
    model_cfg = {}

    # Save raw eSE priv key to model configuration
    ese_priv = serialization.load_pem_private_key(
        ese_priv_path.read_bytes(),
        password=None
    )
    model_cfg["s_t_priv"] = ese_priv.private_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PrivateFormat.Raw,
        encryption_algorithm=serialization.NoEncryption()
    )

    # Save raw eSE pub key to model configuration
    ese_cert = load_der_x509_certificate(
        load_cert(ese_cert_path),
        default_backend()
    )
    model_cfg["s_t_pub"] = ese_cert.public_key().public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )

    # Get chip id from batch package YAML
    chip_id = b''.join(parse_byte_string(v) for v in pkg_yml["chip_id"].values())
    model_cfg["chip_id"] = chip_id

    # Generate certificate store and save it to model configuration
    cert_store_header = b'\x01'  # Store version
    cert_store_header += b'\x04'  # Number of certificates
    cert_store_body = b''
    for cert_path in [ese_cert_path,
                      pkg_dir_path.joinpath(pkg_yml["tropic01_xxxx_ca_certificate"]),
                      pkg_dir_path.joinpath(pkg_yml["tropic01_ca_certificate"]),
                      pkg_dir_path.joinpath(pkg_yml["tropicsquare_root_ca_certificate"])]:
        cert_der = load_cert(cert_path)
        cert_store_header += len(cert_der).to_bytes(2, 'big')
        cert_store_body += cert_der
    model_cfg["x509_certificate"] = cert_store_header+cert_store_body

    # Get raw sh0pub key and save it to model configuration
    sh0_pub_key = serialization.load_pem_public_key(
        pkg_dir_path.joinpath(pkg_yml["s_h0pub_key"]).read_bytes()
    )
    sh0_pub_raw = sh0_pub_key.public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )
    model_cfg["i_pairing_keys"] = {
        0: {
            "value": sh0_pub_raw,
            "state": "written"
        }
    }

    print("Warning: Following variables are not configured (model will set them to default values):")
    print("\t- r_config")
    print("\t- r_ecc_keys")
    print("\t- r_user_data")
    print("\t- r_mcounters")
    print("\t- r_macandd_data")
    print("\t- i_config")
    print("\t- riscv_fw_version")
    print("\t- spect_fw_version")
    print("\t- debug_random_value")
    print("\t- activate_encryption")
    print("\t- init_byte")
    print("\t- busy_iter")

    # Generate the model configuration YAML
    with model_cfg_path.open("w") as f:
        yaml.dump(model_cfg, f, default_flow_style=False)