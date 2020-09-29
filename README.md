# DLPHIN test
This is a git repo for DLPHIN test using 2018 MC and data  
For run 3 MC, please check the CMSSW_11_1_x branch  

1. setup CMSSW and this repo
```
cmsrel CMSSW_10_2_18
cd CMSSW_10_2_18/src
cmsenv
git cms-addpkg RecoLocalCalo/HcalRecProducers
git clone https://github.com/wang-hui/HCAL.git
```

2. add DLPHIN and compile
```
mv HCAL/HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src
mv HCAL/BuildFile.xml RecoLocalCalo/HcalRecProducers
scram b -j 4
```

3. run on MC
```
cd HCAL
mkdir results_temp
cmsRun reco_test_RAW2DIGI_RECO.py <ntuple_list> > results_temp/reco_test.stdout
mkdir split_file
python split_file.py
python add_gen_energy.py
python compare_gen_reco.py
root plot_HCAL.C
```

4. run on data
```
mkdir results_temp
cmsRun reco_data_RAW2DIGI_RECO.py <ntuple_list> > results_temp/reco_data.stdout
python make_data_csv.py
python compare_DLPHIN_reco.py
root plot_HCAL_data.C
```
