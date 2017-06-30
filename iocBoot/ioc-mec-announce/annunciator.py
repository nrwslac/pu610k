#!/usr/bin/env python
from psp import Pv
import pyca
import os
import time

scinit = 1
finit = 1
charge_ok = Pv.Pv("MEC:PFN:CHARGE_OK", initialize=True, monitor=True)

def start_charge(e):
    global scinit, charge_ok
    print "sc %d" % scinit
    if scinit > 0:
        scinit -= 1
    elif charge_ok.value != 0:
        os.system("/usr/local/bin/mpg123 -a hw:0,0 charging.mp3")

def fire(e):
    global finit
    print "f %d" % finit
    if finit > 0:
        finit -= 1
    else:
        os.system("/usr/local/bin/mpg123 -a hw:0,0 fire.mp3")

fire_pv     = Pv.Pv("MEC:PFN:FIRE",         initialize=True, monitor=fire)
charging_pv = Pv.Pv("MEC:PFN:START_CHARGE", initialize=True, monitor=start_charge)

while True:
     time.sleep(3600)
     

"""
cs = [Pv.Pv("MEC:PFN:CH0:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH1:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH2:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH3:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH4:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH5:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH6:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH7:CHARGE_STATE", monitor=True),
      Pv.Pv("MEC:PFN:CH8:CHARGE_STATE", monitor=True)]

enable = [Pv.Pv("MEC:PFN:CH0:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH1:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH2:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH3:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH4:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH5:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH6:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH7:ENABLE_RBV", monitor=True),
          Pv.Pv("MEC:PFN:CH8:ENABLE_RBV", monitor=True)]
"""