./rm.sh
make deps
clear
make
./oc 01-hello.oc
cat *.str *.tok *.ast *.sym > results.txt
#vim results.txt

