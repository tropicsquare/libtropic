#!/bin/sh
rm -r build/
cmake ./ -B build \
-DLT_BUILD_TESTS=1 \
-DLT_SH0_PRIV_PATH=../provisioning_data/2025-06-27T07-51-29Z__prod_C2S_T200__provisioning__lab_batch_package/sh0_key_pair/sh_x25519_private_key_2025-03-24T09-15-15Z.pem \
-DCMAKE_BUILD_TYPE=Debug \
-DLT_USE_ASSERT=1
cd build/
make