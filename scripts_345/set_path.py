#!/usr/bin/env python3
from krauss_misc import txt_mixin
import os
home = os.path.expanduser("~")
os.chdir(home)
myfile = txt_mixin.txt_file_with_list(".bashrc")
path_inds = myfile.findall("$PATH")
script_inds = myfile.findall("345_lab_git/scripts_345")


bashrc_path = os.path.join(home, ".bashrc")
print("bashrc_path = %s" % bashrc_path)

