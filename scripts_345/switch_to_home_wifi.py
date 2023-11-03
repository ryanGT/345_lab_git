#!/usr/bin/env python3
from krauss_misc import txt_mixin
import os, shutil

# backup up interface if it is valid static ip

# plan
# - read in interfaces
# - search for a line containing:
#   35.40.171.233
# - verify that the last three digits are valid
#     - print warning and exit if they are not
# - if the ip is valid, copy interfaces to intefaces_static_backup_F23

# after static backup:
# - copy interface_dhcp_home to interfaces
#     - put this dhcp file in ~/345_lab_git/scripts_345

main_path = "/etc/network/interfaces"
static_file = txt_mixin.txt_file_with_list(main_path)

ip_pat = "address 35.40.171."
#ip_pat = "35.40.171."

ind_list = static_file.list.findall(ip_pat)
if len(ind_list) == 0:
    print("did not find static address line")
if len(ind_list) > 1:
    print("found more than one address line")
ind = ind_list[0]

ip_line = static_file.list[ind]
print(ip_line)

pre, rest = ip_line.split("address",1)
ip_str = rest.strip()
ip_parts = ip_str.split(".")
ip_end = int(ip_parts[-1])

valid_list = [233]

backup_name = "interfaces_static_backup_F23"
network_folder = "/etc/network"
interfaces = "interfaces"
backup_path = os.path.join(network_folder,backup_name)

if not os.path.exists(backup_path) and (ip_end in valid_list):
    print("your ip address is valid, backing up your interfaces file")
    src = os.path.join(network_folder, interfaces)
    dst = os.path.join(network_folder, backup_name)
    #shutil.copyfile(src,dst)
    cmd = "sudo cp %s %s" % (src,dst)
    os.system(cmd)
elif os.path.exists(backup_path):
    print("static intefaces file already backuped")
elif ip_end not in valid_list:
    print("could not find valid static ip, exiting")
    exit(1)

