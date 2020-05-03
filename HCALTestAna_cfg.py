import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process("Test", eras.Run2_2018)

# geometry
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

# global tag
globalTag = '102X_upgrade2018_design_v9'
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = globalTag

# source
process.source = cms.Source(
    "PoolSource",
    fileNames  = cms.untracked.vstring( "file:/eos/uscms/store/user/huiwang/HCAL/opendata/206A6E9A-4DB2-1941-A60B-7174FA398D86.root" ),
    )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.myAna = cms.EDAnalyzer("HCALTestAna", do_PU = cms.untracked.bool(True))

process.path = cms.Path(process.myAna)
