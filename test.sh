#!/bin/bash

WAIT_TIME=2  # 设置等待时间为 2 秒

cd ./build/Linux-x86_64-GCC

echo "cheng:"
./cheng
sleep $WAIT_TIME
# echo
# echo "add:"
# ./add
# echo
echo "tanh:"
./tanh