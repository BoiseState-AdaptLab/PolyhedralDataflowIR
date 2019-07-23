#!/usr/bin/env python3

import sys
import traceback as tb
from argparse import ArgumentParser
from csv import DictWriter
from tools import json

def usage():
    print("build_vars <main_file> <header_file> <var_1> ... [<var_n>]")

def main():
    if len(sys.argv) < 2:       # Read from standard in...
        usage()
        return

    main_file = sys.arv[1]
    header_file = sys.argv[2]
    variants = sys.argv[3:]

    stop = 1

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt as e: # Ctrl-C
        print("Closing gracefully on keyboard interrupt...")
    except Exception as e:
        print('ERROR: ' + str(e))
        tb.print_exc()
