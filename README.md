# DLPHIN: Deep Learning Processes for HCAL INtegration

The setup follows
[PdmV's official recommendation](https://twiki.cern.ch/twiki/bin/view/CMS/PdmVLegacy2018Analysis?rev=1)
for 2018 UL

```
cmsrel CMSSW_10_6_12
cd CMSSW_10_6_12/src
cmsenv
git cms-addpkg RecoLocalCalo/HcalRecProducers
git cms-addpkg RecoLocalCalo/HcalRecAlgos
git clone https://github.com/wang-hui/HCAL.git
cd HCAL
python DLPHIN_setup.py cp #this script copies the modified / added files as listed below
cd ..
scram b -j 4
```

Modified files:
```
RecoLocalCalo/HcalRecAlgos/BuildFile.xml
RecoLocalCalo/HcalRecProducers/python/HBHEPhase1Reconstructor_cfi.py
RecoLocalCalo/HcalRecProducers/src/HBHEPhase1Reconstructor.cc
```

Added files:
```
RecoLocalCalo/HcalRecAlgos/interface/DLPHIN.h
RecoLocalCalo/HcalRecAlgos/src/DLPHIN.cc
RecoLocalCalo/HcalRecProducers/python/DLPHIN_cfi.py
```

Test run
```
cd HCAL
cmsRun reco_MC_RAW2DIGI_RECO.py
```
