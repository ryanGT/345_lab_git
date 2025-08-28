#!/bin/bash
cd
if [ -d "git" ];
then
    echo "git directory exists."
else
    mkdir "git"
fi
cd git
git clone https://github.com/ryanGT/rpiblockdiagram.git
