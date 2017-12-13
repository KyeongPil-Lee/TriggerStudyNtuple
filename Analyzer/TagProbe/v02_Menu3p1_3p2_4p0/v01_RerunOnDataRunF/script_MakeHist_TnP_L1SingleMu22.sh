#!bin/bash

# root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu3p1")' >&log_MakeHist_TnP_L1SingleMu22_Menu3p1.log

root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu3p2")' >&log_MakeHist_TnP_L1SingleMu22_Menu3p2.log

root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu4p0")' >&log_MakeHist_TnP_L1SingleMu22_Menu4p0.log

echo "finished"
