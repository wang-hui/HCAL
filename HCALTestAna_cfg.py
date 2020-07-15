import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_cff import Run3
process = cms.Process("Test", Run3)

# geometry
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

# global tag
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '') 

f = open("FileList/Run3_RelVal_1TeV_pion_gun_RAW_0PU.list", "r")
my_list = f.readlines()
f.close()

# source
process.source = cms.Source(
    "PoolSource",
    fileNames  = cms.untracked.vstring(
	#'file:/eos/uscms/store/user/huiwang/HCAL/MH-125_MFF-50_CTau-10000mm_step2.root' 
	my_list
	),
    )

process.TFileService = cms.Service("TFileService", fileName = cms.string("gen_hist.root") )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.myAna = cms.EDAnalyzer(
    "HCALTestAna",
    do_PU = cms.untracked.bool(True),
    is_run3_relVal = cms.untracked.bool(True),
    min_simHit_energy = cms.untracked.double(0.0))

process.path = cms.Path(process.myAna)
