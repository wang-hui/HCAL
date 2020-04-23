import ROOT as rt
import pandas as pd

out_file = rt.TFile("result_depth_test.root","RECREATE")

result = pd.read_csv("result.csv", sep=',', header=0)
reco_h = rt.TH1F("reco_h", "reco energy", 100, 0.0, 100.0)
gen_h = rt.TH1F("gen_h", "truth energy", 100, 0.0, 100.0)
reco_vs_gen_h = rt.TH2F("reco_vs_gen_h", "reco vs gen", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthG1_h = rt.TH2F("reco_vs_gen_depthG1_h", "reco vs gen, depth > 1", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthE1_HB_h = rt.TH2F("reco_vs_gen_depthE1_HB_h", "reco vs gen, depth = 1, HB", 100, 0.0, 100.0, 100, 0.0, 100.0)
reco_vs_gen_depthE1_HE_h = rt.TH2F("reco_vs_gen_depthE1_HE_h", "reco vs gen, depth = 1, HE", 100, 0.0, 100.0, 100, 0.0, 100.0)
ratio_h = rt.TH1F("ratio_h", "reco/gen for gen > 1 GeV", 100, 0.0, 2.0)
ratio_depthE1_HB_h = rt.TH1F("ratio_depthE1_HB_h", "reco/gen for gen > 1 GeV, depth = 1, HB", 100, 0.0, 2.0)
ratio_depthE1_HE_h = rt.TH1F("ratio_depthE1_HE_h", "reco/gen for gen > 1 GeV, depth = 1, HE", 100, 0.0, 2.0)
ratio_depthG1_h = rt.TH1F("ratio_depthG1_h", "reco/gen for gen > 1 GeV, depth > 1", 100, 0.0, 2.0)

Nrows = result.shape[0]
#Nrows = 100000
print "total rows: ", Nrows
for i in range(Nrows):
	if i%100000 == 0: print "process %d events" %i

	reco_energy = result["reco energy"][i]
	gen_energy = result["truth energy"][i]
	depth = result["depth"] [i]
	sub_det = result["sub detector"] [i]

	#if depth == 1:
	#	gen_energy = gen_energy * 5/12
	#	if sub_det == 2: gen_energy = gen_energy * 1.15
	if depth == 1 and sub_det == 2: gen_energy = gen_energy * 5/12

	reco_h.Fill(reco_energy)
	gen_h.Fill(gen_energy)
	reco_vs_gen_h.Fill(reco_energy, gen_energy)

	if depth == 1:
		if sub_det == 1: reco_vs_gen_depthE1_HB_h.Fill(reco_energy, gen_energy)
		elif sub_det == 2: reco_vs_gen_depthE1_HE_h.Fill(reco_energy, gen_energy)
		else: print "strange sub_det: ", sub_det
	else: reco_vs_gen_depthG1_h.Fill(reco_energy, gen_energy)

	if gen_energy > 1:
		ratio = reco_energy / gen_energy
		ratio_h.Fill(ratio)
		if depth == 1:
			if sub_det == 1: ratio_depthE1_HB_h.Fill(ratio)
			elif sub_det == 2: ratio_depthE1_HE_h.Fill(ratio)
		else: ratio_depthG1_h.Fill(ratio)

out_file.cd()
out_file.Write()
out_file.Close()
