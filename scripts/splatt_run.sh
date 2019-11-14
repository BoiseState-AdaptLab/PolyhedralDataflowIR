#!/bin/bash

export LD_LIBRARY_PATH=/home/edavis/Work/PolyhedralDataflowIR/artifact/lib/splatt/lib:$LD_LIBRARY_PATH
export PATH=/home/edavis/Work/PolyhedralDataflowIR/artifact/lib/splatt/bin:$PATH
splatt cpd --seed 1568224077 -r $2 -i $3 -t $OMP_NUM_THREADS $1

