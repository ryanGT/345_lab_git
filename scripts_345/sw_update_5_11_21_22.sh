#!/bin/bash
cd ~/git/rpiblockdiagram
git pull origin main
build_rpibdlib.sh
upgrade_pybd_pip_stuff.sh
cd ~/345_lab_git/scripts_345
rpibd_build.py servo_write.c
rpibd_build.py read_encoder.c
rpibd_build.py zero_encoder.c


