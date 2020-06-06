#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
#export SCRAM_ARCH=slc6_amd64_gcc630
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_11_1_0_pre8`
cd CMSSW_11_1_0_pre8/src
eval `scramv1 runtime -sh`
git cms-init --upstream-only
git cms-addpkg RecoLocalCalo/HcalRecAlgos
mv ${_CONDOR_SCRATCH_DIR}/MahiFit.cc RecoLocalCalo/HcalRecAlgos/src/
git clone https://github.com/wang-hui/HCAL.git -b CMSSW_11_1_x
scram b
cd ${_CONDOR_SCRATCH_DIR}
tar -xvf FileList.tar
pwd

cmsRun reco_test_RAW2DIGI_RECO.py $1 > reco_test.stdout

mkdir split_file
python split_file.py
#tar -cvf split_file.tar split_file
ls

python add_gen_energy.py

python compare_gen_reco.py

#xrdcp split_file.tar root://cmseos.fnal.gov//${2}/split_file_${3}.tar
xrdcp reco_test.stdout root://cmseos.fnal.gov//${2}/reco_test_${3}.stdout
xrdcp result.csv root://cmseos.fnal.gov//${2}/result_${3}.csv
xrdcp result_no_PU_energy.root root://cmseos.fnal.gov//${2}/result_no_PU_energy_${3}.root
