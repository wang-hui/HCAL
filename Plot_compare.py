#lowercase with underline for function/class names
#canmel case for variables

import ROOT as rt

BaseName = "Old respCorr"
BaseFileList = ["Sunanda_respCorr.root"]
BaseHistList = ["D" + str(d+1) for d in range(7)]
#BaseHistList = ["LeadingCaloJet_nConstituents_h"]

Comp1Name = "New, noPU"
Comp1FileList = BaseFileList
Comp1FileList = ["Sunanda_zeroOut_respCorr.root"]
Comp1HistList = BaseHistList
#Comp1HistList = ["CaloMETBE_h"]

Comp2Name = "New, PU"
Comp2FileList = BaseFileList
Comp2FileList = ["Sunanda_zeroOut_PU_respCorr.root"]
Comp2HistList = BaseHistList
#Comp2HistList = ["myCaloMETBE_h"]

Comp3Name = "My new, PU"
Comp3FileList = BaseFileList
Comp3FileList = ["My_zeroOut_PU_respCorr_ieta26.root"]
Comp3HistList = BaseHistList
#Comp3HistList = ["myCaloMETBE_h"]

ShapeComp = False
SetLogY = False

YTitle = "respCorr"
XTitle = "ieta"
#XTitle = "nConstituents"

XMin = 0
XMax = 0
YScale = 1.5

FileDir = "results/"
HistDir = ""

if ShapeComp: YTitle = "A.U."

class MyStruct:
    def __init__(self, Name, FileList, HistList, Color, StructList):
        self.Name = Name
        self.FileList = FileList
        self.HistList = HistList
        self.Color = Color
        StructList.append(self)

StructList = []
Base = MyStruct(BaseName, BaseFileList, BaseHistList, rt.kBlue, StructList)
Comp1 = MyStruct(Comp1Name, Comp1FileList, Comp1HistList, rt.kRed, StructList)
Comp2 = MyStruct(Comp2Name, Comp2FileList, Comp2HistList, rt.kGreen+1, StructList)
Comp3 = MyStruct(Comp3Name, Comp3FileList, Comp3HistList, rt.kYellow+1, StructList)

rt.TH1.AddDirectory(rt.kFALSE)
#rt.TH1.__init__._creates = False

for i in range(len(BaseFileList)):
    for j in range(len(BaseHistList)):
        MyCanvas = rt.TCanvas("MyCanvas", "MyCanvas", 600, 600)
        rt.gStyle.SetOptStat(rt.kFALSE)

        MyLeg = rt.TLegend(0.5,0.65,0.9,0.9)

        PadUp = rt.TPad("PadUp", "PadUp", 0, 0.3, 1, 1.0)
        PadUp.SetBottomMargin(0.01)
        PadUp.Draw()

        PadDown = rt.TPad("PadDown", "PadDown", 0, 0, 1, 0.3)
        PadDown.SetTopMargin(0.03)
        PadDown.SetBottomMargin(0.3)
        PadDown.SetGrid()
        PadDown.Draw()

        OutName = BaseFileList[i].replace(".root", "_")
        BaseHist = None

        for k in range(len(StructList)):
            iFileName = StructList[k].FileList[i]
            iHistName = StructList[k].HistList[j]
            OutName = OutName + iHistName.replace("_h", "_")
            iFile = rt.TFile.Open(FileDir + iFileName)
            iHist = iFile.Get(HistDir + iHistName)
            print k, iHist
            iHist.SetLineColor(StructList[k].Color)
            iHist.Sumw2()
            if ShapeComp: iHist.Scale(1/iHist.GetEntries())
            MyLeg.AddEntry(iHist, StructList[k].Name, "l")

            MyCanvas.cd()
            PadUp.cd()

            YMaxTemp = iHist.GetMaximum() * YScale
            if k == 0:
                BaseHist = iHist.Clone()

                BaseHist.GetYaxis().SetTitle(YTitle)
                BaseHist.SetMaximum(YMaxTemp)
                BaseHist.SetTitle("")
                if XMax > 0:
                    BaseHist.GetXaxis().SetRangeUser(XMin, XMax)
                BaseHist.Draw("hist")

                MyCanvas.cd()
                PadDown.cd()
                BaseFrame = BaseHist.Clone()
                BaseFrame.Reset()

                BaseFrame.GetYaxis().SetTitle("Ratio")
                BaseFrame.GetYaxis().SetTitleOffset(0.4)
                BaseFrame.GetYaxis().SetTitleSize(0.1)
                BaseFrame.GetYaxis().SetLabelSize(0.08)
                BaseFrame.GetYaxis().SetRangeUser(0, 2)

                BaseFrame.GetXaxis().SetTitle(XTitle)
                BaseFrame.GetXaxis().SetTitleOffset(0.8)
                BaseFrame.GetXaxis().SetTitleSize(0.1)
                BaseFrame.GetXaxis().SetLabelSize(0.08)
                BaseFrame.Draw()

                MyLine = rt.TLine(BaseFrame.GetXaxis().GetXmin(), 1.0, BaseFrame.GetXaxis().GetXmax(), 1.0)
                MyLine.Draw()
            else:
                if YMaxTemp > BaseHist.GetMaximum(): 
                    BaseHist.SetMaximum(YMaxTemp)
                iHist.Draw("histsame")

                MyCanvas.cd()
                PadDown.cd()
                RatioHist = iHist.Clone()
                rt.SetOwnership(RatioHist, False)
                RatioHist.Divide(BaseHist)
                RatioHist.Draw("same")

        MyCanvas.cd()
        PadUp.cd()
        if SetLogY: PadUp.SetLogy()
        MyLeg.Draw()
        MyCanvas.SaveAs("plots_temp/" + OutName + ".png")

