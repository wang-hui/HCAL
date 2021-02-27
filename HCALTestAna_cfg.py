import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process("Test", eras.Run2_2018)

# geometry
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

# source
process.source = cms.Source(
    "PoolSource",
    fileNames  = cms.untracked.vstring(
        "root://cmseos.fnal.gov//eos/uscms/store/user/lpcrutgers/aatkinso/hcal/UL_1TeV_pion_gun_RAW_PU-2020-12-20/UL_1TeV_pion_gun_RAW_PU_0.root" 
    ),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2)
)

# Additional output definition
process.TFileService = cms.Service("TFileService", fileName = cms.string("gen_hist.root") )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v11_L1v1', '')

# Path and EndPath definitions
process.myAna = cms.EDAnalyzer(
    "HCALTestAna",
    print_1d = cms.untracked.bool(True),
    print_2d = cms.untracked.bool(True),
    do_PU = cms.untracked.bool(True),
    is_run3_relVal = cms.untracked.bool(False),
    min_simHit_energy = cms.untracked.double(0.0))

process.path = cms.Path(process.myAna)
