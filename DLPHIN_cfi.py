import FWCore.ParameterSet.Config as cms

import os
DLPHIN_pb_folder = "%s/src/HCAL/DLPHIN_pb/" % os.environ['CMSSW_BASE']

DLPHINConfig = cms.PSet(
    DLPHIN_apply_respCorr = cms.bool(True),
    DLPHIN_respCorr_name = cms.string(DLPHIN_pb_folder + "JP_respCorr_DLPHIN_zeroOut.root"),
    DLPHIN_truncate = cms.bool(True),
    DLPHIN_pb_2dHB = cms.string(DLPHIN_pb_folder + "model_2dHB_p1TeV_cut0_epoch100_weight0.pb"),
    DLPHIN_pb_2dHE = cms.string(DLPHIN_pb_folder + "model_2dHE_p1TeV_cut0p1_epoch100_weight_0to1.pb"),
)
