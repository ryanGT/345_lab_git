#!/usr/bin/env python3
import argparse

parser = argparse.ArgumentParser()
#parser.add_argument('--example', nargs='?', const=1, type=int, default=3)
parser.add_argument('datafile', type=str, \
                    help='csv or txt data file name')
args = parser.parse_args()
datafile = args.datafile

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

def _get_time(data,time_col=1):
    time = data[:,time_col]
    if time.max() > 100:
        # assume milliseconds; convert
        time /= 1000
    return time


def plot_enc_data(data,time_col=1,enc_col=-1):
    time = _get_time(data, time_col=time_col)
    encoder = data[:,enc_col]
    
    plt.figure()
    plt.plot(time,encoder,label='Pend. Encoder')
    line_time = [time[0],time[-1]]
    mymax = 30
    plt.plot(line_time, [mymax, mymax],'k--',label=None)
    plt.plot(line_time, [-mymax, -mymax],'k--',label=None)

    
    plt.xlabel('Time (sec.)')
    plt.ylabel('Encoder Counts')
    plt.tight_layout()


#fn = "Willemstein_Witherell_Trial1_VibrationSuppression.csv"
#data = np.loadtxt(fn, skiprows=1, delimiter=',')
#print("fn = %s" % fn)
#print("shape = (%s, %s)" % data.shape)
#calc_settling_time_event_1(data)
#plot_settling_time_fig(data)



## Steps:
## - load data file
## - find vib_on and pend_enc columns
## - plot the data
## - find settling time
## - plot symbols




# find column labels:

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


data = np.loadtxt(datafile, delimiter=',', skiprows=1)
mylabels = load_column_labels(datafile)
pend_col = find_col(mylabels, "pend_enc")

encoder = data[:,pend_col]

enc_max = encoder.max()
enc_min = encoder.min()


print("\n"*2)
print("encoder max: %s" % enc_max)
print("encoder min: %s" % enc_min)


plot_enc_data(data,time_col=1,enc_col=pend_col)


plt.show()
