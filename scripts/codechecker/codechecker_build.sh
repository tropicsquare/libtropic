#!/bin/bash
cd tropic01_model/
mkdir -p build
cd build
cmake -DLT_BUILD_EXAMPLES=1 -DLT_CAL=mbedtls_v4 ..
make clean && make