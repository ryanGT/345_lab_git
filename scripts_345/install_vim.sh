#!/bin/bash
sudo apt-get install vim-gtk
cd 
git clone https://github.com/ryanGT/myvim
mv myvim .vim
ln -s ~/.vim/.vimrc ~/.vimrc
cd ~/.vim
git submodule update --init --recursive
sudo apt-get install universal-ctags
