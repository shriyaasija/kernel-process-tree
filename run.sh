#!/bin/bash

echo "[1] Building the kernel module..."
make

echo "" 

echo "[2] Compiling user space program..."
gcc -o user_module user_module.c

echo ""

echo "[3] Running child processes..."
gnome-terminal -- bash -c "./user_module; exec bash" &
sleep 2

echo ""

echo "[4] Waiting for parent PID..."
PARENT_PID=$(pgrep -n user_module)

if [ -z "$PARENT_PID" ]; then
	echo "Failed to get user module PID"
	exit 1
fi

echo "User module PID: $PARENT_PID"

echo ""

echo "[5] Loading kernel program..."
sleep 45
sudo insmod kernel_module.ko parent_pid=$PARENT_PID

echo ""

echo "[6] Displaying mem tree from kernel module..."
gnome-terminal -- bash -c "cat /proc/mem_tree; exec bash"

echo ""

echo "[7] Unloading kernel module..."
sudo rmmod kernel_module

echo "[8] Final clean up..."
make clean 

echo ""

echo "all done :)"
