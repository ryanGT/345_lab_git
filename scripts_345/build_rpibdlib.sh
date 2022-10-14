#!/bin/bash
cd ~/git/rpiblockdiagram
gcc -o rpiblockdiagram.o -c rpiblockdiagram.cpp
ar rsc librpiblockdiagram.a rpiblockdiagram.o 
sudo cp rpiblockdiagram.h /usr/local/include
sudo cp librpiblockdiagram.a /usr/local/lib
