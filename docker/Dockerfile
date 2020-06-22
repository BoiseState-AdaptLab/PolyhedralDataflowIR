FROM ubuntu:bionic
RUN apt-get update && apt-get install -y --no-install-recommends build-essential bison flex libfl-dev \
    cmake ca-certificates curl libssl-dev git libeigen3-dev libgsl23 libgsl-dev libgslcblas0
RUN git clone --depth 1 -b no_deps https://github.com/BoiseState-AdaptLab/PolyhedralDataflowIR.git /usr/src/pdfg
RUN mkdir /usr/src/pdfg/bin && cd /usr/src/pdfg/bin && cmake .. && make -j $(nproc) && mv libpolylib.so ../lib

