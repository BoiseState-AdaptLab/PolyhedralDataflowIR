#!/bin/bash
wget https://sparse.tamu.edu/MM/Williams/cant.tar.gz
tar -zxvf cant.tar.gz
rm -f cant.tar.gz
../py/mtx2csr.py ./cant/cant.mtx
wget https://sparse.tamu.edu/MM/Williams/consph.tar.gz
tar -zxvf consph.tar.gz
rm -f consph.tar.gz
../py/mtx2csr.py ./consph/consph.mtx
wget https://sparse.tamu.edu/MM/Williams/mc2depi.tar.gz
tar -zxvf mc2depi.tar.gz
rm -f mc2depi.tar.gz
../py/mtx2csr.py ./mc2depi/mc2depi.mtx
wget https://sparse.tamu.edu/MM/Williams/cop20k_A.tar.gz
tar -zxvf cop20k_A.tar.gz
rm -f cop20k_A.tar.gz
../py/mtx2csr.py ./cop20k_A/cop20k_A.mtx
wget https://sparse.tamu.edu/MM/Williams/mac_econ_fwd500.tar.gz
tar -zxvf mac_econ_fwd500.tar.gz
rm -f mac_econ_fwd500.tar.gz
../py/mtx2csr.py ./mac_econ_fwd500/mac_econ_fwd500.mtx
wget https://sparse.tamu.edu/MM/Williams/pdb1HYS.tar.gz
tar -zxvf pdb1HYS.tar.gz
rm -f pdb1HYS.tar.gz
../py/mtx2csr.py ./pdb1HYS/pdb1HYS.mtx
wget https://sparse.tamu.edu/MM/Hamm/scircuit.tar.gz
tar -zxvf scircuit.tar.gz
rm -f scircuit.tar.gz
../py/mtx2csr.py ./scircuit/scircuit.mtx
wget https://sparse.tamu.edu/MM/Bova/rma10.tar.gz
tar -zxvf rma10.tar.gz
rm -f rma10.tar.gz
../py/mtx2csr.py ./rma10/rma10.mtx
wget https://sparse.tamu.edu/MM/Boeing/pwtk.tar.gz
tar -zxvf pwtk.tar.gz
rm -f pwtk.tar.gz
../py/mtx2csr.py ./pwtk/pwtk.mtx
wget https://sparse.tamu.edu/MM/DNVS/shipsec1.tar.gz
tar -zxvf shipsec1.tar.gz
rm -f shipsec1.tar.gz
../py/mtx2csr.py ./shipsec1/shipsec1.mtx
wget https://sparse.tamu.edu/MM/ACUSIM/Pres_Poisson.tar.gz
tar -zxvf Pres_Poisson.tar.gz
rm -f Pres_Poisson.tar.gz
../py/mtx2csr.py ./Pres_Poisson/Pres_Poisson.mtx
