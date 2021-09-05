#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH
eval `scramv1 project CMSSW CMSSW_10_6_12`
cd CMSSW_10_6_12/src
eval `scramv1 runtime -sh`
git cms-init --upstream-only
git cms-addpkg RecoLocalCalo/HcalRecProducers
git cms-addpkg RecoLocalCalo/HcalRecAlgos
git cms-addpkg Configuration/StandardSequences
git cms-addpkg Calibration/HcalAlCaRecoProducers

git clone -b CMSSW_10_6_x https://github.com/wang-hui/HCAL.git
mv HCAL/HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src
mv HCAL/BuildFile.xml RecoLocalCalo/HcalRecProducers
#mv HCAL/SimpleHBHEPhase1Algo.cc RecoLocalCalo/HcalRecAlgos/src
mv ${_CONDOR_SCRATCH_DIR}/SimpleHBHEPhase1Algo.cc RecoLocalCalo/HcalRecAlgos/src
mv HCAL/parseHBHEPhase1AlgoDescription.cc RecoLocalCalo/HcalRecAlgos/src
mv HCAL/AlCaRecoStreams_cff.py Configuration/StandardSequences/python
mv HCAL/ALCARECOHcalCalIsoTrkFilterNoHLT_cff.py  Calibration/HcalAlCaRecoProducers/python
mv ${_CONDOR_SCRATCH_DIR}/HBHEPhase1Reconstructor_cfi.py RecoLocalCalo/HcalRecProducers/python
scram b -j 4
cd HCAL
mv ${_CONDOR_SCRATCH_DIR}/reco_MC_RAW2DIGI_RECO_ALCARECO.py .
mv ${_CONDOR_SCRATCH_DIR}/reco_data_RAW2DIGI_ALCARECO_NoHLT.py .
mv ${_CONDOR_SCRATCH_DIR}/FileList.tar .
tar -xvf FileList.tar
pwd

#cmsRun reco_MC_RAW2DIGI_RECO_ALCARECO.py $1
cmsRun reco_data_RAW2DIGI_ALCARECO_NoHLT.py $1

#xrdcp ALCARECO_MC.root root://cmseos.fnal.gov//${2}/ALCARECO_MC_${3}.root
xrdcp ALCARECO_NoHLT_data.root root://cmseos.fnal.gov//${2}/ALCARECO_NoHLT_data_${3}.root
