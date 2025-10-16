#!/usr/bin/env python3
from krauss_misc import txt_mixin
import os, shutil


def sudo_copy(src, dst):
    cmd = "sudo cp %s %s" % (src,dst)
    os.system(cmd)


# backup must already exist
# - exit if it doesn't
backup_name = "interfaces_static_backup_F23"
network_folder = "/etc/network"
interfaces = "interfaces"
backup_path = os.path.join(network_folder,backup_name)

if not os.path.exists(backup_path):
    print("static ip backup file doesn't exist.  exiting")
    exit(1)



# copy backup to main
main_path = "/etc/network/interfaces"

src = backup_path
dst = main_path
print(" %s --> %s" % (src, dst))
sudo_copy(src, dst)

