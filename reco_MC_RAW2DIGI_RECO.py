# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: reco_MC --filein file:step2.root --mc --eventcontent RECOSIM --conditions 106X_upgrade2018_realistic_v11_L1v1 --step RAW2DIGI,RECO --nThreads 4 --geometry DB:Extended --era Run2_2018 -n 100 --no_exec
import FWCore.ParameterSet.Config as cms

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
    input = cms.untracked.int32(5)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        #"file:results_temp/step2_PU.root"
        #"root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/aatkinso/hcal/UL_1TeV_pion_gun_RAW_PU-2020-12-20/UL_1TeV_pion_gun_RAW_PU_0.root"
        #"root://cmseos.fnal.gov//store/user/lpcrutgers/aatkinso/hcal/UL_1TeV_pion_gun_RAW_noPU-2020-12-20/UL_1TeV_pion_gun_RAW_0.root"
        #"root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/huiwang/HCAL/RSGravitonToQuarkQuark_kMpl01_M_2000_RAW_PU-2021-05-02/UL_MC_RAW_PU_0.root"
        "root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/huiwang/HCAL/UL_p1TeV_pion_gun_RAW_PU-2021-05-23/UL_MC_RAW_PU_0.root"
        #"/store/mc/RunIISummer19UL18DIGIPremix/QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraphMLM-pythia8/GEN-SIM-DIGI/106X_upgrade2018_realistic_v11_L1v1-v2/260000/ED88DE9D-DF15-EF43-AFE2-37DBB0105D8A.root"
        #"root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/huiwang/HCAL/UL_DoublePion_E-50_RAW_noPU-2020-12-25/UL_1TeV_pion_gun_RAW_0_0.root"
        #"root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/huiwang/HCAL/UL_DoublePion_E-50_RAW_PU-2020-12-25/UL_1TeV_pion_gun_RAW_PU_0_0.root"
    ),
    #skipEvents = cms.untracked.uint32(620),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reco_MC nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
#RAW content
SimG4CoreRAW = cms.PSet(
    outputCommands = cms.untracked.vstring('keep *_g4SimHits_*_*',
        'keep edmHepMCProduct_source_*_*')
)
outputCommands_temp = process.RECOSIMEventContent.outputCommands
outputCommands_temp.extend(SimG4CoreRAW.outputCommands)

process.RECOSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('results_temp/RECO_MC.root'),
    outputCommands = outputCommands_temp,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition
process.TFileService = cms.Service("TFileService", fileName = cms.string("gen_hist.root") )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v11_L1v1', '')

# Path and EndPath definitions
process.myAna = cms.EDAnalyzer(
    "HCALTestAna",
    print_1d = cms.untracked.bool(False),
    print_2d_HB = cms.untracked.bool(True),
    print_2d_HE = cms.untracked.bool(False),
    do_PU = cms.untracked.bool(True),
    is_run3_relVal = cms.untracked.bool(False),
    min_simHit_energy = cms.untracked.double(0.0),
    max_simHit_time = cms.untracked.double(125.0))

process.raw2digi_step = cms.Path(process.myAna + process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOSIMoutput_step = cms.EndPath(process.RECOSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RECOSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(1)
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
