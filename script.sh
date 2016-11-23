#!/bin/bash
clear
make spotless
rm *.str
rm *.tok
make deps
make
valgrind --leak-check=full ./oc foo.oc
echo ""
echo ""
echo "foo.str"
cat foo.str
echo ""
echo ""
echo "foo.tok"
cat foo.tok

