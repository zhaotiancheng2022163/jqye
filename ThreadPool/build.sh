#!/bin/sh

# 这是构建项目的bash脚本


path=build
rm -rf ${path} ThreadPool lib  bin
cmake -S . -B ${path}
cmake --build ${path} --config Release -j$(nproc)
echo
echo
./bin/ThreadPool
#./bin/ThreadPool && rm -f  valgrind.txt && valgrind -s  --log-file=./valgrind.txt --leak-check=full --track-origins=yes --show-leak-kinds=all ./bin/ThreadPool


