import ROOT as rt
import pandas as pd

result_dir = ""
result_file = "result"

result = pd.read_csv(result_dir + result_file + ".csv", sep=',', header=0)

run_mod = ""
run_mod = "_no_PU_energy"

study_gain = False
out_file = rt.TFile(result_file + run_mod + ".root","RECREATE")

Ebins = 200
Emin = 0.0
Emax = 1000.0

fcByPE_h = rt.TH1F("fcByPE_h", "fcByPE for each TS", 100, 0.0, 1000.0)
PU_h = rt.TH1F("PU_h", "pileup", 100, 0.0, 100.0)
reco_h = rt.TH1F("reco_h", "reco energy", Ebins, Emin, Emax)
gen_h = rt.TH1F("gen_h", "truth energy", Ebins, Emin, Emax)
reco_vs_gen_h = rt.TH2F("reco_vs_gen_h", "reco vs gen", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_h = rt.TH2F("reco_vs_gen_depthG1_h", "reco vs gen, depth > 1", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HB_h = rt.TH2F("reco_vs_gen_depthG1_HB_h", "reco vs gen, depth > 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthG1_HE_h = rt.TH2F("reco_vs_gen_depthG1_HE_h", "reco vs gen, depth > 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HB_h = rt.TH2F("reco_vs_gen_depthE1_HB_h", "reco vs gen, depth = 1, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HB_ietaS15_h = rt.TH2F("reco_vs_gen_depthE1_HB_ietaS15_h", "reco vs gen, depth = 1, |ieta| < 15, HB", Ebins, Emin, Emax, Ebins, Emin, Emax)
reco_vs_gen_depthE1_HE_h = rt.TH2F("reco_vs_gen_depthE1_HE_h", "reco vs gen, depth = 1, HE", Ebins, Emin, Emax, Ebins, Emin, Emax)
sum_amp_over_gen_h = rt.TH1F("sum_amp_over_gen_h", "sum 8 amplitudes / gen for gen > 10 GeV", 100, 0.0, 2500.0)
raw_energy_over_gen_h = rt.TH1F("raw_energy_over_gen_h", "raw reco energy / gen for gen > 10 GeV", 100, 0.0, 2500.0)
inv_gain_h = rt.TH1F("inv_gain_h", "1/gain for gen > 10 GeV", 100, 0.0, 2500.0)
ratio_h = rt.TH1F("ratio_h", "reco/gen for gen > 1 GeV", 100, 0.0, 2.0)
ratio_depthG1_h = rt.TH1F("ratio_depthG1_h", "reco/gen for gen > 1 GeV, depth > 1", 100, 0.0, 2.0)
ratio_depthG1_HB_h = rt.TH1F("ratio_depthG1_HB_h", "reco/gen for gen > 1 GeV, depth > 1, HB", 100, 0.0, 2.0)
ratio_depthG1_HE_h = rt.TH1F("ratio_depthG1_HE_h", "reco/gen for gen > 1 GeV, depth > 1, HE", 100, 0.0, 2.0)
ratio_depthE1_HB_h = rt.TH1F("ratio_depthE1_HB_h", "reco/gen for gen > 1 GeV, depth = 1, HB", 100, 0.0, 2.0)
ratio_depthE1_HB_ietaS15_h = rt.TH1F("ratio_depthE1_HB_ietaS15_h", "reco/gen for gen > 1 GeV, depth = 1, |ieta| < 15, HB", 100, 0.0, 2.0)
ratio_depthE1_HE_h = rt.TH1F("ratio_depthE1_HE_h", "reco/gen for gen > 1 GeV, depth = 1, HE", 100, 0.0, 2.0)

reco_vs_gen_depthE1_HB_list = []
reco_vs_gen_depthG1_HB_list = []
for i in range(1,17):
    E1_hist = rt.TH2F("reco_vs_gen_depthE1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HB |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthE1_HB_list.append(E1_hist)
    G1_hist = rt.TH2F("reco_vs_gen_depthG1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HB |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthG1_HB_list.append(G1_hist)

reco_vs_gen_depthE1_HE_list = []
reco_vs_gen_depthG1_HE_list = []
for i in range(16,30):
    E1_hist = rt.TH2F("reco_vs_gen_depthE1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HE |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthE1_HE_list.append(E1_hist)
    G1_hist = rt.TH2F("reco_vs_gen_depthG1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HE |ieta| " + str(i), Ebins, Emin, Emax, Ebins, Emin, Emax)
    reco_vs_gen_depthG1_HE_list.append(G1_hist)

Nrows = result.shape[0]
#Nrows = 100000
print "total rows: ", Nrows
for i in range(Nrows):
    if i%100000 == 0: print "process %d rows" %i

    gen_energy = 0.0
    if run_mod == "": gen_energy = result["truth energy"][i]
    if run_mod == "_no_PU_energy": gen_energy = result["raw truth energy"][i]
    reco_energy = result["reco energy"][i]
    ieta = abs(result["ieta"] [i])
    depth = result["depth"] [i]
    sub_det = result["sub detector"] [i]
    PU = result["PU"] [i]
    gain = result["gain"] [i]
    raw_energy = result["raw energy"] [i]

    sum_amplitude = 0
    if study_gain:
        for j in range(1,9):
            fcByPE_str = "TS" + str(j) + " fcByPE"
            fcByPE_h.Fill(result[fcByPE_str] [i])

            RC_str = "TS" + str(j) + " raw charge"
            ped_str = "TS" + str(j) + " ped noise"
            sum_amplitude += max((result[RC_str] [i] - result[ped_str] [i]), 0)

    PU_h.Fill(PU)
    reco_h.Fill(reco_energy)
    gen_h.Fill(gen_energy)
    reco_vs_gen_h.Fill(reco_energy, gen_energy)

    if depth == 1:
        if sub_det == 1:
            reco_vs_gen_depthE1_HB_h.Fill(reco_energy, gen_energy)
            if ieta < 15: reco_vs_gen_depthE1_HB_ietaS15_h.Fill(reco_energy, gen_energy)
            reco_vs_gen_depthE1_HB_list[ieta - 1].Fill(reco_energy, gen_energy)
        elif sub_det == 2:
            reco_vs_gen_depthE1_HE_h.Fill(reco_energy, gen_energy)
            reco_vs_gen_depthE1_HE_list[ieta - 16].Fill(reco_energy, gen_energy)
        else: print "strange sub_det: ", sub_det
    else:
        reco_vs_gen_depthG1_h.Fill(reco_energy, gen_energy)
        if sub_det == 1:
            reco_vs_gen_depthG1_HB_h.Fill(reco_energy, gen_energy)
            reco_vs_gen_depthG1_HB_list[ieta - 1].Fill(reco_energy, gen_energy)
        elif sub_det == 2:
            reco_vs_gen_depthG1_HE_h.Fill(reco_energy, gen_energy)
            reco_vs_gen_depthG1_HE_list[ieta - 16].Fill(reco_energy, gen_energy)
        else: print "strange sub_det: ", sub_det

    if gen_energy > 1:
        ratio = reco_energy / gen_energy
        ratio_h.Fill(ratio)
        if depth == 1:
            if sub_det == 1:
                ratio_depthE1_HB_h.Fill(ratio)
                if ieta < 15: ratio_depthE1_HB_ietaS15_h.Fill(ratio)
            elif sub_det == 2: ratio_depthE1_HE_h.Fill(ratio)
        else:
            ratio_depthG1_h.Fill(ratio)
            if sub_det == 1: ratio_depthG1_HB_h.Fill(ratio)
            elif sub_det == 2: ratio_depthG1_HE_h.Fill(ratio)
               
        if gen_energy > 10:
            sum_amp_over_gen_h.Fill(sum_amplitude / gen_energy)
            raw_energy_over_gen_h.Fill(raw_energy / gen_energy)
            inv_gain_h.Fill(1.0 / gain)

out_file.cd()
out_file.Write()
out_file.Close()
