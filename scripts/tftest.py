#!/usr/bin/env python3

import os
import re
import sys
import argparse as ap
import traceback as tb
import numpy as np
import pandas as pd
import tensorflow as tf
import time

import scipy as sp
import tensorly as tl
from tensorly import backend as T
from tensorly.decomposition.candecomp_parafac import parafac

def read_tensor(tnsfile):
    df = pd.read_csv(tnsfile, delimiter=" ", header=None)
    (nnz, order) = df.shape
    order -= 1
    cols = [i for i in range(order)]
    indices = df.iloc[:, cols]
    data = df.iloc[:, order:].values.flatten()
    dims = indices.max().values
    indices -= 1
    indices = indices.values
    tns = tf.SparseTensor(indices, data, dims)
    return tns

def tensorly_test(tns):
    #rng = check_random_state(1234)
    tensor = T.tensor(tns.values) #rng.random_sample((3, 4, 2)))
    factors = parafac(tensor, rank=4, n_iter_max=200, init='svd', tol=10e-5)
    stop=1

def pinv_test(size=4):
    A = np.random.randn(size, size)
    start = time.clock()
    B1 = np.linalg.pinv(A)
    stop1 = time.clock() - start
    start = time.clock()
    B2 = sp.linalg.pinv(A)
    stop2 = time.clock() - start
    start = time.clock()
    B3 = sp.linalg.pinv2(A)
    stop3 = time.clock() - start
    res1 = np.allclose(A, np.dot(A, np.dot(B1, A)))
    res2 = np.allclose(A, np.dot(A, np.dot(B2, A)))
    res3 = np.allclose(A, np.dot(A, np.dot(B3, A)))
    return res1 #and res2 and res3

def main():
    pinv_test(128)
    tns = read_tensor(sys.argv[1])
    tensorly_test(tns)
    t0 = time.time()
    sum = tf.sparse.add(tns, tns)
    t1 = time.time()
    elapsed = t1 - t0
    val = sum.values
    print('elapsed: %.6lf' % elapsed)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt as e: # Ctrl-C
        print("Closing gracefully on keyboard interrupt...")
    except Exception as e:
        print('ERROR: ' + str(e))
        tb.print_exc()
