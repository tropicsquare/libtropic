#!/bin/bash
cd tests/model_based_project/
mkdir -p build
cd build
cmake ..
make clean && make