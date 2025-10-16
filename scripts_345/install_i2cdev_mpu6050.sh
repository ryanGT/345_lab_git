#!/bin/bash
cd
if [ -d "git" ];
then
    echo "git directory exists."
else
    mkdir "git"
fi
cd git
git clone https://github.com/jrowberg/i2cdevlib.git

cd ~/Arduino/libraries/
ln -s ~/git/i2cdevlib/Arduino/I2Cdev I2Cdev 
ln -s ~/git/i2cdevlib/Arduino/MPU6050 MPU6050
