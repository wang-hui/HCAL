import ROOT as rt

in_file = rt.TFile.Open("results/My_zeroOut_PU_respCorr_ieta26.root")
out_file = rt.TFile("DLPHIN_zeroOut_PU_respCorr_my_ieta26.root", "recreate")
SubDetList = ["HB", "HE"]

HB_ieta_list = range(-1, -17, -1) + range(1, 17)
HB_depth_list = range(1,3)
HE_ieta_list = range(-16, -30, -1) + range(16, 30)
HE_depth_list = range(1,8)

for SubDet in SubDetList:
    ieta_list = HB_ieta_list
    depth_list = HB_depth_list
    if SubDet == "HE":
        ieta_list = HE_ieta_list
        depth_list = HE_depth_list
    #print SubDet
    #print ieta_list
    #print depth_list

    for depth in depth_list:
        in_hist = in_file.Get("D" + str(depth))
        last_respCorrN = 1.0
        last_uncN = 0.0
        last_respCorrP = 1.0
        last_uncP = 0.0
        for ieta in ieta_list:
            respCorr_temp = in_hist.GetBinContent(in_hist.FindBin(ieta))
            unc_temp = in_hist.GetBinError(in_hist.FindBin(ieta))

            respCorr = 1.0
            unc = 0.0
            if respCorr_temp != 0:
                respCorr = respCorr_temp
                unc = unc_temp

            if ieta <= -25:
                if respCorr_temp != 0:
                    last_respCorrN = respCorr_temp
                    last_uncN = unc_temp
                respCorr = last_respCorrN
                unc = last_uncN
            if ieta >= 25:
                if respCorr_temp != 0:
                    last_respCorrP = respCorr_temp
                    last_uncP = unc_temp
                respCorr = last_respCorrP
                unc = last_uncP

            h1_name = "DLPHIN_respCorr_" + SubDet + "_iEta_" + str(ieta) + "_depth_" + str(depth) + "_h"
            print h1_name, "respCorr ", respCorr, "+-", unc
            h1 = rt.TH1F(h1_name, h1_name, 1, 0, 9999)
            for h1_bin in range(3):
                h1.SetBinContent(h1_bin, respCorr)
                h1.SetBinError(h1_bin, unc)
            h1.Write()

out_file.Close()

