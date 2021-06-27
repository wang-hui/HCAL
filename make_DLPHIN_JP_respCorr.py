import ROOT as rt

in_file = rt.TFile.Open("results/JP_respCorr_noPU.root")
out_file = rt.TFile("DLPHIN_JP_respCorr.root", "recreate")
SubDetList = ["HB", "HE"]

HB_ieta_list = []
for i in range(1, 17): HB_ieta_list.append(i)
for i in range(-16, 0): HB_ieta_list.append(i)
HB_depth_list = range(1,3)

HE_ieta_list = []
for i in range(16, 30): HE_ieta_list.append(i)
for i in range(-29, -15): HE_ieta_list.append(i)
HE_depth_list = range(1,8)

for SubDet in SubDetList:
    ieta_list = []
    depth_list = []
    if SubDet == "HB":
        ieta_list = HB_ieta_list
        depth_list = HB_depth_list
    if SubDet == "HE":
        ieta_list = HE_ieta_list
        depth_list = HE_depth_list
    #print SubDet
    #print ieta_list
    #print depth_list

    for i in ieta_list:
        for j in depth_list:
            in_hist = in_file.Get("D" + str(j))
            respCorr_temp = in_hist.GetBinContent(in_hist.FindBin(i))
            unc_temp = in_hist.GetBinError(in_hist.FindBin(i))

            h1_name = "DLPHIN_respCorr_" + SubDet + "_iEta_" + str(i) + "_depth_" + str(j) + "_h"
            respCorr = 1.0
            unc = 0.0
            if respCorr_temp != 0:
                respCorr = respCorr_temp
                unc = unc_temp

            print h1_name, "respCorr ", respCorr, "+-", unc
            h1 = rt.TH1F(h1_name, h1_name, 1, 0, 9999)
            for h1_bin in range(3):
                h1.SetBinContent(h1_bin, respCorr)
                h1.SetBinError(h1_bin, unc)
            h1.Write()

out_file.Close()

