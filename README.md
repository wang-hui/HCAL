# DLPHIN test
This is a git repo for UL DLPHIN tests  
Use master branch for 2018 MC and data  
Use the CMSSW_11_1_x branch for run 3 MC  

1. setup CMSSW and this repo
```
cmsrel CMSSW_10_6_12
cd CMSSW_10_6_12/src
cmsenv
git cms-addpkg RecoLocalCalo/HcalRecProducers
git clone -b CMSSW_10_6_x https://github.com/wang-hui/HCAL.git
```

2. add DLPHIN and compile
```
mv HCAL/BuildFile.xml RecoLocalCalo/HcalRecProducers
mv HCAL/HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src
scram b -j 4
```

3. modify the cfg file and run
```
#edit these 2 lines of HCAL/HBHEPhase1Reconstructor_cfi.py if necessary
DLPHIN_scale = cms.bool(True),  #Apply DLPHIN/MAHI scale factor on DLPHIN energy. Set True in data
DLPHIN_save = cms.bool(True),   #If False, DLPHIN will have a "dry run" without being saved
#If True, DLPHIN energy will be saved by overwriting the original (MAHI) energy in HBHERecHits

mv HCAL/HBHEPhase1Reconstructor_cfi.py RecoLocalCalo/HcalRecProducers/python
cd HCAL
cmsRun reco_data_RAW2DIGI_RECO.py
```
