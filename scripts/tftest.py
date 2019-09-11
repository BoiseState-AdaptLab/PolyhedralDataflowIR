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

from scipy.io.matlab import loadmat
from sktensor import sptensor, cp_als
import logging

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
    return (indices, data, dims)

def tensorly_test(tns):
    #rng = check_random_state(1234)
    tensor = T.tensor(tns.values) #rng.random_sample((3, 4, 2)))
    factors = parafac(tensor, rank=4, n_iter_max=200, init='svd', tol=10e-5)
    stop=1

def pinv_test(size=4):
    A = np.random.randn(size, size)

    start = time.clock()
    (u, s, vt) = np.linalg.svd(A, full_matrices=True)
    print(u.shape, s.shape, vt.shape)
    B0 = np.matmul(np.transpose(vt), np.multiply(s, np.transpose(u)))
    stop0 = time.clock() - start

    start = time.clock()
    B1 = np.linalg.pinv(A)
    stop1 = time.clock() - start

    start = time.clock()
    B2 = sp.linalg.pinv(A)
    stop2 = time.clock() - start

    start = time.clock()
    B3 = sp.linalg.pinv2(A)
    stop3 = time.clock() - start

    res0 = np.allclose(B0, B1)
    res1 = np.allclose(A, np.dot(A, np.dot(B1, A)))
    res2 = np.allclose(B1, B2)
    res3 = np.allclose(B2, B3)
    return res1 #and res2 and res3

def sktensor_test(indices, vals, dims):
    # Set logging to DEBUG to see CP-ALS information
    logging.basicConfig(level=logging.DEBUG)

    # Convert TF data into SK rep...
    ilist = [[] for i in range(len(dims))]
    for sub in indices:
        [ilist[i].append(sub[i]) for i in range(len(sub))]

    spt = sptensor(tuple(ilist), list(vals), shape=dims, dtype=np.float)

    # Decompose tensor using CP-ALS
    rank = 10       # Match SPLATT default...
    start = time.clock()
    (cpt, fit, itr) = cp_als(spt, rank, init='random', max_iter=11)#, fit_method=None)
    elapsed = time.clock() - start
    #res = np.allclose(spt, cpt.totensor())
    print("CPD: fit=%lg, iter=%d, elapsed=%lg" % (fit, itr+1, elapsed))
    stop=1

def main():
    indices, data, dims = read_tensor(sys.argv[1])
    sktensor_test(indices, data, dims)
    return

    tns = tf.SparseTensor(indices, data, dims)
    tensorly_test(tns)
    pinv_test()

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
