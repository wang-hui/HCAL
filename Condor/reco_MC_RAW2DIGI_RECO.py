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
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

f = open(sys.argv[2], "r")
my_list = f.readlines()
f.close()

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        #'file:step2.root'
        #"root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/aatkinso/hcal/UL_1TeV_pion_gun_RAW_noPU-2020-12-20/UL_1TeV_pion_gun_RAW_0.root"
        my_list
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco_MC nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
outputCommands_slim = cms.untracked.vstring(
        "drop *",
        "keep HBHERecHits*_*_*_*",
        "keep CaloTowers*_*_*_*",
        "keep recoCaloMETs*_*_*_*",
        "keep recoPFMETs*_*_*_*",
        "keep recoMuons*_*_*_*",
        "keep recoGsfElectrons*_*_*_*",
        "keep recoCaloJets*_*_*_*",
        "keep recoPFJets*_*_*_*",
        "keep recoVertexs*_*_*_*",
        "keep recoGenMETs*_*_*_*",
        "keep recoGenJets*_*_*_*",
)

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('reco_MC_RAW2DIGI_RECO.root'),
    #outputCommands = process.RECOSIMEventContent.outputCommands,
    outputCommands = outputCommands_slim,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v11_L1v1', '')

import os
DLPHIN_pb_folder = "%s/src/HCAL/DLPHIN_pb/" % os.environ['CMSSW_BASE']

process.GlobalTag.toGet = cms.VPSet(
    cms.PSet(record = cms.string("PFCalibrationRcd"),
             #tag = cms.string("simHit_UL2018"),
             tag = cms.string("simHit_run3_E_2to500"),
             #connect = cms.string("sqlite_file:" + DLPHIN_pb_folder + "/PFCalibration_simHit.db")
             connect = cms.string("sqlite_file:PFCalibration_simHit.db")
             )
    )

process.ak4CaloJets.jetPtMin = cms.double(1.0)

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RECOSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(2)
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
