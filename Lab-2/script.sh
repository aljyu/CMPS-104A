#!/bin/bash
clear
make spotless
rm *.str
rm *.tok
make deps
make
valgrind --leak-check=full ./oc foo.oc
./oc foo.oc > results.txt

