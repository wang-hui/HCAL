#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_10_2_12_patch1`
cd CMSSW_10_2_12_patch1/src
eval `scramv1 runtime -sh`
git cms-init --upstream-only
git cms-addpkg RecoLocalCalo/HcalRecProducers
git clone https://github.com/wang-hui/HCAL.git
mv HCAL/HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src
mv HCAL/BuildFile.xml RecoLocalCalo/HcalRecProducers
scram b -j 4
cd HCAL
mv ${_CONDOR_SCRATCH_DIR}/reco_test_RAW2DIGI_RECO.py .
mv ${_CONDOR_SCRATCH_DIR}/FileList.tar .
tar -xvf FileList.tar
pwd

mkdir results_temp
cmsRun reco_test_RAW2DIGI_RECO.py $1 > results_temp/reco_test.stdout

mkdir split_file
python split_file.py
#tar -cvf split_file.tar split_file
ls

python add_gen_energy.py

python compare_gen_reco.py

xrdcp results_temp/reco_test.stdout root://cmseos.fnal.gov//${2}/reco_test_${3}.stdout
xrdcp results_temp/result.csv root://cmseos.fnal.gov//${2}/result_${3}.csv
xrdcp results_temp/result_origin.root root://cmseos.fnal.gov//${2}/result_${3}.root
