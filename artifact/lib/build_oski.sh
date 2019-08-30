#!/bin/bash
wget https://downloads.sourceforge.net/project/oski/oski/1.0.1h/oski-1.0.1h.tar.bz2
tar -jxvf oski-1.0.1h.tar.bz2
mkdir oski
cd oski
../oski-1.0.1h/configure --prefix=${PWD} --without-papi
make
make install
cd ..
