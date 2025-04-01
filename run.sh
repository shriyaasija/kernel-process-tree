#!/bin/bash

echo "[1] Building the kernel module..."
make

echo "" 

echo "[2] Compiling user space program..."
gcc -o user_module user_module.c

echo ""
echo "[3] Running child processes..."
./user_module

echo "[4] Waiting for parent PID..."
while [ ! -f parent_pid.txt ]; do sleep 1; done
PARENT_PID=$(cat parent_pid.txt)
echo "Found PID: $PARENT_PID"

echo ""

echo "[4] Loading kernel program..."
sudo insmod kernel_module.ko parent_pid=$PARENT_PID

echo ""

echo "[5] Displaying mem tree from kernel module..."
cat /proc/mem_tree

wait USER_PROGRAM_PID

echo "[6] Unloading kernel module..."
sudo rmmod tasks_lister_dfs

echo "[7] Final clean up..."
make clean 

echo "all done :)"
