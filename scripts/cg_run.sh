#!/bin/bash

matrices=(
    "af_shell3"
    "cant"
    "consph"
    "mc2depi"
    "Pres_Poisson"
    "pwtk"
    "shipsec1"
    "mac_econ_fwd500"
    "cop20k_A"
    "rma10"
    "pdb1HYS"
    "scircuit"
    "shipsec1"
    "chesapeake"
    "tomographic1"
    "webbase-1M"
)

formats=(
    "coo"
    "csr"
    "dsr"
    "ell"
    "csb"
    "dia"
)

root=/home/edavis/Work
exe=${root}/PolyhedralDataflowIR/cg_par
path=${root}/VarDevEddie/themes/Solver/matrices
nruns=9

for mtx in "${matrices[@]}"; do
    for fmt in "${formats[@]}"; do
        cmd="${exe} ${path}/$mtx/${mtx}.mtx $nruns $fmt"
        echo $cmd
        #`$cmd`
    done
done
