rm -rf build
mkdir build
cd build
cmake -DBUILD_DOCS=1 -DUSE_TREZOR_CRYPTO=1 ..
make doc_doxygen
mkdir packed
cd docs/doxygen/latex/
make
mv refman.pdf ../../../packed/
cd ../
mv html/ ../../packed/
cd ../../
tar -zcvf packed.tar.gz packed/
