# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco_MC --filein file:step2.root --mc --eventcontent RECOSIM --conditions 106X_upgrade2018_realistic_v11_L1v1 --step RAW2DIGI,RECO --nThreads 4 --geometry DB:Extended --era Run2_2018 -n 10 --no_exec
import FWCore.ParameterSet.Config as cms
import sys
from Configuration.Eras.Era_Run2_2018_cff import Run2_2018

process = cms.Process('RECO',Run2_2018)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
#process.MessageLogger.cerr.FwkReport.reportEvery = 10000

#process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
#process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

f = open(sys.argv[2], "r")
my_list = f.readlines()
f.close()

if len(sys.argv) == 4:
    nFile = int(sys.argv[3])
    my_list = my_list[0:nFile]

OutputFile = sys.argv[2].split("/")[-1]
OutputFile = OutputFile.split(".")[0]
OutputFile = OutputFile + "_TTree.root"

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        my_list
        #"root://cmseos.fnal.gov//store/user/lpcrutgers/huiwang/HCAL/UL_p1TeV_pion_gun_RAW_PU-2021-05-23/UL_MC_RAW_PU_0.root"
        #"root://cmseos.fnal.gov//store/user/lpcrutgers/huiwang/HCAL/UL_p1TeV_pion_gun_RAW_noPU-2021-06-11/UL_MC_RAW_noPU_0.root"
    ),
    secondaryFileNames = cms.untracked.vstring()
)
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco_MC nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
#
#process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
#    dataset = cms.untracked.PSet(
#        dataTier = cms.untracked.string(''),
#        filterName = cms.untracked.string('')
#    ),
#    fileName = cms.untracked.string('reco_MC_RAW2DIGI_RECO.root'),
#    outputCommands = process.RECOSIMEventContent.outputCommands,
#    splitLevel = cms.untracked.int32(0)
#)

# Additional output definition
process.TFileService = cms.Service("TFileService", fileName = cms.string(OutputFile) )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v11_L1v1', '')

#special tests for M2 and M3, should be commented out
#process.hbheprereco.algorithm.useM2 = cms.bool(True)
#process.hbheprereco.algorithm.useM3 = cms.bool(False)
#process.hbheprereco.algorithm.chiSqSwitch = cms.double(-1)
#process.hbheprereco.algorithm.activeBXs = cms.vint32(0)

process.hbheprereco.saveInfos = cms.bool(True)
process.hbheprereco.saveDLPHIN = cms.bool(False)        #turn off DLPHIN in HBHEPhase1Reconstructor 
process.hbheprereco.saveSimHit = cms.bool(False)        #and run it in the analyzer in debug mode

import RecoLocalCalo.HcalRecProducers.DLPHIN_cfi as DLPHIN
DLPHIN.DLPHINConfig.DLPHIN_debug = cms.bool(True)

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.hbheprereco)
process.DLPHIN_analyzer = cms.EDAnalyzer("DLPHIN_analyzer",
    HaveSimHits = cms.bool(True), DLPHINConfig = DLPHIN.DLPHINConfig)
process.DLPHIN_step = cms.Path(process.DLPHIN_analyzer)
#process.endjob_step = cms.EndPath(process.endOfProcess)
#process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
#process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RECOSIMoutput_step)
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.DLPHIN_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(4)
process.options.numberOfStreams=cms.untracked.uint32(0)
process.options.numberOfConcurrentLuminosityBlocks=cms.untracked.uint32(1)


# Customisation from command line

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
