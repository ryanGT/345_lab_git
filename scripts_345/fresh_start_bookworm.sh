#!/bin/bash
sudo pip3 install --upgrade --break-system-packages krauss_misc
./set_path.py
./install_345_apt_packages.py
./upgrade_drkrauss_python.sh
./install_arduino_libraries_drkrauss.sh
./copy_inputrc.sh
