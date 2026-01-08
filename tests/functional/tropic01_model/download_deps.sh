#!/usr/bin/env bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

rm -rf "$SCRIPT_DIR/_deps"
mkdir -p "$SCRIPT_DIR/_deps"

echo "Downloading ed25519..."
git clone https://github.com/orlp/ed25519.git "$SCRIPT_DIR/_deps/ed25519"
git -C "$SCRIPT_DIR/_deps/ed25519" checkout b1f19fab4aebe607805620d25a5e42566ce46a0e 

echo "Downloading Micro ECC..."
curl -L -o "$SCRIPT_DIR/_deps/micro-ecc.zip" "https://github.com/kmackay/micro-ecc/archive/refs/tags/v1.1.zip"

# Verify micro-ecc zip checksum
echo "Verifying micro-ecc.zip checksum..."
EXPECTED_MICRO_ECC="67cc3867dda3860335780ddd8004d69d82afeaac8c0aa630e29112a2b5be153d"
ACTUAL_MICRO_ECC=$(sha256sum "$SCRIPT_DIR/_deps/micro-ecc.zip" | awk '{print $1}')
if [ "$EXPECTED_MICRO_ECC" != "$ACTUAL_MICRO_ECC" ]; then
  echo "Checksum mismatch for micro-ecc.zip: expected $EXPECTED_MICRO_ECC, got $ACTUAL_MICRO_ECC" >&2
  exit 1
fi

unzip "$SCRIPT_DIR/_deps/micro-ecc.zip" -d "$SCRIPT_DIR/_deps"
mv "$SCRIPT_DIR/_deps/micro-ecc-1.1" "$SCRIPT_DIR/_deps/micro-ecc"
rm "$SCRIPT_DIR/_deps/micro-ecc.zip"

echo "Downloading MbedTLSv4..."
curl -L -o "$SCRIPT_DIR/_deps/mbedtls.tar.bz2" "https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2"

# Verify mbedtls tar.bz2 checksum
echo "Verifying mbedtls.tar.bz2 checksum..."
EXPECTED_MBEDTLS="2f3a47f7b3a541ddef450e4867eeecb7ce2ef7776093f3a11d6d43ead6bf2827"
ACTUAL_MBEDTLS=$(sha256sum "$SCRIPT_DIR/_deps/mbedtls.tar.bz2" | awk '{print $1}')
if [ "$EXPECTED_MBEDTLS" != "$ACTUAL_MBEDTLS" ]; then
  echo "Checksum mismatch for mbedtls.tar.bz2: expected $EXPECTED_MBEDTLS, got $ACTUAL_MBEDTLS" >&2
  exit 1
fi

tar -xjf "$SCRIPT_DIR/_deps/mbedtls.tar.bz2" -C "$SCRIPT_DIR/_deps"
rm "$SCRIPT_DIR/_deps/mbedtls.tar.bz2"
mv "$SCRIPT_DIR/_deps/mbedtls-4.0.0" "$SCRIPT_DIR/_deps/mbedtls_v4"