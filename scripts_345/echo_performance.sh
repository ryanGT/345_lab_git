#!/bin/bash
echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
