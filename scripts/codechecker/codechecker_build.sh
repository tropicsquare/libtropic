#!/bin/bash
cd tropic01_model/
mkdir -p build
cd build
cmake -DLT_BUILD_EXAMPLES=1 ..
make clean && make