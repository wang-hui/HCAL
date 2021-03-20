#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_10_6_12`
cd CMSSW_10_6_12/src
eval `scramv1 runtime -sh`

cd ${_CONDOR_SCRATCH_DIR}
mkdir results_temp

xrdcp $1 results_temp/result.csv
python compare_gen_reco.py
xrdcp results_temp/result_origin.root root://cmseos.fnal.gov//${2}/result_${3}.root
#xrdcp results_temp/result_with_weight.csv root://cmseos.fnal.gov//${2}/result_with_weight_${3}.csv

#xrdcp $1 results_temp/result_data.csv
#python compare_DLPHIN_reco.py
#xrdcp results_temp/result_data_origin.root root://cmseos.fnal.gov//${2}/result_data_${3}.root
