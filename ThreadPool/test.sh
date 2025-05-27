#!/bin/sh

# 这是一个循环测试的bash脚本

while (( 1 )) ; do
	echo
	echo
# if [[  -f core* ]];then
if [[  $(find . -name "core*") ]];then
	return
fi
./build.sh
done
