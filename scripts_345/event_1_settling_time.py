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


def _find_start_max_and_settle_inds(data,enc_col=1,vib_on_col=2):
    encoder = data[:,enc_col]
    # analysis starts when vib_on_col goes to 1
    vib_on = data[:,vib_on_col]
    start_ind = np.where(vib_on>0)[0][0]
    trun_enc = encoder[start_ind:]
    # settling time is based on the maximum encoder value after
    # vibration suppression turns on:
    max_ind = np.abs(trun_enc).argmax() + start_ind
    start_enc = encoder[max_ind]
    settle_enc = np.abs(0.04*start_enc)# 4% settling value 
    # the pendulum encoder has settled when if it no longer
    # outside +/- settle_enc
    # - the index where it settles is one more than the last time
    #   it was outside the settling range:
    bad_inds = np.where(np.abs(encoder) > settle_enc)[0]# not settled
    settle_ind = bad_inds[-1]+1# one higher than last unsettled index
    return start_ind, max_ind, settle_ind



def calc_settling_time_event_1(data,time_col=1,enc_col=1,vib_on_col=2):
    time = _get_time(data,time_col)
    start_ind, max_ind, settle_ind = _find_start_max_and_settle_inds(data, \
                                                                     enc_col=enc_col,\
                                                                     vib_on_col=vib_on_col)
    if settle_ind >= len(time)-5:
        # did not settle
        print("did not settle")
        return 1e6
    else:
        ts = time[settle_ind] - time[start_ind]
        return ts



def plot_settling_time_fig(data,time_col=1,enc_col=1,vib_on_col=2):
    time = _get_time(data, time_col=time_col)
    encoder = data[:,enc_col]
    vib_on = data[:,vib_on_col]

    if np.max(np.abs(vib_on)) < 10:
        vib_on *= 100
    
    start_ind, max_ind, settle_ind = _find_start_max_and_settle_inds(data)
    start_enc = encoder[max_ind]
    settle_enc = np.abs(0.04*start_enc)
    
    plt.figure()
    plt.plot(time,encoder,label='Pend. Encoder')
    plt.plot(time,vib_on,'--',linewidth=2,label='Vib. Suppress On')
    plt.plot(time[max_ind],encoder[max_ind],'ro',label='Max. Enc.')
    
    myt = [time[start_ind],time.max()]
    plt.plot(myt,[settle_enc,settle_enc],'k:', label=None)
    plt.plot(myt,[-settle_enc,-settle_enc],'k:',label=None)
        
    if settle_ind < len(time):    
        plt.plot(time[settle_ind],encoder[settle_ind],'g^',label='Settled Enc.')
    
    
    plt.xlabel('Time (sec.)')
    plt.ylabel('Encoder Counts')
    plt.legend(loc=4)
    #plt.ylim([-5,5])
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
vib_col = find_col(mylabels, "vib_on")
pend_col = find_col(mylabels, "pend_enc")

print("\n"*2)

plot_settling_time_fig(data,time_col=1,enc_col=pend_col,vib_on_col=vib_col)

ts = calc_settling_time_event_1(data,time_col=1, \
                                enc_col=pend_col, \
                                vib_on_col=vib_col)
print("settling time: %0.6g" % ts)

plt.show()
