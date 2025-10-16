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

import matplotlib.pyplot as plt
import numpy as np
import control
from numpy import sin, cos, tan, pi
#from control import TransferFunction as TF

import matplotlib
rcfname = matplotlib.matplotlib_fname()
print("rcfname: %s" % rcfname)

import os, glob

plt.rcParams['font.size'] = 14


backend = plt.rcParams['backend']
print("backend: %s" % backend)


def _get_time(data,time_col=0):
    time = data[:,time_col]
    if time.max() > 100:
        # assume milliseconds; convert
        time /= 1000
    return time

data = np.loadtxt(datafile, delimiter=',')
t = _get_time(data)

## plot all data
#plt.figure(1)
#plt.plot(t, data[:,1:])


# - find columns
#     - search for G and if_then
# - plot data
# - do settling time stuff

def load_column_labels(datafile):
    f = open(datafile, 'r')
    label_str = f.readline()
    f.close()
    label_str = label_str.strip()
    labels = label_str.split(',')
    return labels


def find_col(mylabels, vib_label="vib_on"):
    msg = "%s not in column labels:\n %s" % (vib_label, mylabels)
    assert vib_label in mylabels, msg
    vib_ind = mylabels.index(vib_label)
    return vib_ind


mylabels = load_column_labels(datafile)
if_col = find_col(mylabels, "if_then")
pend_col = find_col(mylabels, "G")

if_data = data[:,if_col]
pend_data = data[:,pend_col]
on_ind = np.where(if_data != 0)[0][0]#<-- vib. suppress on
# find previous peak or valley?
# - how do I do this for a peak or a valley?
if pend_data[on_ind-1] > pend_data[on_ind]:
    # the values were coming down
    for i in range(1000):
        if pend_data[on_ind-1-i] < pend_data[on_ind-i]:
            # we have peaked
            peak_ind = on_ind - i
            break
else:
    # the values were going up
    for i in range(1000):
        if pend_data[on_ind-1-i] > pend_data[on_ind-i]:
            # we have bottomed out
            peak_ind = on_ind - i
            break

start_peak = pend_data[peak_ind]
tlims = [t.min(), t.max()]

settle_enc = np.abs(0.04*start_peak)# 4% settling value 
bad_inds = np.where(np.abs(pend_data) > settle_enc)[0]# not settled
settle_ind = bad_inds[-1]+1# one higher than last unsettled index
N = len(t)

plt.figure(2)
plt.plot(t,if_data, t, pend_data)
plt.plot(t[on_ind], if_data[on_ind],'g^', label=None)
plt.plot(t[peak_ind], start_peak,'bs', label=None)
plt.xlabel('Time (sec.)')
plt.ylabel('Signal Amplitude (counts)')
plt.legend(['If Block','Encoder'])
# plot settling bars
plt.plot(tlims,[settle_enc,settle_enc], 'k:', label=None)
plt.plot(tlims,[-settle_enc,-settle_enc], 'k:', label=None)

if settle_ind == N:
    print("did not settle")

else:
    plt.plot(t[settle_ind], pend_data[settle_ind],'ro')
    plt.tight_layout()
    t_s = t[settle_ind] - t[on_ind]
    print("settling time: %0.4g seconds" % t_s)


plt.show()
