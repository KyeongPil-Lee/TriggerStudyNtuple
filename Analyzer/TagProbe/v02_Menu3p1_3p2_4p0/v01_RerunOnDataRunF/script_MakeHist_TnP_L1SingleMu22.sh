#!bin/bash

# root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu3p1")' >&log_MakeHist_TnP_L1SingleMu22_Menu3p1.log

root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu3p2_v1")' >&log_MakeHist_TnP_L1SingleMu22_Menu3p2_v1.log

root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu3p2_v2")' >&log_MakeHist_TnP_L1SingleMu22_Menu3p2_v2.log


root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu4p0_v1")' >&log_MakeHist_TnP_L1SingleMu22_Menu4p0_v1.log

root -b -q 'MakeHist_TnP_L1SingleMu22.cxx++("Menu4p0_v2")' >&log_MakeHist_TnP_L1SingleMu22_Menu4p0_v2.log

echo "finished"
