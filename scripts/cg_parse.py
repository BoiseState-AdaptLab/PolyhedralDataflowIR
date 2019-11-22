#!/usr/bin/env python3

import os
import sys
import traceback as tb
import json
import csv
import subprocess as sub

def usage():
    print("cg_parse <output_file>")

def command(cmd):
    return sub.check_output(cmd, stderr=sub.STDOUT, shell=True).decode('utf-8')

def main():
    if len(sys.argv) < 2:
        usage()
        return

    out_file = open(sys.argv[1])
    csv_file = open(sys.argv[1].replace('.out', '.csv'), 'w', newline='')
    writer = None

    for line in out_file:
        if line.count(':') == 1:
            (exe, results) = line.split(':')
            pos = exe.rfind('/')
            exe = exe[pos+1:]
            items = results.strip().split(',')

            data = {'exec': exe}
            for item in items:
                if '=' in item:
                    (key, val) = item.split('=')
                    data[key] = val

            if 'format' in data and data['format'].startswith('eig'):
                data['format'] += 'en'

            if 'matrix' in data:
                matrix = data['matrix'].replace('\'','')
                pos = matrix.rfind('/')
                matrix = matrix[pos+1:].replace('.mtx', '')
                data['matrix'] = matrix

            if writer is None:
                writer = csv.DictWriter(csv_file, fieldnames=data.keys())
                writer.writeheader()
            writer.writerow(data)
        else:
            print('ERROR: ' + line.rstrip())

    out_file.close()
    csv_file.close()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt as e: # Ctrl-C
        print("Closing gracefully on keyboard interrupt...")
    except Exception as e:
        print('ERROR: ' + str(e))
        tb.print_exc()
