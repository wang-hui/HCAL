# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco_test --filein file:root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18DR/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM-DIGI-RAW/PUAvg50IdealConditions_IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/4978A440-0E12-8541-86B8-81086CDC98A0.root --fileout file:reco_test.root --mc --eventcontent FEVTSIM --conditions 102X_upgrade2018_design_v9 --step RAW2DIGI,RECO --nThreads 8 --geometry DB:Extended --era Run2_2018
import FWCore.ParameterSet.Config as cms
import sys
from Configuration.StandardSequences.Eras import eras

process = cms.Process('RECO',eras.Run2_2018)

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
	#'file:/eos/uscms/store/user/lpcrutgers/sve6/2018_1TeV_pion_gun_RAW_0PU-2020-07-25/Run3_RelVal_1TeV_pion_gun_RAW_0.root'
	my_list
	),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco_test nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.FEVTSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:reco_test.root'),
    outputCommands = process.FEVTSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition
process.TFileService = cms.Service("TFileService", fileName = cms.string("gen_hist.root") )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_design_v9', '')

# Path and EndPath definitions
process.myAna = cms.EDAnalyzer(
    "HCALTestAna",
    do_PU = cms.untracked.bool(True),
    is_run3_relVal = cms.untracked.bool(False),
    min_simHit_energy = cms.untracked.double(0.0))

process.raw2digi_step = cms.Path(process.myAna + process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTSIMoutput_step = cms.EndPath(process.FEVTSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.FEVTSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(1)
process.options.numberOfStreams=cms.untracked.uint32(0)


# Customisation from command line

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
