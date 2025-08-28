#!/bin/bash
cd
if [ -d "Arduino" ];
then
    echo "Arduino directory exists."
else
    mkdir Arduino
    cd Arduino
    mkdir libraries
fi
cd git
g
cd ~/Arduino/libraries/
git clone https://github.com/ryanGT/rtblockdiagram.git
git clone https://github.com/ryanGT/kraussserial.git 
