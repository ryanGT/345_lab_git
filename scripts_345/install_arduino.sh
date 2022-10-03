#!/bin/bash
cp arduino-1.8.19-linuxarm.tar.xz ~/
cd 
tar -xvf arduino-1.8.19-linuxarm.tar.xz
cd
cd ~/arduino-1.8.19/
sudo ./install.sh
#install my libraries
## this folder might not exist yet (until after Arduino is 
## launched the first time)
cd ~/Arduino/libraries/
git clone https://github.com/ryanGT/rtblockdiagram.git
git clone https://github.com/ryanGT/kraussserial.git 
