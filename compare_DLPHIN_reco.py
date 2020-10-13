import ROOT as rt
import pandas as pd
import sys

result_dir = "results_temp/"
result_file = "result_data"
#print sys.argv
#index = sys.argv[1]
#result_file += index

result = pd.read_csv(result_dir + result_file + ".csv", sep=',', skipinitialspace = True, header=0)

run_mod = "origin"
#run_mod = "slope1"

reco_slope = 1.0
use_8_pulse_bit = 1<<29

out_file = rt.TFile(result_dir + result_file + "_" + run_mod + ".root","RECREATE")

Ebins = 200
Emin = 0.0
Emax = 100.0

#==============global hist =======================
reco_h = rt.TH1F("reco_h", "reco energy", Ebins, Emin, Emax)
DLPHIN_h = rt.TH1F("DLPHIN_h", "DLPHIN energy", Ebins, Emin, Emax)
use_8_pulse_h = rt.TH1F("use_8_pulse_h", "use 8 pulses", 2, 0, 2)
respCorr_depthG1_HB_h = rt.TH1F("respCorr_depthG1_HB_h", "response correction, depth > 1, HB", 100, 0, 4)
respCorr_depthG1_HE_h = rt.TH1F("respCorr_depthG1_HE_h", "response correction, depth > 1, HE", 100, 0, 4)
respCorr_depthE1_HB_h = rt.TH1F("respCorr_depthE1_HB_h", "response correction, depth = 1, HB", 100, 0, 4)
respCorr_depthE1_HE_h = rt.TH1F("respCorr_depthE1_HE_h", "response correction, depth = 1, HE", 100, 0, 4)

#==============DLPHIN vs reco 2d hist ==================
DLPHIN_vs_reco_h = rt.TH2F("DLPHIN_vs_reco_h", "DLPHIN vs reco", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_err_vs_reco_h = rt.TH2F("DLPHIN_err_vs_reco_h", "|DLPHIN - reco|/reco vs reco", Ebins, Emin, Emax, 100, 0, 1)
DLPHIN_vs_reco_depthG1_HB_h = rt.TH2F("DLPHIN_vs_reco_depthG1_HB_h", "DLPHIN vs reco, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_reco_depthG1_HE_h = rt.TH2F("DLPHIN_vs_reco_depthG1_HE_h", "DLPHIN vs reco, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_reco_depthE1_HB_h = rt.TH2F("DLPHIN_vs_reco_depthE1_HB_h", "DLPHIN vs reco, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
DLPHIN_vs_reco_depthE1_HE_h = rt.TH2F("DLPHIN_vs_reco_depthE1_HE_h", "DLPHIN vs reco, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)

ratio_ieta_depthG1_HB_h = rt.TH2F("ratio_ieta_depthG1_HB_h", "DLPHIN/reco for 10<reco<30GeV, depth > 1, HB", 17, 1, 18, 100, 0, 2)
ratio_ieta_depthG1_HE_h = rt.TH2F("ratio_ieta_depthG1_HE_h", "DLPHIN/reco for 10<reco<30GeV, depth > 1, HE", 15, 16, 31, 100, 0, 2)
ratio_ieta_depthE1_HB_h = rt.TH2F("ratio_ieta_depthE1_HB_h", "DLPHIN/reco for 10<reco<30GeV, depth = 1, HB", 17, 1, 18, 100, 0, 2)
ratio_ieta_depthE1_HE_h = rt.TH2F("ratio_ieta_depthE1_HE_h", "DLPHIN/reco for 10<reco<30GeV, depth = 1, HE", 15, 16, 31, 100, 0, 2)

#==============DLPHIN vs reco 3d hist ==================
ratio_ietaP_depthG1_HB_h = rt.TH3F("ratio_ietaP_depthG1_HB_h", "DLPHIN/reco for reco>1GeV, ieta > 0, depth > 1, HB", 17, 1, 18, 72, 1, 73, 100, 0, 2)
ratio_ietaM_depthG1_HB_h = rt.TH3F("ratio_ietaM_depthG1_HB_h", "DLPHIN/reco for reco>1GeV, ieta < 0, depth > 1, HB", 17, -17, 0, 72, 1, 73, 100, 0, 2)
ratio_ietaP_depthG1_HE_h = rt.TH3F("ratio_ietaP_depthG1_HE_h", "DLPHIN/reco for reco>1GeV, ieta > 0, depth > 1, HE", 15, 16, 31, 72, 1, 73, 100, 0, 2)
ratio_ietaM_depthG1_HE_h = rt.TH3F("ratio_ietaM_depthG1_HE_h", "DLPHIN/reco for reco>1GeV, ieta < 0, depth > 1, HE", 15, -30, -15, 72, 1, 73, 100, 0, 2)
ratio_ietaP_depthE1_HB_h = rt.TH3F("ratio_ietaP_depthE1_HB_h", "DLPHIN/reco for reco>1GeV, ieta > 0, depth = 1, HB", 17, 1, 18, 72, 1, 73, 100, 0, 2)
ratio_ietaM_depthE1_HB_h = rt.TH3F("ratio_ietaM_depthE1_HB_h", "DLPHIN/reco for reco>1GeV, ieta < 0, depth = 1, HB", 17, -17, 0, 72, 1, 73, 100, 0, 2)
ratio_ietaP_depthE1_HE_h = rt.TH3F("ratio_ietaP_depthE1_HE_h", "DLPHIN/reco for reco>1GeV, ieta > 0, depth = 1, HE", 15, 16, 31, 72, 1, 73, 100, 0, 2)
ratio_ietaM_depthE1_HE_h = rt.TH3F("ratio_ietaM_depthE1_HE_h", "DLPHIN/reco for reco>1GeV, ieta < 0, depth = 1, HE", 15, -30, -15, 72, 1, 73, 100, 0, 2)

Nrows = result.shape[0]
Nrows = 500000
print "total rows: ", Nrows
for i in range(Nrows):
    if i%100000 == 0: print "process %d rows" %i

    reco_corr = 1
    if run_mod == "slope1":
        reco_corr = reco_slope

    raw_gain = result["raw gain"] [i]
    gain = result["gain"] [i]
    respCorr = gain / raw_gain
    reco_energy = (result["mahi energy"][i] / respCorr) / reco_corr
    DLPHIN_energy = result["DLPHIN energy"][i]
    ieta = result["ieta"] [i]
    iphi = result["iphi"] [i]
    depth = result["depth"] [i]
    sub_det = result["sub detector"] [i]
    flags = result["flags"] [i]
    use_8_pulse = ((flags & use_8_pulse_bit) == use_8_pulse_bit)

    reco_h.Fill(reco_energy)
    DLPHIN_h.Fill(DLPHIN_energy)
    use_8_pulse_h.Fill(use_8_pulse)
    DLPHIN_vs_reco_h.Fill(reco_energy, DLPHIN_energy)

    if sub_det == 1:
        if depth == 1:
            DLPHIN_vs_reco_depthE1_HB_h.Fill(reco_energy, DLPHIN_energy)
            respCorr_depthE1_HB_h.Fill(respCorr)
            if reco_energy > 1:
                ratio = DLPHIN_energy / reco_energy
                if reco_energy > 10 and reco_energy < 30:
                   ratio_ieta_depthE1_HB_h.Fill(abs(ieta), ratio)
                if ieta > 0:
                   ratio_ietaP_depthE1_HB_h.Fill(ieta, iphi, ratio)
                else: 
                   ratio_ietaM_depthE1_HB_h.Fill(ieta, iphi, ratio)
        else:
            DLPHIN_vs_reco_depthG1_HB_h.Fill(reco_energy, DLPHIN_energy)
            respCorr_depthG1_HB_h.Fill(respCorr)
            if reco_energy > 1:
                ratio = DLPHIN_energy / reco_energy
                if reco_energy > 10 and reco_energy < 30:
                   ratio_ieta_depthG1_HB_h.Fill(abs(ieta), ratio)
                if ieta > 0:
                   ratio_ietaP_depthG1_HB_h.Fill(ieta, iphi, ratio)
                else: 
                   ratio_ietaM_depthG1_HB_h.Fill(ieta, iphi, ratio)
    elif sub_det == 2:
        if depth == 1:
            DLPHIN_vs_reco_depthE1_HE_h.Fill(reco_energy, DLPHIN_energy)
            respCorr_depthE1_HE_h.Fill(respCorr)
            if reco_energy > 1:
                ratio = DLPHIN_energy / reco_energy
                if reco_energy > 10 and reco_energy < 30:
                   ratio_ieta_depthE1_HE_h.Fill(abs(ieta), ratio)
                if ieta > 0:
                   ratio_ietaP_depthE1_HE_h.Fill(ieta, iphi, ratio)
                else: 
                   ratio_ietaM_depthE1_HE_h.Fill(ieta, iphi, ratio)
        else:
            DLPHIN_vs_reco_depthG1_HE_h.Fill(reco_energy, DLPHIN_energy)
            respCorr_depthG1_HE_h.Fill(respCorr)
            if reco_energy > 1:
                ratio = DLPHIN_energy / reco_energy
                if reco_energy > 10 and reco_energy < 30:
                   ratio_ieta_depthG1_HE_h.Fill(abs(ieta), ratio)
                if ieta > 0:
                   ratio_ietaP_depthG1_HE_h.Fill(ieta, iphi, ratio)
                else: 
                   ratio_ietaM_depthG1_HE_h.Fill(ieta, iphi, ratio)
    else: print "strange sub_det: ", sub_det

out_file.cd()
out_file.Write()
out_file.Close()
