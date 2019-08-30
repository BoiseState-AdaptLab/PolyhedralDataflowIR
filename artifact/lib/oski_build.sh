#!/bin/bash
wget https://downloads.sourceforge.net/project/oski/oski/1.0.1h/oski-1.0.1h.tar.bz2
tar -jxvf oski-1.0.1h.tar.bz2
mkdir oski
cd oski
export CFLAGS="-O3 -malign-double"
../oski-1.0.1h/configure --prefix=${PWD} --without-papi --enable-bench-all=no
make
make install
cd ..
