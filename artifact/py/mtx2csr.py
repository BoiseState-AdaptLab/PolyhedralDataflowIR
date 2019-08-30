#!/usr/bin/env python3

import sys
import scipy.io as io

def mtx2csr(mtxfile):
    csrfile = mtxfile.replace('.mtx', '.csr')

    coo = io.mmread(mtxfile)
    csr = coo.tocsr()

    fout = open(csrfile, 'w')
    fout.write("Manu_generated_file\n")
    fout.write("%d  %d  %d\n" % (coo.shape[0], coo.shape[1], csr.nnz))

    for i in csr.indptr:
        fout.write("%d\n" % (i + 1))
    for i in csr.indices:
        fout.write("%d\n" % (i + 1))
    for d in csr.data:
        fout.write("%.6lf\n" % d)
    fout.close()

    return csrfile

def main():
    if len(sys.argv) > 1:
        mtxfile = sys.argv[1]
        outfile = mtx2csr(mtxfile)
        print("%s => %s" % (mtxfile, outfile))
    else:
        print("usage: mtx2csr.py <in_matrix.mtx>")

try:
    main()
except KeyboardInterrupt as e: # Ctrl-C
    print("Closing gracefully on keyboard interrupt...")
except Exception as e:
    print('ERROR: ' + str(e))
