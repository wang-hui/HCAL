#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_10_2_12_patch1`
cd CMSSW_10_2_12_patch1/src
eval `scramv1 runtime -sh`

cd ${_CONDOR_SCRATCH_DIR}
xrdcp $1 result.csv

python compare_gen_reco.py

xrdcp result_origin.root root://cmseos.fnal.gov//${2}/result_${3}.root
