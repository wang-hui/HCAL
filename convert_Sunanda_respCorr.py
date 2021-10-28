import ROOT as rt
import pandas as pd

DLPHIN_respCorr_pd = pd.read_csv("results/dlphin_zeroout_PU_ieta26.txt", sep=' ', skipinitialspace = True, header=0)
#print DLPHIN_respCorr_pd
#print DLPHIN_respCorr_pd.dtypes

out_file = rt.TFile("My_zeroOut_PU_respCorr_ieta26.root", "recreate")
HistList = [rt.TH1D("D" + str(d+1), "D" + str(d+1), 59, -29.5, 29.5) for d in range(7)]

Nrows = DLPHIN_respCorr_pd.shape[0]
for row in range(Nrows):
    ieta = DLPHIN_respCorr_pd["ieta"][row]
    depth = DLPHIN_respCorr_pd["depth"][row]
    respCorr = DLPHIN_respCorr_pd["respCorr"][row]
    unc = DLPHIN_respCorr_pd["unc"][row]

    HistList[depth-1].SetBinContent(HistList[depth-1].FindBin(ieta), respCorr)
    HistList[depth-1].SetBinError(HistList[depth-1].FindBin(ieta), unc)

out_file.cd()
out_file.Write()
out_file.Close()

