#!/bin/bash

WAIT_TIME=1  # 设置等待时间为 1 秒

cd ./build/Linux-x86_64-GCC

# echo "cheng:"
# ./cheng
# sleep $WAIT_TIME
# echo
# echo "add:"
# ./add
# sleep $WAIT_TIME
# echo
# echo "tanh:"
# ./tanh
sleep $WAIT_TIME
echo
echo "convert:"
./convert