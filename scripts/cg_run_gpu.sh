#!/bin/bash
export NRUNS=9
#export OMP_SCHEDULE=auto
#for NTHREADS in 1 2 4 8 16 24 32
#do
#export OMP_NUM_THREADS=$NTHREADS
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/af_shell3/af_shell3.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cant/cant.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/consph/consph.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/Pres_Poisson/Pres_Poisson.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pwtk/pwtk.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS eig
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS coo
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS csr
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS dsr
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS ell
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS csb 64
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/cop20k_A/cop20k_A.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/rma10/rma10.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/pdb1HYS/pdb1HYS.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/scircuit/scircuit.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/shipsec1/shipsec1.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS eig
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS coo
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS csr
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS dsr
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS ell
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS csb 64
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/tomographic1/tomographic1.mtx $NRUNS csb 256
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS eig
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS coo
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS csr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS dsr
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS ell
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS dia
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS csb 64
/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS csb 128
#/home/edavis/Work/pdfl/cg_acc.x /home/edavis/Work/VarDevEddie/themes/Solver/matrices/webbase-1M/webbase-1M.mtx $NRUNS csb 256
#done
