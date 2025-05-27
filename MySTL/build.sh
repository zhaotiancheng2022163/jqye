#!/bin/sh

rm -rf build bin bbb TAGS
cmake -S . -B build
echo
echo
cmake --build build  -j$(nproc)

echo

read -p "run/gdb/xclip/no [r/g/x/n] "
case $REPLY in
	r) ./bin/test
		;;
	g) gdb ./bin/test
		;;
	x) ./bin/test | xclip -sel clip
		;;
esac

