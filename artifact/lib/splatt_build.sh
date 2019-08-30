#!/bin/bash
git clone https://github.com/ShadenSmith/splatt.git
cd splatt
./configure --prefix=${PWD} --build-lib=shared --download-blas-lapack
make
make install
cd ..
