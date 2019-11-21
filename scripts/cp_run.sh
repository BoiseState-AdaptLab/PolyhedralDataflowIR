#!/bin/bash

export SPLATTHOME=/home/edavis/Work/PolyhedralDataflowIR/artifact/lib/splatt
export LD_LIBRARY_PATH=$SPLATTHOME/lib:$LD_LIBRARY_PATH
export PATH=$SPLATTHOME/bin:$PATH
export TNS_PATH=/home/edavis/Work/VarDevEddie/themes/tensors
export OMP_NUM_THREADS=16
export OMP_SCHEDULE=auto
export RAND_SEED=1568224077
export TNS_RANK=10
export MAX_ITER=50
export NRUNS=1

#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/nips.tns &> $TNS_PATH/nips_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/nell-1.tns &> $TNS_PATH/nell1_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/nell-2.tns &> $TNS_PATH/nell2_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/enron.tns &> $TNS_PATH/enron_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/delicious-3d.tns &> $TNS_PATH/deli3d_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/chicago-crime-geo.tns &> $TNS_PATH/crime_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/flickr-3d.tns &> $TNS_PATH/flickr_splatt.out
#splatt cpd --seed $RAND_SEED -r $TNS_RANK -i $MAX_ITER -t $OMP_NUM_THREADS $TNS_PATH/uber.tns &> $TNS_PATH/uber_splatt.out

./cp_par $TNS_PATH/nips.tns $NRUNS coo 7
./cp_par $TNS_PATH/nips.tns $NRUNS hicoo 7 10 64
./cp_par $TNS_PATH/nips.tns $NRUNS hicoo 7 10 128
./cp_par $TNS_PATH/nips.tns $NRUNS hicoo 7 10 256
