#!/usr/bin/env python3
from krauss_misc import txt_mixin
import os

myfile = txt_mixin.txt_file_with_list('package_list.txt')
mylist = myfile.list
import os
for line in mylist:
    cmd = "sudo apt -y install %s" % line
    print(cmd)
    os.system(cmd)
