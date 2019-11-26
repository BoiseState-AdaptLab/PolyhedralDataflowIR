#!/bin/bash
pgc++ -w -fast -Minfo=accel -ta=tesla:cc60,fastmath,managed,beta -O3 -g -I./out -I./include -I./test -I./src ./test/cg_acc.cpp -o cg_acc.x
