import ROOT as rt
import pandas as pd

DLPHIN_respCorr_pd = pd.read_csv("results/dlphin_corr0.txt", sep=' ', skipinitialspace = True, header=0)
#print DLPHIN_respCorr_pd
#print DLPHIN_respCorr_pd.dtypes

out_file = rt.TFile("DLPHIN_respCorr.root", "recreate")
SubDetList = ["HB", "HE"]

for SubDet in SubDetList:
    ieta_first = 1
    ieta_last = 16
    depth_first = 1
    depth_last = 2
    if SubDet == "HE":
        ieta_first = 16
        ieta_last = 29
        depth_first = 1
        depth_last = 7

    for i in range(ieta_first, ieta_last+1):
        for j in range(depth_first, depth_last+1):
            h1_name = "DLPHIN_respCorr_" + SubDet + "_iEta_" + str(i) + "_depth_" + str(j) + "_h"

            respCorr = 1.0
            unc = 0.0

            Nrows = DLPHIN_respCorr_pd.shape[0]
            for row in range(Nrows):
                ieta = DLPHIN_respCorr_pd["ieta"][row]
                depth = DLPHIN_respCorr_pd["depth"][row]
                respCorr_temp = DLPHIN_respCorr_pd["respCorr"][row]
                unc_temp = DLPHIN_respCorr_pd["unc"][row]

                if ieta == i and depth == j:
                    respCorr = respCorr_temp
                    unc = unc_temp
                    break

            print h1_name, "respCorr ", respCorr, "+-", unc
            h1 = rt.TH1F(h1_name, h1_name, 1, 0, 9999)
            for h1_bin in range(3):
                h1.SetBinContent(h1_bin, respCorr)
                h1.SetBinError(h1_bin, unc)
            h1.Write()

#out_file.cd()
#out_file.Write()
out_file.Close()
