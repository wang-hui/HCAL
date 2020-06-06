import ROOT as rt
import pandas as pd

result_dir = ""
result_file = "result"

result = pd.read_csv(result_dir + result_file + ".csv", sep=',', header=0)

run_mod = ""
run_mod = "_no_PU_energy"
out_file = rt.TFile(result_file + run_mod + ".root","RECREATE")

PU_h = rt.TH1F("PU_h", "pileup", 100, 0.0, 100.0)
reco_h = rt.TH1F("reco_h", "reco energy", 100, 0.0, 100.0)
gen_h = rt.TH1F("gen_h", "truth energy", 100, 0.0, 100.0)
reco_vs_gen_h = rt.TH2F("reco_vs_gen_h", "reco vs gen", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthG1_h = rt.TH2F("reco_vs_gen_depthG1_h", "reco vs gen, depth > 1", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthG1_HB_h = rt.TH2F("reco_vs_gen_depthG1_HB_h", "reco vs gen, depth > 1, HB", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthG1_HE_h = rt.TH2F("reco_vs_gen_depthG1_HE_h", "reco vs gen, depth > 1, HE", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthE1_HB_h = rt.TH2F("reco_vs_gen_depthE1_HB_h", "reco vs gen, depth = 1, HB", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthE1_HE_h = rt.TH2F("reco_vs_gen_depthE1_HE_h", "reco vs gen, depth = 1, HE", 100, 0.0, 100.0, 100, 0.0, 100.0)
ratio_h = rt.TH1F("ratio_h", "reco/gen for gen > 1 GeV", 100, 0.0, 2.0)
ratio_depthG1_h = rt.TH1F("ratio_depthG1_h", "reco/gen for gen > 1 GeV, depth > 1", 100, 0.0, 2.0)
ratio_depthG1_HB_h = rt.TH1F("ratio_depthG1_HB_h", "reco/gen for gen > 1 GeV, depth > 1, HB", 100, 0.0, 2.0)
ratio_depthG1_HE_h = rt.TH1F("ratio_depthG1_HE_h", "reco/gen for gen > 1 GeV, depth > 1, HE", 100, 0.0, 2.0)
ratio_depthE1_HB_h = rt.TH1F("ratio_depthE1_HB_h", "reco/gen for gen > 1 GeV, depth = 1, HB", 100, 0.0, 2.0)
ratio_depthE1_HE_h = rt.TH1F("ratio_depthE1_HE_h", "reco/gen for gen > 1 GeV, depth = 1, HE", 100, 0.0, 2.0)

reco_vs_gen_depthE1_HB_list = []
reco_vs_gen_depthG1_HB_list = []
for i in range(1,17):
	E1_hist = rt.TH2F("reco_vs_gen_depthE1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HB |ieta| " + str(i), 100, 0.0, 100.0, 100, 0.0, 100.0)
	reco_vs_gen_depthE1_HB_list.append(E1_hist)
	G1_hist = rt.TH2F("reco_vs_gen_depthG1_HB_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HB |ieta| " + str(i), 100, 0.0, 100.0, 100, 0.0, 100.0)
	reco_vs_gen_depthG1_HB_list.append(G1_hist)

reco_vs_gen_depthE1_HE_list = []
reco_vs_gen_depthG1_HE_list = []
for i in range(16,30):
	E1_hist = rt.TH2F("reco_vs_gen_depthE1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth = 1, HE |ieta| " + str(i), 100, 0.0, 100.0, 100, 0.0, 100.0)
	reco_vs_gen_depthE1_HE_list.append(E1_hist)
	G1_hist = rt.TH2F("reco_vs_gen_depthG1_HE_iEta_" + str(i) + "_h", "reco vs gen, depth > 1, HE |ieta| " + str(i), 100, 0.0, 100.0, 100, 0.0, 100.0)
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


	#if depth == 1:
	#	gen_energy = gen_energy * 5/12
	#	if sub_det == 2: gen_energy = gen_energy * 1.15
	#if depth == 1 and sub_det == 2: gen_energy = gen_energy * 5/12

	PU_h.Fill(PU)
	reco_h.Fill(reco_energy)
	gen_h.Fill(gen_energy)
	reco_vs_gen_h.Fill(reco_energy, gen_energy)

	if depth == 1:
		if sub_det == 1:
			reco_vs_gen_depthE1_HB_h.Fill(reco_energy, gen_energy)
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
			if sub_det == 1: ratio_depthE1_HB_h.Fill(ratio)
			elif sub_det == 2: ratio_depthE1_HE_h.Fill(ratio)
		else:
			ratio_depthG1_h.Fill(ratio)
			if sub_det == 1: ratio_depthG1_HB_h.Fill(ratio)
			elif sub_det == 2: ratio_depthG1_HE_h.Fill(ratio)

out_file.cd()
out_file.Write()
out_file.Close()
