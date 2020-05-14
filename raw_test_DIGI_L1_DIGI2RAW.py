# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: raw_test --filein file:root://eospublic.cern.ch//eos/opendata/cms/MonteCarloUpgrade/RunIIFall18wmLHEGS/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM/IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/4FF18384-7B96-9E4A-B611-AA5D176A4156.root --fileout file:raw_test.root --mc --eventcontent FEVTDEBUGHLT --datatier GEN-SIM-DIGI-RAW --conditions 102X_upgrade2018_design_v9 --step DIGI:pdigi_valid,L1,DIGI2RAW --nThreads 8 --geometry DB:Extended --era Run2_2018 -n 8
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('DIGI2RAW',eras.Run2_2018)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    fileNames = cms.untracked.vstring(
	#'file:root://eospublic.cern.ch//eos/opendata/cms/MonteCarloUpgrade/RunIIFall18wmLHEGS/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM/IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/4FF18384-7B96-9E4A-B611-AA5D176A4156.root'
	#"file:root://eospublic.cern.ch//eos/opendata/cms/MonteCarloUpgrade/RunIIFall18wmLHEGS/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM/IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/59C152F9-B97E-2B4F-A931-E68D589679D4.root"
	#"file:root://eospublic.cern.ch//eos/opendata/cms/MonteCarloUpgrade/RunIIFall18wmLHEGS/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM/IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/61361C3A-A440-8D4F-B6E8-FE7CBD7343BB.root"
	"file:root://eospublic.cern.ch//eos/opendata/cms/MonteCarloUpgrade/RunIIFall18wmLHEGS/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/GEN-SIM/IdealConditions_102X_upgrade2018_design_v9_ext1-v2/260000/ECC2273E-E7E1-5E46-89E6-126B42FE692D.root"
	),
    inputCommands = cms.untracked.vstring(
        'keep *', 
        'drop *_genParticles_*_*', 
        'drop *_genParticlesForJets_*_*', 
        'drop *_kt4GenJets_*_*', 
        'drop *_kt6GenJets_*_*', 
        'drop *_iterativeCone5GenJets_*_*', 
        'drop *_ak4GenJets_*_*', 
        'drop *_ak7GenJets_*_*', 
        'drop *_ak8GenJets_*_*', 
        'drop *_ak4GenJetsNoNu_*_*', 
        'drop *_ak8GenJetsNoNu_*_*', 
        'drop *_genCandidatesForMET_*_*', 
        'drop *_genParticlesForMETAllVisible_*_*', 
        'drop *_genMetCalo_*_*', 
        'drop *_genMetCaloAndNonPrompt_*_*', 
        'drop *_genMetTrue_*_*', 
        'drop *_genMetIC5GenJs_*_*'
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('raw_test nevts:8'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-DIGI-RAW'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:/eos/uscms/store/user/huiwang/HCAL/opendata_2018_TTbar_raw_noPU/opendata_2018_TTbar_raw_noPU_4.root'),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.digitizers = cms.PSet(process.theDigitizersValid)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '102X_upgrade2018_design_v9', '')

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi_valid)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.endjob_step,process.FEVTDEBUGHLToutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(8)
process.options.numberOfStreams=cms.untracked.uint32(0)


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
