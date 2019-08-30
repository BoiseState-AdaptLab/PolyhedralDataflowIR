#!/usr/bin/env python3

import sys
import os
import subprocess as sub

NRUNS = 3

def run(args, vars={}, stream=None):
    output = ''
    for key in vars:
        os.environ[key] = vars[key]

    try:
        data = sub.check_output(args, env=os.environ, stderr=sub.STDOUT)
        output = data.decode()
    except Exception as e:
        eprint(str(e))

    if stream is not None:
        stream.write(output)
    return output

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def ncpus():
    import multiprocessing
    return multiprocessing.cpu_count()

def csr_bsr(nruns=NRUNS):
    matrices = sorted(['cant', 'consph', 'mc2depi', 'cop20k_A', 'mac_econ_fwd500', 'pdb1HYS',
                       'scircuit', 'rma10', 'pwtk', 'shipsec1', 'Pres_Poisson'])
    executable = 'csr_bsr_insp'
    for matrix in matrices:
        for i in range(nruns):
            out = run(['cmake-build-debug/' + executable, 'data/' + matrix + '/' + matrix + '.csr'])
            stop = 1


def coo_csf(nruns=NRUNS):
    tensors = ['chicago-crime-geo', 'enron', 'nell-2', 'nips']
    executable = 'coo_csf_insp'
    for tensor in tensors:
        for i in range(nruns):
            out = run(['cmake-build-debug/' + executable, 'data/' + tensor + '.tns'])
            print(out.rstrip())

def mfd_run(nruns=NRUNS):
    cell_sizes = [16, 128]
    box_sizes = [14336, 32]
    nthreads = ncpus()
    variants = ['series', 'fuse_within', 'fuse_all']

    for variant in variants:
        executable = 'mfd_' + variant
        for i in range(len(cell_sizes)):
            ncells = cell_sizes[i]
            nboxes = box_sizes[i]
            for n in range(nruns):
                out = run(['cmake-build-debug/' + executable, '-C', str(ncells), '-B', str(nboxes), '-p', str(nthreads)])
                print(out.rstrip())

def main(nruns=NRUNS):
    #csr_bsr()
    coo_csf()
    mfd_run()

try:
    main()
except KeyboardInterrupt as e: # Ctrl-C
    print("Closing gracefully on keyboard interrupt...")
except Exception as e:
    print('ERROR: ' + str(e))
