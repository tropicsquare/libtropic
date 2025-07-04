#!/bin/sh
rm -r build/
cmake ./ -B build \
-DLT_BUILD_TESTS=1 \
-DCMAKE_BUILD_TYPE=Debug \
-DLT_USE_ASSERT=1
cd build/
make