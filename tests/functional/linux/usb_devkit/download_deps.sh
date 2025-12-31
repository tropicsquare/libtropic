#!/usr/bin/env bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

rm -rf "$SCRIPT_DIR/_deps"
mkdir -p "$SCRIPT_DIR/_deps"

echo "Downloading ed25519..."
git clone https://github.com/orlp/ed25519.git _deps/ed25519

echo "Downloading Micro ECC..."
curl -L -o "$SCRIPT_DIR/_deps/micro-ecc.zip" "https://github.com/kmackay/micro-ecc/archive/refs/tags/v1.1.zip"
unzip "$SCRIPT_DIR/_deps/micro-ecc.zip" -d "$SCRIPT_DIR/_deps"
mv "$SCRIPT_DIR/_deps/micro-ecc-1.1" "$SCRIPT_DIR/_deps/micro-ecc"
rm "$SCRIPT_DIR/_deps/micro-ecc.zip"

echo "Downloading MbedTLSv4..."
curl -L -o "$SCRIPT_DIR/_deps/mbedtls.tar.bz2" "https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-4.0.0/mbedtls-4.0.0.tar.bz2"
tar -xjf "$SCRIPT_DIR/_deps/mbedtls.tar.bz2" -C "$SCRIPT_DIR/_deps"
rm "$SCRIPT_DIR/_deps/mbedtls.tar.bz2"
mv "$SCRIPT_DIR/_deps/mbedtls-4.0.0" "$SCRIPT_DIR/_deps/mbedtls_v4"