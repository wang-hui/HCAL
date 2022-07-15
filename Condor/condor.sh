#!/bin/bash

echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc7_amd64_gcc700
echo $SCRAM_ARCH

eval `scramv1 project CMSSW CMSSW_10_6_12`
xrdcp root://cmseos.fnal.gov//store/user/hatake/Tarballs/CMSSW_10_6_12_DLPHIN_partial.tgz .
tar xzf CMSSW_10_6_12_DLPHIN_partial.tgz
cd CMSSW_10_6_12/src
eval `scramv1 runtime -sh`
#git cms-init --upstream-only
#git cms-addpkg RecoLocalCalo/HcalRecProducers
#git cms-addpkg RecoLocalCalo/HcalRecAlgos
#git clone https://github.com/wang-hui/HCAL.git
#cd HCAL
#mv ${_CONDOR_SCRATCH_DIR}/DLPHIN_cfi.py DLPHIN_code
#mv ${_CONDOR_SCRATCH_DIR}/HBHEPhase1Reconstructor_cfi.py DLPHIN_code
#python DLPHIN_setup.py mv
#cd ..
#scram b -j 4
mv ${_CONDOR_SCRATCH_DIR}/reco_MC_RAW2DIGI_RECO.py .
mv ${_CONDOR_SCRATCH_DIR}/FileList.tar .
tar -xvf FileList.tar
pwd

#cmsRun reco_MC_RAW2DIGI_RECO.py $1
cmsRun reco_MC_RAW2DIGI_L1Reco_RECO_RECOSIM_EI_PAT.py $1

#xrdcp -f reco_MC_RAW2DIGI_RECO.root root://cmseos.fnal.gov//${2}/MC_RECO_${3}.root
xrdcp -f reco_MC_inMINIAODSIM.root root://cmseos.fnal.gov//${2}/MINIAODSIM_${3}.root
