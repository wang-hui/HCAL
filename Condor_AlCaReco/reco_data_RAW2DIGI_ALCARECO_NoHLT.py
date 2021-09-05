# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco_data -s RAW2DIGI,RECO --nThreads 4 --data --era Run2_2018,pf_badHcalMitigation --scenario pp --conditions 106X_dataRun2_v28 --eventcontent RECO --filein file:root://cmseos.fnal.gov//store/user/huiwang/HCAL/DoubleMuon_Run2018A-v1_RAW_6601F23E-0E65-E811-859E-FA163E98BEC0.root -n 100 --no_exec
import FWCore.ParameterSet.Config as cms
import sys
from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
from Configuration.Eras.Modifier_pf_badHcalMitigation_cff import pf_badHcalMitigation

process = cms.Process('RECO',Run2_2018,pf_badHcalMitigation)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.AlCaRecoStreams_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

f = open(sys.argv[2], "r")
my_list = f.readlines()
f.close()

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        #"/store/data/Run2018B/DoubleMuon/RAW/v1/000/317/392/00000/FEC3C392-CC67-E811-A4CF-FA163E228F53.root"
        #"/store/data/Run2018B/EGamma/RAW/v1/000/317/392/00000/005E507E-C267-E811-A1B6-FA163E47C8BD.root"
        my_list
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco_data nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
process.ALCARECOStreamHcalCalIsoTrkFilter = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOHcalCalIsoTrkFilterNoHLT')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('ALCARECOHcalCalIsoTrkFilterNoHLT')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    #outputCommands = process.OutALCARECOHcalCalIsoTrkFilter.outputCommands,
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_gtStage2Digis_*_*',
        'keep *_hbheprereco_*_*',
        'keep *_hbhereco_*_*',
        'keep *_ecalRecHit_*_*',
        'keep *_towerMaker_*_*',
        'keep *_offlineBeamSpot_*_*',
        #'keep *_hltTriggerSummaryAOD_*_*',
        #'keep *_TriggerResults_*_*',
        'keep *_generalTracks_*_*',
        'keep *_generalTracksExtra_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_TkAlIsoProdFilter_*_*',
    ),
    fileName = cms.untracked.string('ALCARECO_NoHLT_data.root'),
)

#cross-check with with HLT
process.alcaIsoTracksFilter.EBHitEnergyThreshold = 0.08
process.alcaIsoTracksFilter.EEHitEnergyThreshold0 = 0.3
process.alcaIsoTracksFilter.EEHitEnergyThreshold1 = 0
process.alcaIsoTracksFilter.EEHitEnergyThreshold2 = 0
process.alcaIsoTracksFilter.EEHitEnergyThreshold3 = 0
process.alcaIsoTracksFilter.EEHitEnergyThresholdLow = 0.3
process.alcaIsoTracksFilter.EEHitEnergyThresholdHigh = 0.3
process.alcaIsoTracksFilter.momentumRangeLow = 10
process.alcaIsoTracksFilter.preScaleFactor = 1
process.alcaIsoTracksFilter.preScaleHigh = 1

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v28', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.ALCARECOStreamHcalCalIsoTrkFilterOutPath = cms.EndPath(process.ALCARECOStreamHcalCalIsoTrkFilter)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.pathALCARECOHcalCalIsoTrkFilterNoHLT,process.endjob_step,process.ALCARECOStreamHcalCalIsoTrkFilterOutPath)

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
