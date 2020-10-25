#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_10_2_18`
cd CMSSW_10_2_18/src
eval `scramv1 runtime -sh`
git cms-init --upstream-only
git cms-addpkg RecoLocalCalo/HcalRecProducers
git clone https://github.com/wang-hui/HCAL.git
mv HCAL/BuildFile.xml RecoLocalCalo/HcalRecProducers
mv HCAL/HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src
mv ${_CONDOR_SCRATCH_DIR}/HBHEPhase1Reconstructor_cfi.py RecoLocalCalo/HcalRecProducers/python
scram b -j 4
cd HCAL
mv ${_CONDOR_SCRATCH_DIR}/reco_data_RAW2DIGI_RECO.py .
mv ${_CONDOR_SCRATCH_DIR}/FileList.tar .
tar -xvf FileList.tar
pwd

mkdir results_temp
cmsRun reco_data_RAW2DIGI_RECO.py $1 > results_temp/reco_data.stdout

python make_data_csv.py
python compare_DLPHIN_reco.py

xrdcp results_temp/reco_data.stdout root://cmseos.fnal.gov//${2}/reco_data_${3}.stdout
xrdcp results_temp/result_data.csv root://cmseos.fnal.gov//${2}/result_data_${3}.csv
xrdcp results_temp/result_data_origin.root root://cmseos.fnal.gov//${2}/result_data_${3}.root
