./rm.sh
rm results.txt
make deps
clear
make
./oc example.oc
cat *.str *.tok *.ast *.sym > results.txt
#vim results.txt

