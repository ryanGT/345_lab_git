#!/usr/bin/env python3
from krauss_misc import txt_mixin
import os
mypath = "/boot/config.txt"
myfile = txt_mixin.txt_file_with_list(mypath)

#possible options:
#dtparam=i2c_arm=on
#dtparam=i2c_arm_baudrate=400000
#
# or
#
#dtparam=i2c_arm=on,i2c_arm_baudrate=400000
#
# one line goal:
# dtparam=i2c_arm=on,i2c_arm_baudrate=400000

# Approach:
# - does i2c_arm=on must exist
# - does i2c_arm=on line contain i2c_arm_baudrate=400000
# - if there are lines containin arm on and bausrate 400000, am I happy?
#     - even if they are on two separate lines?

pat1 = "i2c_arm=on"
pat2 = "i2c_baudrate=400000"


pat_list = [pat1, pat2]
failed_cases = []
    
for i,pat in enumerate(pat_list):
    pat_inds = myfile.findall(pat)
    if len(pat_inds) == 0:
        print("%s not found" % pat)
        failed_cases.append(i)
    else:
        print("%s line(s):" % pat)
        for ind in pat_inds:
            print(myfile.list[ind])
        print("\n")


if len(failed_cases) == 0:
    print("all i2c patterns found, eveything should be fine")
elif 0 in failed_cases:
    print("i2c is not on, nothing should work")
elif 1 in failed_cases:
    # main, expected case
    # replace i2c_arm=on line with full line
    inds = myfile.findall(pat1)
    Ninds = len(inds)
    assert Ninds == 1, "len(inds) = %i, somthing is wrong" % Ninds
    ind = inds[0]
    good_line = "dtparam=i2c_arm=on,i2c_arm_baudrate=400000"
    myfile.list[ind] = good_line
    myfile.save(mypath)




#script_inds = myfile.findall("345_lab_git/scripts_345")
#
#
#bashrc_path = os.path.join(home, ".bashrc")
#print("bashrc_path = %s" % bashrc_path)
#
#new_part = "$HOME/345_lab_git/scripts_345"
#new_line = "PATH=%s:$PATH" % new_part
#export_line = "export PATH"
#
#if len(script_inds) == 0:
#    if len(path_inds) == 0:
#        # append both to end
#        myfile.list.append(new_line)
#        myfile.list.append(export_line)
#        myfile.save(bashrc_path)
#
#
