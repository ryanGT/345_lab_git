#!/usr/bin/env python3
import argparse

parser = argparse.ArgumentParser()
#parser.add_argument('--example', nargs='?', const=1, type=int, default=3)
parser.add_argument('datafile', type=str, \
                    help='csv or txt data file name')
args = parser.parse_args()
datafile = args.datafile

from krauss_misc import txt_mixin

import re, os

myfile = txt_mixin.txt_file_with_list(datafile)
label_inds = myfile.findall("t_ms,",forcestart=1)
label_ind = label_inds[-1]


# go to end of file and search backward for 
# first line that does not start with a float
# followed by a comma
pat1 = r"^-?\d*\.{0,1}\d+,"
p1 = re.compile(pat1)
N = len(myfile.list)

for i in range(1000):
    curline = myfile.list[N-i-1]
    q1 = p1.search(curline)
    if q1:
        #we are at the last line of data
        end_ind = N-i
        break

clean_list = myfile.list[label_ind:end_ind]
label_row = '#' + clean_list[0]
clean_list[0] = label_row

fno, ext = os.path.splitext(datafile)
outname = fno + '_clean' + ext
txt_mixin.dump(outname, clean_list)

