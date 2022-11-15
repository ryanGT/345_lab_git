#!/bin/bash
cd ~/git/rpiblockdiagram
git pull origin main
build_rpibdlib.sh
upgrade_pybd_pip_stuff.sh
cp ~/345_lab_git/matplotlibrc ~/.config/matplotlib
