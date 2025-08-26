#!/bin/bash

rm -rf ./bin/test
rm -rf build bin bbb TAGS core*
cmake -S . -B build
echo
echo
cmake --build build  -j"$(nproc)"

echo

# echo -n "run/gdb/xclip/no [r/g/x/n] "
# read -r
# case $REPLY in
	# r) ./bin/test
./bin/test
		# ;;
# 	g) gdb ./bin/test
# 		;;
# 	x) ./bin/test | xclip -sel clip
# 		;;
# esac


rm valgrind-test.log

va ./bin/test
m valgrind-test.log
