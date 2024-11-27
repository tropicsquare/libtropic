#!/bin/bash
cd tests/integration/
mkdir -p build
cd build
cmake ..
make clean && make