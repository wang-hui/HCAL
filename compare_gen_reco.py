import ROOT as rt
import pandas as pd
import sys
import math
import numpy as np

result_dir = "results_temp/"
result_file = "result"

tot_rows = None
#tot_rows = 100000

run_mod = "origin"
#run_mod = "slope1"

reco_slope = 1.0
aux_slope = 1.0
raw_slope = 1.0
use_8_pulse_bit = 1<<29

study_gain = False
add_weight = False

Ebins = 400
Emin = 0.0
Emax = 1000.0

reco_test = pd.read_csv(result_dir + result_file + ".csv", nrows=10, sep=',', skipinitialspace = True, header=0)
reco64_cols = [c for c in reco_test if reco_test[c].dtype == "float64"]
reco32_cols = {c: np.float32 for c in reco64_cols}

result = pd.read_csv(result_dir + result_file + ".csv", engine='c', dtype=reco32_cols, sep=',', skipinitialspace = True, header=0, nrows=tot_rows)

if add_weight:
    result["weight"] = 1.0
    HE_depth1_weight_file = rt.TFile.Open("HE_depth1_weight.root")
    HE_depth1_weight_hist = HE_depth1_weight_file.Get("HE_depth1_weight")

out_file = rt.TFile(result_dir + result_file + "_" + run_mod + ".root","RECREATE")

def chi2loss(y_true, y_pred):
    loss = math.pow((y_pred - y_true)/(math.sqrt(1+(y_true/32))), 2)
    return loss

#==============global hist =======================
weighted_time_h = rt.TH1F("weighted_time_h", "weighted simHit time", 100, 0.0, 500.0)
TS45_time_h = rt.TH1F("TS45_time_h", "TS45 time", 100, 75.0, 100.0)
arrival_time_h = rt.TH1F("arrival_time_h", "arrival time", 100, 75.0, 100.0)
weighted_time_vs_gen_h = rt.TH2F("weighted_time_vs_gen_h", "weighted time vs gen", Ebins, Emin, Emax, 100, 0.0, 500.0)
TS45_time_vs_gen_h = rt.TH2F("TS45_time_vs_gen_h", "TS45 time vs gen", Ebins, Emin, Emax, 100, 75.0, 100.0)
arrival_time_vs_gen_h = rt.TH2F("arrival_time_vs_gen_h", "arrival time vs gen", Ebins, Emin, Emax, 100, 75.0, 100.0)
charge_vs_TS_h = rt.TH2F("charge_vs_TS_h", "charge_vs_TS_h", 8, 0, 8, 100, 0, 1)
abnormal_charge_vs_TS_h = rt.TH2F("abnormal_charge_vs_TS_h", "abnormal_charge_vs_TS_h", 8, 0, 8, 100, 0, 1)
median_time_h = rt.TH1F("median_time_h", "median simHit time", 100, 0.0, 500.0)
fcByPE_h = rt.TH1F("fcByPE_h", "fcByPE for each TS", 100, 0.0, 1000.0)
PU_h = rt.TH1F("PU_h", "pileup", 100, 0.0, 100.0)
reco_h = rt.TH1F("reco_h", "reco energy", Ebins, Emin, Emax)
gen_h = rt.TH1F("gen_h", "truth energy", Ebins, Emin, Emax)
genG0_h = rt.TH1F("genG0_h", "truth energy > 0 GeV", 400, 0, 100)
use_8_pulse_h = rt.TH1F("use_8_pulse_h", "use 8 pulses", 2, 0, 2)

#==============reco vs gen 2d hist ==================
reco_vs_gen_h = rt.TH2F("reco_vs_gen_h", "reco vs gen", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_err_vs_gen_h = rt.TH2F("reco_err_vs_gen_h", "|reco - gen|/gen vs gen", Ebins, Emin, Emax, 100, 0, 1)
reco_vs_gen_depthG1_h = rt.TH2F("reco_vs_gen_depthG1_h", "reco vs gen, depth > 1", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_HB_h = rt.TH2F("reco_vs_gen_HB_h", "reco vs gen, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_HB_PUL_h = rt.TH2F("reco_vs_gen_HB_PUL_h", "reco_vs_gen_HB_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_HB_PUM_h = rt.TH2F("reco_vs_gen_HB_PUM_h", "reco_vs_gen_HB_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_HB_PUH_h = rt.TH2F("reco_vs_gen_HB_PUH_h", "reco_vs_gen_HB_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HB_h = rt.TH2F("reco_vs_gen_depthG1_HB_h", "reco vs gen, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HB_1_pulse_h = rt.TH2F("reco_vs_gen_depthG1_HB_1_pulse_h", "reco vs gen, depth > 1, HB, 1 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HB_8_pulse_h = rt.TH2F("reco_vs_gen_depthG1_HB_8_pulse_h", "reco vs gen, depth > 1, HB, 8 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_h = rt.TH2F("reco_vs_gen_depthG1_HE_h", "reco vs gen, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_PUL_h = rt.TH2F("reco_vs_gen_depthG1_HE_PUL_h", "reco_vs_gen_depthG1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_PUM_h = rt.TH2F("reco_vs_gen_depthG1_HE_PUM_h", "reco_vs_gen_depthG1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_PUH_h = rt.TH2F("reco_vs_gen_depthG1_HE_PUH_h", "reco_vs_gen_depthG1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_1_pulse_h = rt.TH2F("reco_vs_gen_depthG1_HE_1_pulse_h", "reco vs gen, depth > 1, HE, 1 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_8_pulse_h = rt.TH2F("reco_vs_gen_depthG1_HE_8_pulse_h", "reco vs gen, depth > 1, HE, 8 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HB_h = rt.TH2F("reco_vs_gen_depthE1_HB_h", "reco vs gen, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HB_1_pulse_h = rt.TH2F("reco_vs_gen_depthE1_HB_1_pulse_h", "reco vs gen, depth = 1, HB, 1 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HB_8_pulse_h = rt.TH2F("reco_vs_gen_depthE1_HB_8_pulse_h", "reco vs gen, depth = 1, HB, 8 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_h = rt.TH2F("reco_vs_gen_depthE1_HE_h", "reco vs gen, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_PUL_h = rt.TH2F("reco_vs_gen_depthE1_HE_PUL_h", "reco_vs_gen_depthE1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_PUM_h = rt.TH2F("reco_vs_gen_depthE1_HE_PUM_h", "reco_vs_gen_depthE1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_PUH_h = rt.TH2F("reco_vs_gen_depthE1_HE_PUH_h", "reco_vs_gen_depthE1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_1_pulse_h = rt.TH2F("reco_vs_gen_depthE1_HE_1_pulse_h", "reco vs gen, depth = 1, HE, 1 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_8_pulse_h = rt.TH2F("reco_vs_gen_depthE1_HE_8_pulse_h", "reco vs gen, depth = 1, HE, 8 pulse", Ebins, Emin, Emax, Ebins, Emin, Emax)

#==============aux vs gen 2d hist ==================
aux_vs_gen_h = rt.TH2F("aux_vs_gen_h", "aux vs gen", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_err_vs_gen_h = rt.TH2F("aux_err_vs_gen_h", "|aux - gen|/gen vs gen", Ebins, Emin, Emax, 100, 0, 1)
aux_vs_gen_depthG1_h = rt.TH2F("aux_vs_gen_depthG1_h", "aux vs gen, depth > 1", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_HB_h = rt.TH2F("aux_vs_gen_HB_h", "aux vs gen, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_HB_PUL_h = rt.TH2F("aux_vs_gen_HB_PUL_h", "aux_vs_gen_HB_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_HB_PUM_h = rt.TH2F("aux_vs_gen_HB_PUM_h", "aux_vs_gen_HB_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_HB_PUH_h = rt.TH2F("aux_vs_gen_HB_PUH_h", "aux_vs_gen_HB_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthG1_HB_h = rt.TH2F("aux_vs_gen_depthG1_HB_h", "aux vs gen, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthG1_HE_h = rt.TH2F("aux_vs_gen_depthG1_HE_h", "aux vs gen, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthG1_HE_PUL_h = rt.TH2F("aux_vs_gen_depthG1_HE_PUL_h", "aux_vs_gen_depthG1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthG1_HE_PUM_h = rt.TH2F("aux_vs_gen_depthG1_HE_PUM_h", "aux_vs_gen_depthG1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthG1_HE_PUH_h = rt.TH2F("aux_vs_gen_depthG1_HE_PUH_h", "aux_vs_gen_depthG1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthE1_HB_h = rt.TH2F("aux_vs_gen_depthE1_HB_h", "aux vs gen, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthE1_HE_h = rt.TH2F("aux_vs_gen_depthE1_HE_h", "aux vs gen, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthE1_HE_PUL_h = rt.TH2F("aux_vs_gen_depthE1_HE_PUL_h", "aux_vs_gen_depthE1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthE1_HE_PUM_h = rt.TH2F("aux_vs_gen_depthE1_HE_PUM_h", "aux_vs_gen_depthE1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
aux_vs_gen_depthE1_HE_PUH_h = rt.TH2F("aux_vs_gen_depthE1_HE_PUH_h", "aux_vs_gen_depthE1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)

#==============raw vs gen 2d hist ==================
raw_vs_gen_HB_h = rt.TH2F("raw_vs_gen_HB_h", "raw vs gen, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_HB_PUL_h = rt.TH2F("raw_vs_gen_HB_PUL_h", "raw_vs_gen_HB_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_HB_PUM_h = rt.TH2F("raw_vs_gen_HB_PUM_h", "raw_vs_gen_HB_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_HB_PUH_h = rt.TH2F("raw_vs_gen_HB_PUH_h", "raw_vs_gen_HB_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthG1_HB_h = rt.TH2F("raw_vs_gen_depthG1_HB_h", "raw vs gen, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthG1_HE_h = rt.TH2F("raw_vs_gen_depthG1_HE_h", "raw vs gen, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthG1_HE_PUL_h = rt.TH2F("raw_vs_gen_depthG1_HE_PUL_h", "raw_vs_gen_depthG1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthG1_HE_PUM_h = rt.TH2F("raw_vs_gen_depthG1_HE_PUM_h", "raw_vs_gen_depthG1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthG1_HE_PUH_h = rt.TH2F("raw_vs_gen_depthG1_HE_PUH_h", "raw_vs_gen_depthG1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthE1_HB_h = rt.TH2F("raw_vs_gen_depthE1_HB_h", "raw vs gen, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthE1_HE_h = rt.TH2F("raw_vs_gen_depthE1_HE_h", "raw vs gen, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthE1_HE_PUL_h = rt.TH2F("raw_vs_gen_depthE1_HE_PUL_h", "raw_vs_gen_depthE1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthE1_HE_PUM_h = rt.TH2F("raw_vs_gen_depthE1_HE_PUM_h", "raw_vs_gen_depthE1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
raw_vs_gen_depthE1_HE_PUH_h = rt.TH2F("raw_vs_gen_depthE1_HE_PUH_h", "raw_vs_gen_depthE1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)

#==============DLPHIN vs gen 2d hist ==================
DLPHIN_vs_gen_h = rt.TH2F("DLPHIN_vs_gen_h", "DLPHIN vs gen", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_err_vs_gen_h = rt.TH2F("DLPHIN_err_vs_gen_h", "|DLPHIN - gen|/gen vs gen", Ebins, Emin, Emax, 100, 0, 1)
DLPHIN_vs_gen_depthG1_h = rt.TH2F("DLPHIN_vs_gen_depthG1_h", "DLPHIN vs gen, depth > 1", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_HB_h = rt.TH2F("DLPHIN_vs_gen_HB_h", "DLPHIN vs gen, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_HB_PUL_h = rt.TH2F("DLPHIN_vs_gen_HB_PUL_h", "DLPHIN_vs_gen_HB_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_HB_PUM_h = rt.TH2F("DLPHIN_vs_gen_HB_PUM_h", "DLPHIN_vs_gen_HB_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_HB_PUH_h = rt.TH2F("DLPHIN_vs_gen_HB_PUH_h", "DLPHIN_vs_gen_HB_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthG1_HB_h = rt.TH2F("DLPHIN_vs_gen_depthG1_HB_h", "DLPHIN vs gen, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthG1_HE_h = rt.TH2F("DLPHIN_vs_gen_depthG1_HE_h", "DLPHIN vs gen, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthG1_HE_PUL_h = rt.TH2F("DLPHIN_vs_gen_depthG1_HE_PUL_h", "DLPHIN_vs_gen_depthG1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthG1_HE_PUM_h = rt.TH2F("DLPHIN_vs_gen_depthG1_HE_PUM_h", "DLPHIN_vs_gen_depthG1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthG1_HE_PUH_h = rt.TH2F("DLPHIN_vs_gen_depthG1_HE_PUH_h", "DLPHIN_vs_gen_depthG1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthE1_HB_h = rt.TH2F("DLPHIN_vs_gen_depthE1_HB_h", "DLPHIN vs gen, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthE1_HE_h = rt.TH2F("DLPHIN_vs_gen_depthE1_HE_h", "DLPHIN vs gen, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthE1_HE_PUL_h = rt.TH2F("DLPHIN_vs_gen_depthE1_HE_PUL_h", "DLPHIN_vs_gen_depthE1_HE_PUL_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthE1_HE_PUM_h = rt.TH2F("DLPHIN_vs_gen_depthE1_HE_PUM_h", "DLPHIN_vs_gen_depthE1_HE_PUM_h", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_gen_depthE1_HE_PUH_h = rt.TH2F("DLPHIN_vs_gen_depthE1_HE_PUH_h", "DLPHIN_vs_gen_depthE1_HE_PUH_h", Ebins, Emin, Emax, Ebins, Emin, Emax)

#==============ratio 1d hist ==========================
sum_amp_over_gen_h = rt.TH1F("sum_amp_over_gen_h", "sum 8 amplitudes / gen for gen > 10 GeV", 100, 0.0, 2500.0)
inv_gain_h = rt.TH1F("inv_gain_h", "1/gain for gen > 10 GeV", 100, 0.0, 2500.0)

reco_ratio_h = rt.TH1F("reco_ratio_h", "reco_ratio_h", 100, 0.0, 2.0)
reco_ratio_HB_h = rt.TH1F("reco_ratio_HB_h", "reco_ratio_HB_h", 100, 0.0, 2.0)
reco_ratio_HB_genL_h = rt.TH1F("reco_ratio_HB_genL_h", "reco_ratio_HB_genL_h", 100, 0.0, 2.0)
reco_ratio_HB_genM_h = rt.TH1F("reco_ratio_HB_genM_h", "reco_ratio_HB_genM_h", 100, 0.0, 2.0)
reco_ratio_HB_genH_h = rt.TH1F("reco_ratio_HB_genH_h", "reco_ratio_HB_genH_h", 100, 0.0, 2.0)
reco_ratio_depthG1_h = rt.TH1F("reco_ratio_depthG1_h", "reco_ratio_depthG1_h", 100, 0.0, 2.0)
reco_ratio_depthG1_HB_h = rt.TH1F("reco_ratio_depthG1_HB_h", "reco_ratio_depthG1_HB_h", 100, 0.0, 2.0)
reco_ratio_depthG1_HE_h = rt.TH1F("reco_ratio_depthG1_HE_h", "reco_ratio_depthG1_HE_h", 100, 0.0, 2.0)
reco_ratio_depthG1_HE_genL_h = rt.TH1F("reco_ratio_depthG1_HE_genL_h", "reco_ratio_depthG1_HE_genL_h", 100, 0.0, 2.0)
reco_ratio_depthG1_HE_genM_h = rt.TH1F("reco_ratio_depthG1_HE_genM_h", "reco_ratio_depthG1_HE_genM_h", 100, 0.0, 2.0)
reco_ratio_depthG1_HE_genH_h = rt.TH1F("reco_ratio_depthG1_HE_genH_h", "reco_ratio_depthG1_HE_genH_h", 100, 0.0, 2.0)
reco_ratio_depthE1_HB_h = rt.TH1F("reco_ratio_depthE1_HB_h", "reco_ratio_depthE1_HB_h", 100, 0.0, 2.0)
reco_ratio_depthE1_HE_h = rt.TH1F("reco_ratio_depthE1_HE_h", "reco_ratio_depthE1_HE_h", 100, 0.0, 2.0)
reco_ratio_depthE1_HE_genL_h = rt.TH1F("reco_ratio_depthE1_HE_genL_h", "reco_ratio_depthE1_HE_genL_h", 100, 0.0, 2.0)
reco_ratio_depthE1_HE_genM_h = rt.TH1F("reco_ratio_depthE1_HE_genM_h", "reco_ratio_depthE1_HE_genM_h", 100, 0.0, 2.0)
reco_ratio_depthE1_HE_genH_h = rt.TH1F("reco_ratio_depthE1_HE_genH_h", "reco_ratio_depthE1_HE_genH_h", 100, 0.0, 2.0)

aux_ratio_h = rt.TH1F("aux_ratio_h", "aux_ratio_h", 100, 0.0, 2.0)
aux_ratio_HB_h = rt.TH1F("aux_ratio_HB_h", "aux_ratio_HB_h", 100, 0.0, 2.0)
aux_ratio_HB_genL_h = rt.TH1F("aux_ratio_HB_genL_h", "aux_ratio_HB_genL_h", 100, 0.0, 2.0)
aux_ratio_HB_genM_h = rt.TH1F("aux_ratio_HB_genM_h", "aux_ratio_HB_genM_h", 100, 0.0, 2.0)
aux_ratio_HB_genH_h = rt.TH1F("aux_ratio_HB_genH_h", "aux_ratio_HB_genH_h", 100, 0.0, 2.0)
aux_ratio_depthG1_h = rt.TH1F("aux_ratio_depthG1_h", "aux_ratio_depthG1_h", 100, 0.0, 2.0)
aux_ratio_depthG1_HB_h = rt.TH1F("aux_ratio_depthG1_HB_h", "aux_ratio_depthG1_HB_h", 100, 0.0, 2.0)
aux_ratio_depthG1_HE_h = rt.TH1F("aux_ratio_depthG1_HE_h", "aux_ratio_depthG1_HE_h", 100, 0.0, 2.0)
aux_ratio_depthG1_HE_genL_h = rt.TH1F("aux_ratio_depthG1_HE_genL_h", "aux_ratio_depthG1_HE_genL_h", 100, 0.0, 2.0)
aux_ratio_depthG1_HE_genM_h = rt.TH1F("aux_ratio_depthG1_HE_genM_h", "aux_ratio_depthG1_HE_genM_h", 100, 0.0, 2.0)
aux_ratio_depthG1_HE_genH_h = rt.TH1F("aux_ratio_depthG1_HE_genH_h", "aux_ratio_depthG1_HE_genH_h", 100, 0.0, 2.0)
aux_ratio_depthE1_HB_h = rt.TH1F("aux_ratio_depthE1_HB_h", "aux_ratio_depthE1_HB_h", 100, 0.0, 2.0)
aux_ratio_depthE1_HE_h = rt.TH1F("aux_ratio_depthE1_HE_h", "aux_ratio_depthE1_HE_h", 100, 0.0, 2.0)
aux_ratio_depthE1_HE_genL_h = rt.TH1F("aux_ratio_depthE1_HE_genL_h", "aux_ratio_depthE1_HE_genL_h", 100, 0.0, 2.0)
aux_ratio_depthE1_HE_genM_h = rt.TH1F("aux_ratio_depthE1_HE_genM_h", "aux_ratio_depthE1_HE_genM_h", 100, 0.0, 2.0)
aux_ratio_depthE1_HE_genH_h = rt.TH1F("aux_ratio_depthE1_HE_genH_h", "aux_ratio_depthE1_HE_genH_h", 100, 0.0, 2.0)

raw_ratio_h = rt.TH1F("raw_ratio_h", "raw_ratio_h", 100, 0.0, 2.0)
raw_ratio_HB_h = rt.TH1F("raw_ratio_HB_h", "raw_ratio_HB_h", 100, 0.0, 2.0)
raw_ratio_HB_genL_h = rt.TH1F("raw_ratio_HB_genL_h", "raw_ratio_HB_genL_h", 100, 0.0, 2.0)
raw_ratio_HB_genM_h = rt.TH1F("raw_ratio_HB_genM_h", "raw_ratio_HB_genM_h", 100, 0.0, 2.0)
raw_ratio_HB_genH_h = rt.TH1F("raw_ratio_HB_genH_h", "raw_ratio_HB_genH_h", 100, 0.0, 2.0)
raw_ratio_depthG1_h = rt.TH1F("raw_ratio_depthG1_h", "raw_ratio_depthG1_h", 100, 0.0, 2.0)
raw_ratio_depthG1_HB_h = rt.TH1F("raw_ratio_depthG1_HB_h", "raw_ratio_depthG1_HB_h", 100, 0.0, 2.0)
raw_ratio_depthG1_HE_h = rt.TH1F("raw_ratio_depthG1_HE_h", "raw_ratio_depthG1_HE_h", 100, 0.0, 2.0)
raw_ratio_depthG1_HE_genL_h = rt.TH1F("raw_ratio_depthG1_HE_genL_h", "raw_ratio_depthG1_HE_genL_h", 100, 0.0, 2.0)
raw_ratio_depthG1_HE_genM_h = rt.TH1F("raw_ratio_depthG1_HE_genM_h", "raw_ratio_depthG1_HE_genM_h", 100, 0.0, 2.0)
raw_ratio_depthG1_HE_genH_h = rt.TH1F("raw_ratio_depthG1_HE_genH_h", "raw_ratio_depthG1_HE_genH_h", 100, 0.0, 2.0)
raw_ratio_depthE1_HB_h = rt.TH1F("raw_ratio_depthE1_HB_h", "raw_ratio_depthE1_HB_h", 100, 0.0, 2.0)
raw_ratio_depthE1_HE_h = rt.TH1F("raw_ratio_depthE1_HE_h", "raw_ratio_depthE1_HE_h", 100, 0.0, 2.0)
raw_ratio_depthE1_HE_genL_h = rt.TH1F("raw_ratio_depthE1_HE_genL_h", "raw_ratio_depthE1_HE_genL_h", 100, 0.0, 2.0)
raw_ratio_depthE1_HE_genM_h = rt.TH1F("raw_ratio_depthE1_HE_genM_h", "raw_ratio_depthE1_HE_genM_h", 100, 0.0, 2.0)
raw_ratio_depthE1_HE_genH_h = rt.TH1F("raw_ratio_depthE1_HE_genH_h", "raw_ratio_depthE1_HE_genH_h", 100, 0.0, 2.0)

DLPHIN_ratio_h = rt.TH1F("DLPHIN_ratio_h", "DLPHIN_ratio_h", 100, 0.0, 2.0)
DLPHIN_ratio_HB_h = rt.TH1F("DLPHIN_ratio_HB_h", "DLPHIN_ratio_HB_h", 100, 0.0, 2.0)
DLPHIN_ratio_HB_genL_h = rt.TH1F("DLPHIN_ratio_HB_genL_h", "DLPHIN_ratio_HB_genL_h", 100, 0.0, 2.0)
DLPHIN_ratio_HB_genM_h = rt.TH1F("DLPHIN_ratio_HB_genM_h", "DLPHIN_ratio_HB_genM_h", 100, 0.0, 2.0)
DLPHIN_ratio_HB_genH_h = rt.TH1F("DLPHIN_ratio_HB_genH_h", "DLPHIN_ratio_HB_genH_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_h = rt.TH1F("DLPHIN_ratio_depthG1_h", "DLPHIN_ratio_depthG1_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_HB_h = rt.TH1F("DLPHIN_ratio_depthG1_HB_h", "DLPHIN_ratio_depthG1_HB_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_HE_h = rt.TH1F("DLPHIN_ratio_depthG1_HE_h", "DLPHIN_ratio_depthG1_HE_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_HE_genL_h = rt.TH1F("DLPHIN_ratio_depthG1_HE_genL_h", "DLPHIN_ratio_depthG1_HE_genL_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_HE_genM_h = rt.TH1F("DLPHIN_ratio_depthG1_HE_genM_h", "DLPHIN_ratio_depthG1_HE_genM_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthG1_HE_genH_h = rt.TH1F("DLPHIN_ratio_depthG1_HE_genH_h", "DLPHIN_ratio_depthG1_HE_genH_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthE1_HB_h = rt.TH1F("DLPHIN_ratio_depthE1_HB_h", "DLPHIN_ratio_depthE1_HB_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthE1_HE_h = rt.TH1F("DLPHIN_ratio_depthE1_HE_h", "DLPHIN_ratio_depthE1_HE_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthE1_HE_genL_h = rt.TH1F("DLPHIN_ratio_depthE1_HE_genL_h", "DLPHIN_ratio_depthE1_HE_genL_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthE1_HE_genM_h = rt.TH1F("DLPHIN_ratio_depthE1_HE_genM_h", "DLPHIN_ratio_depthE1_HE_genM_h", 100, 0.0, 2.0)
DLPHIN_ratio_depthE1_HE_genH_h = rt.TH1F("DLPHIN_ratio_depthE1_HE_genH_h", "DLPHIN_ratio_depthE1_HE_genH_h", 100, 0.0, 2.0)

raw_loss_depthE1_HE_h = rt.TH1F("raw_loss_depthE1_HE_h", "raw_loss_depthE1_HE_h", 100, 0.0, 50.0)
DLPHIN_loss_depthE1_HE_h = rt.TH1F("DLPHIN_loss_depthE1_HE_h", "DLPHIN_loss_depthE1_HE_h", 100, 0.0, 50.0)
raw_loss_depthE1_HE_genL_h = rt.TH1F("raw_loss_depthE1_HE_genL_h", "raw_loss_depthE1_HE_genL_h", 100, 0.0, 2.0)
DLPHIN_loss_depthE1_HE_genL_h = rt.TH1F("DLPHIN_loss_depthE1_HE_genL_h", "DLPHIN_loss_depthE1_HE_genL_h", 100, 0.0, 2.0)
#============loop ieta hist for HB ====================
weighted_time_HB_ieta_list = []
TS45_time_HB_ieta_list = []
arrival_time_HB_ieta_list = []
reco_vs_gen_depthE1_HB_list = []
reco_vs_gen_depthG1_HB_list = []
for i in range(1,17):
    E1_hist = rt.TH2F("reco_vs_gen_depthE1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HB |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthE1_HB_list.append(E1_hist)
    G1_hist = rt.TH2F("reco_vs_gen_depthG1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HB |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthG1_HB_list.append(G1_hist)

    weighted_time_hist_list = []
    TS45_time_hist_list = []
    arrival_time_hist_list = []
    for j in range(1,6):
        weighted_time_hist = rt.TH1F("weighted_time_HB_iEta_" + str(i) + "_depth_" + str(j) + "_h", "weighted simHit time, HB |ieta| " + str(i) + ", depth " + str(j), 100, 0.0, 500.0)
        TS45_time_hist = rt.TH1F("TS45_time_HB_iEta_" + str(i) + "_depth_" + str(j) + "_h", "TS45 time, HB |ieta| " + str(i) + ", depth " + str(j), 100, 75.0, 100.0)
        arrival_time_hist = rt.TH1F("arrival_time_HB_iEta_" + str(i) + "_depth_" + str(j) + "_h", "arrival time, HB |ieta| " + str(i) + ", depth " + str(j), 100, 75.0, 100.0)
        weighted_time_hist_list.append(weighted_time_hist)
        TS45_time_hist_list.append(TS45_time_hist)
        arrival_time_hist_list.append(arrival_time_hist)
    weighted_time_HB_ieta_list.append(weighted_time_hist_list)
    TS45_time_HB_ieta_list.append(TS45_time_hist_list)
    arrival_time_HB_ieta_list.append(arrival_time_hist_list)

#============loop ieta hist for HE ====================
weighted_time_HE_ieta_list = []
TS45_time_HE_ieta_list = []
arrival_time_HE_ieta_list = []
reco_vs_gen_depthE1_HE_list = []
reco_vs_gen_depthG1_HE_list = []
for i in range(16,30):
    E1_hist = rt.TH2F("reco_vs_gen_depthE1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HE |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthE1_HE_list.append(E1_hist)
    G1_hist = rt.TH2F("reco_vs_gen_depthG1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HE |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthG1_HE_list.append(G1_hist)

    weighted_time_hist_list = []
    TS45_time_hist_list = []
    arrival_time_hist_list = []
    for j in range(1,9):
        weighted_time_hist = rt.TH1F("weighted_time_HE_iEta_" + str(i) + "_depth_" + str(j) + "_h", "weighted simHit time, HE |ieta| " + str(i) + ", depth " + str(j), 100, 0.0, 500.0)
        TS45_time_hist = rt.TH1F("TS45_time_HE_iEta_" + str(i) + "_depth_" + str(j) + "_h", "TS45 time, HE |ieta| " + str(i) + ", depth " + str(j), 100, 75.0, 100.0)
        arrival_time_hist = rt.TH1F("arrival_time_HE_iEta_" + str(i) + "_depth_" + str(j) + "_h", "arrival time, HE |ieta| " + str(i) + ", depth " + str(j), 100, 75.0, 100.0)
        weighted_time_hist_list.append(weighted_time_hist)
        TS45_time_hist_list.append(TS45_time_hist)
        arrival_time_hist_list.append(arrival_time_hist)
    weighted_time_HE_ieta_list.append(weighted_time_hist_list)
    TS45_time_HE_ieta_list.append(TS45_time_hist_list)
    arrival_time_HE_ieta_list.append(arrival_time_hist_list)

Nrows = result.shape[0]
print "total rows: ", Nrows
for i in range(Nrows):
    if i%100000 == 0: print "process %d rows" %i

    reco_corr = 1
    aux_corr = 1
    raw_corr = 1
    if run_mod == "slope1":
        reco_corr = reco_slope
        aux_corr = aux_slope
        raw_corr = raw_slope

    raw_gain = result["raw gain"] [i]
    gain = result["gain"] [i]
    respCorr = gain / raw_gain
    gen_energy = result["raw truth energy"][i]
    reco_energy = (result["mahi energy"][i] / respCorr) / reco_corr
    aux_energy = (result["aux energy"][i] / respCorr) / aux_corr
    raw_energy = (result["raw energy"][i] / respCorr) / raw_corr
    DLPHIN_energy = result["DLPHIN energy"][i]
    ieta = abs(result["ieta"] [i])
    depth = result["depth"] [i]
    sub_det = result["sub detector"] [i]
    PU = result["PU"] [i]
    flags = result["flags"] [i]
    use_8_pulse = ((flags & use_8_pulse_bit) == use_8_pulse_bit)

    if study_gain:
        sum_amplitude = 0

        for j in range(1,9):
            fcByPE_str = "TS" + str(j) + " fcByPE"
            fcByPE_h.Fill(result[fcByPE_str] [i])

            RC_str = "TS" + str(j) + " raw charge"
            ped_str = "TS" + str(j) + " ped noise"
            sum_amplitude += max((result[RC_str] [i] - result[ped_str] [i]), 0)
               
        if gen_energy > 10:
            sum_amp_over_gen_h.Fill(sum_amplitude / gen_energy)
            inv_gain_h.Fill(1.0 / gain)

    PU_h.Fill(PU)
    reco_h.Fill(reco_energy)
    gen_h.Fill(gen_energy)
    use_8_pulse_h.Fill(use_8_pulse)
    reco_vs_gen_h.Fill(gen_energy, reco_energy)
    aux_vs_gen_h.Fill(gen_energy, aux_energy)
    DLPHIN_vs_gen_h.Fill(gen_energy, DLPHIN_energy)

    if sub_det == 1:
        reco_vs_gen_HB_h.Fill(gen_energy, reco_energy)
        aux_vs_gen_HB_h.Fill(gen_energy, aux_energy)
        raw_vs_gen_HB_h.Fill(gen_energy, raw_energy)
        DLPHIN_vs_gen_HB_h.Fill(gen_energy, DLPHIN_energy)
        if PU <= 23:
            aux_vs_gen_HB_PUL_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_HB_PUL_h.Fill(gen_energy, raw_energy)
            reco_vs_gen_HB_PUL_h.Fill(gen_energy, reco_energy)
            DLPHIN_vs_gen_HB_PUL_h.Fill(gen_energy, DLPHIN_energy)
        elif PU >= 31 and PU <= 35:
            aux_vs_gen_HB_PUM_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_HB_PUM_h.Fill(gen_energy, raw_energy)
            reco_vs_gen_HB_PUM_h.Fill(gen_energy, reco_energy)
            DLPHIN_vs_gen_HB_PUM_h.Fill(gen_energy, DLPHIN_energy)
        elif PU >= 43:
            aux_vs_gen_HB_PUH_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_HB_PUH_h.Fill(gen_energy, raw_energy)
            reco_vs_gen_HB_PUH_h.Fill(gen_energy, reco_energy)
            DLPHIN_vs_gen_HB_PUH_h.Fill(gen_energy, DLPHIN_energy)
        if depth == 1:
            reco_vs_gen_depthE1_HB_h.Fill(gen_energy, reco_energy)
            aux_vs_gen_depthE1_HB_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_depthE1_HB_h.Fill(gen_energy, raw_energy)
            DLPHIN_vs_gen_depthE1_HB_h.Fill(gen_energy, DLPHIN_energy)
            reco_vs_gen_depthE1_HB_list[ieta - 1].Fill(gen_energy, reco_energy)
            if use_8_pulse: reco_vs_gen_depthE1_HB_8_pulse_h.Fill(gen_energy, reco_energy)
            else: reco_vs_gen_depthE1_HB_1_pulse_h.Fill(gen_energy, reco_energy)
        else:
            reco_vs_gen_depthG1_h.Fill(gen_energy, reco_energy)
            reco_vs_gen_depthG1_HB_h.Fill(gen_energy, reco_energy)
            aux_vs_gen_depthG1_HB_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_depthG1_HB_h.Fill(gen_energy, raw_energy)
            DLPHIN_vs_gen_depthG1_HB_h.Fill(gen_energy, DLPHIN_energy)
            reco_vs_gen_depthG1_HB_list[ieta - 1].Fill(gen_energy, reco_energy)
            if use_8_pulse: reco_vs_gen_depthG1_HB_8_pulse_h.Fill(gen_energy, reco_energy)
            else: reco_vs_gen_depthG1_HB_1_pulse_h.Fill(gen_energy, reco_energy)
    elif sub_det == 2:
        if depth == 1:
            reco_vs_gen_depthE1_HE_h.Fill(gen_energy, reco_energy)
            aux_vs_gen_depthE1_HE_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_depthE1_HE_h.Fill(gen_energy, raw_energy)
            DLPHIN_vs_gen_depthE1_HE_h.Fill(gen_energy, DLPHIN_energy)
            if PU < 23:
                reco_vs_gen_depthE1_HE_PUL_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthE1_HE_PUL_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthE1_HE_PUL_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthE1_HE_PUL_h.Fill(gen_energy, DLPHIN_energy)
            elif PU >= 31 and PU <= 35:
                reco_vs_gen_depthE1_HE_PUM_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthE1_HE_PUM_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthE1_HE_PUM_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthE1_HE_PUM_h.Fill(gen_energy, DLPHIN_energy)
            elif PU >= 43:
                reco_vs_gen_depthE1_HE_PUH_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthE1_HE_PUH_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthE1_HE_PUH_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthE1_HE_PUH_h.Fill(gen_energy, DLPHIN_energy)
            reco_vs_gen_depthE1_HE_list[ieta - 16].Fill(gen_energy, reco_energy)
            if use_8_pulse: reco_vs_gen_depthE1_HE_8_pulse_h.Fill(gen_energy, reco_energy)
            else: reco_vs_gen_depthE1_HE_1_pulse_h.Fill(gen_energy, reco_energy)
        else:
            reco_vs_gen_depthG1_h.Fill(gen_energy, reco_energy)
            reco_vs_gen_depthG1_HE_h.Fill(gen_energy, reco_energy)
            aux_vs_gen_depthG1_HE_h.Fill(gen_energy, aux_energy)
            raw_vs_gen_depthG1_HE_h.Fill(gen_energy, raw_energy)
            DLPHIN_vs_gen_depthG1_HE_h.Fill(gen_energy, DLPHIN_energy)
            if PU < 23:
                reco_vs_gen_depthG1_HE_PUL_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthG1_HE_PUL_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthG1_HE_PUL_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthG1_HE_PUL_h.Fill(gen_energy, DLPHIN_energy)
            elif PU >= 31 and PU <= 35:
                reco_vs_gen_depthG1_HE_PUM_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthG1_HE_PUM_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthG1_HE_PUM_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthG1_HE_PUM_h.Fill(gen_energy, DLPHIN_energy)
            elif PU >= 43:
                reco_vs_gen_depthG1_HE_PUH_h.Fill(gen_energy, reco_energy)
                aux_vs_gen_depthG1_HE_PUH_h.Fill(gen_energy, aux_energy)
                raw_vs_gen_depthG1_HE_PUH_h.Fill(gen_energy, raw_energy)
                DLPHIN_vs_gen_depthG1_HE_PUH_h.Fill(gen_energy, DLPHIN_energy)
            reco_vs_gen_depthG1_HE_list[ieta - 16].Fill(gen_energy, reco_energy)
            if use_8_pulse: reco_vs_gen_depthG1_HE_8_pulse_h.Fill(gen_energy, reco_energy)
            else: reco_vs_gen_depthG1_HE_1_pulse_h.Fill(gen_energy, reco_energy)
    else: print "strange sub_det: ", sub_det

    if gen_energy > 0:
        median_time = result["median time"] [i]
        weighted_time = result["weighted time"] [i]
        arrival_time = result["arrival time"] [i]
        TS4_charge = max(result["TS4 raw charge"] [i] - result["TS4 ped noise"] [i], 0)
        TS5_charge = max(result["TS5 raw charge"] [i] - result["TS5 ped noise"] [i], 0)

        TS45_time = 0
        if TS4_charge + TS5_charge > 0:
            TS45_time = (TS4_charge * 75 + TS5_charge * 100) / (TS4_charge + TS5_charge)

        if gen_energy > 50:
            TS_list = []
            for TS in range(8):
                TS_string = str(TS + 1)
                TS_charge = max(result["TS" + TS_string + " raw charge"] [i] - result["TS" + TS_string + " ped noise"] [i], 0)
                TS_list.append(TS_charge)

            sum_TS = sum(TS_list)
            for TS in range(8):
                weight = TS_list[TS] / sum_TS
                charge_vs_TS_h.Fill(TS, weight)

            if reco_energy < 1:
                for TS in range(8):
                    weight = TS_list[TS] / sum_TS
                    abnormal_charge_vs_TS_h.Fill(TS, weight)

        genG0_h.Fill(gen_energy)
        weighted_time_h.Fill(weighted_time)
        TS45_time_h.Fill(TS45_time)
        arrival_time_h.Fill(arrival_time)
        weighted_time_vs_gen_h.Fill(gen_energy, weighted_time)
        TS45_time_vs_gen_h.Fill(gen_energy, TS45_time)
        arrival_time_vs_gen_h.Fill(gen_energy, arrival_time)
        median_time_h.Fill(median_time)

        reco_err_vs_gen_h.Fill(gen_energy, abs(reco_energy-gen_energy)/gen_energy)
        aux_err_vs_gen_h.Fill(gen_energy, abs(aux_energy-gen_energy)/gen_energy)
        DLPHIN_err_vs_gen_h.Fill(gen_energy, abs(DLPHIN_energy-gen_energy)/gen_energy)

        reco_ratio = reco_energy / gen_energy
        aux_ratio = aux_energy / gen_energy
        raw_ratio = raw_energy / gen_energy
        DLPHIN_ratio = DLPHIN_energy / gen_energy

        raw_loss = chi2loss(gen_energy, raw_energy)
        DLPHIN_loss = chi2loss(gen_energy, DLPHIN_energy)

        reco_ratio_h.Fill(reco_ratio)
        aux_ratio_h.Fill(aux_ratio)
        raw_ratio_h.Fill(raw_ratio)
        DLPHIN_ratio_h.Fill(DLPHIN_ratio)

        if sub_det == 1:
            weighted_time_HB_ieta_list[ieta - 1][depth - 1].Fill(weighted_time)
            TS45_time_HB_ieta_list[ieta - 1][depth - 1].Fill(TS45_time)
            arrival_time_HB_ieta_list[ieta - 1][depth - 1].Fill(arrival_time)
            reco_ratio_HB_h.Fill(reco_ratio)
            aux_ratio_HB_h.Fill(aux_ratio)
            raw_ratio_HB_h.Fill(raw_ratio)
            DLPHIN_ratio_HB_h.Fill(DLPHIN_ratio)
            if gen_energy > 7.5 and gen_energy < 10:
                reco_ratio_HB_genL_h.Fill(reco_ratio)
                aux_ratio_HB_genL_h.Fill(aux_ratio)
                raw_ratio_HB_genL_h.Fill(raw_ratio)
                DLPHIN_ratio_HB_genL_h.Fill(DLPHIN_ratio)
            elif gen_energy > 25 and gen_energy < 30:
                reco_ratio_HB_genM_h.Fill(reco_ratio)
                aux_ratio_HB_genM_h.Fill(aux_ratio)
                raw_ratio_HB_genM_h.Fill(raw_ratio)
                DLPHIN_ratio_HB_genM_h.Fill(DLPHIN_ratio)
            elif gen_energy > 90 and gen_energy < 110:
                reco_ratio_HB_genH_h.Fill(reco_ratio)
                aux_ratio_HB_genH_h.Fill(aux_ratio)
                raw_ratio_HB_genH_h.Fill(raw_ratio)
                DLPHIN_ratio_HB_genH_h.Fill(DLPHIN_ratio)
            if depth == 1:
                reco_ratio_depthE1_HB_h.Fill(reco_ratio)
                aux_ratio_depthE1_HB_h.Fill(aux_ratio)
                raw_ratio_depthE1_HB_h.Fill(raw_ratio)
                DLPHIN_ratio_depthE1_HB_h.Fill(DLPHIN_ratio)
            else:
                reco_ratio_depthG1_h.Fill(reco_ratio)
                reco_ratio_depthG1_HB_h.Fill(reco_ratio)
                aux_ratio_depthG1_HB_h.Fill(aux_ratio)
                raw_ratio_depthG1_HB_h.Fill(raw_ratio)
                DLPHIN_ratio_depthG1_HB_h.Fill(DLPHIN_ratio)
        else:
            weighted_time_HE_ieta_list[ieta - 16][depth - 1].Fill(weighted_time)
            TS45_time_HE_ieta_list[ieta - 16][depth - 1].Fill(TS45_time)
            arrival_time_HE_ieta_list[ieta - 16][depth - 1].Fill(arrival_time)
            if depth == 1:
                reco_ratio_depthE1_HE_h.Fill(reco_ratio)
                aux_ratio_depthE1_HE_h.Fill(aux_ratio)
                raw_ratio_depthE1_HE_h.Fill(raw_ratio)
                DLPHIN_ratio_depthE1_HE_h.Fill(DLPHIN_ratio)
                raw_loss_depthE1_HE_h.Fill(raw_loss)
                DLPHIN_loss_depthE1_HE_h.Fill(DLPHIN_loss)

                if add_weight and gen_energy < 100:
                    HE_depth1_weight = HE_depth1_weight_hist.GetBinContent(HE_depth1_weight_hist.FindBin(gen_energy))
                    result["weight"].iat[i] = HE_depth1_weight

                if gen_energy > 7.5 and gen_energy < 10:
                    reco_ratio_depthE1_HE_genL_h.Fill(reco_ratio)
                    aux_ratio_depthE1_HE_genL_h.Fill(aux_ratio)
                    raw_ratio_depthE1_HE_genL_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthE1_HE_genL_h.Fill(DLPHIN_ratio)

                    raw_loss_depthE1_HE_genL_h.Fill(raw_loss)
                    DLPHIN_loss_depthE1_HE_genL_h.Fill(DLPHIN_loss)
                elif gen_energy > 25 and gen_energy < 30:
                    reco_ratio_depthE1_HE_genM_h.Fill(reco_ratio)
                    aux_ratio_depthE1_HE_genM_h.Fill(aux_ratio)
                    raw_ratio_depthE1_HE_genM_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthE1_HE_genM_h.Fill(DLPHIN_ratio)
                elif gen_energy > 90 and gen_energy < 110:
                    reco_ratio_depthE1_HE_genH_h.Fill(reco_ratio)
                    aux_ratio_depthE1_HE_genH_h.Fill(aux_ratio)
                    raw_ratio_depthE1_HE_genH_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthE1_HE_genH_h.Fill(DLPHIN_ratio)
            else:
                reco_ratio_depthG1_h.Fill(reco_ratio)
                reco_ratio_depthG1_HE_h.Fill(reco_ratio)
                aux_ratio_depthG1_HE_h.Fill(aux_ratio)
                raw_ratio_depthG1_HE_h.Fill(raw_ratio)
                DLPHIN_ratio_depthG1_HE_h.Fill(DLPHIN_ratio)
                if gen_energy > 7.5 and gen_energy < 10:
                    reco_ratio_depthG1_HE_genL_h.Fill(reco_ratio)
                    aux_ratio_depthG1_HE_genL_h.Fill(aux_ratio)
                    raw_ratio_depthG1_HE_genL_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthG1_HE_genL_h.Fill(DLPHIN_ratio)
                elif gen_energy > 25 and gen_energy < 30:
                    reco_ratio_depthG1_HE_genM_h.Fill(reco_ratio)
                    aux_ratio_depthG1_HE_genM_h.Fill(aux_ratio)
                    raw_ratio_depthG1_HE_genM_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthG1_HE_genM_h.Fill(DLPHIN_ratio)
                elif gen_energy > 90 and gen_energy < 110:
                    reco_ratio_depthG1_HE_genH_h.Fill(reco_ratio)
                    aux_ratio_depthG1_HE_genH_h.Fill(aux_ratio)
                    raw_ratio_depthG1_HE_genH_h.Fill(raw_ratio)
                    DLPHIN_ratio_depthG1_HE_genH_h.Fill(DLPHIN_ratio)

out_file.cd()
out_file.Write()
out_file.Close()

if add_weight:
    result.to_csv("results_temp/result_with_weight.csv", index = False, header=True)
