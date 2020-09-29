int plot_HCAL_data()
{
    bool plot_reco_vs_gen_h = false;
    bool plot_ratio_h = true;

    std::vector<TString> hist_list =
    {
        //"DLPHIN_vs_gen",
        //"DLPHIN_vs_reco_depthG1_HE", "DLPHIN_vs_reco_depthE1_HE", "DLPHIN_vs_reco_depthG1_HB", "DLPHIN_vs_reco_depthE1_HB",
        //"ratio_ietaP_depthG1_HE", "ratio_ietaM_depthG1_HE", "ratio_ietaP_depthE1_HE", "ratio_ietaM_depthE1_HE",
        "ratio_ietaP_depthG1_HB", "ratio_ietaM_depthG1_HB", "ratio_ietaP_depthE1_HB", "ratio_ietaM_depthE1_HB",
    };

    TFile *f1 = new TFile("result_origin.root");

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        if(plot_reco_vs_gen_h)
        {
            TString h1_name = hist_name + "_h";

            TH2F *h1 = (TH2F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            //gStyle->SetOptStat(kFALSE);

            auto xmin = h1->GetXaxis()->GetXmin();
            auto xmax = h1->GetXaxis()->GetXmax();
            auto ymin = h1->GetYaxis()->GetXmin();
            auto ymax = h1->GetYaxis()->GetXmax();

            h1->Draw("colz");
            h1->SetTitle(h1_name);
            h1->GetXaxis()->SetTitle("reco energy [GeV]");
            //h1->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            h1->GetYaxis()->SetTitle("DLPHIN energy [GeV]");
            //h1->GetYaxis()->SetRangeUser(ymin, ymax * 0.8);
            gPad->SetLogz();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + hist_name + ".png");

            TProfile *px = h1->ProfileX("px", 1, -1, "os");
            //px->BuildOptions(0, 0, "s");
            px->SetTitle(h1_name);
            //px->GetYaxis()->SetNdivisions(512);
            px->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            px->GetYaxis()->SetRangeUser(ymin, ymax * 0.8);
            px->SetLineColor(kRed);
            //px->SetLineWidth(2);
            //px->SetMarkerStyle(8);
            px->GetXaxis()->SetTitle("reco energy [GeV]");
            px->GetYaxis()->SetTitle("DLPHIN energy [GeV]");
            px->Draw();
            gPad->SetGrid();
            px->Fit("pol1", "w");
            TF1 *f = (TF1*)px->GetListOfFunctions()->FindObject("pol1");
            if(f)
            {
                f->SetLineColor(kBlue);
                f->SetLineWidth(1);
                f->SetLineStyle(2); // 2 = "- - -"
            }

            TLine *l=new TLine(xmin, ymin, xmax * 0.8, ymax * 0.8);
            l->SetLineColor(kBlack);
            if(!hist_name.Contains("err"))
            {l->Draw("same");}

            mycanvas->SaveAs("plots_temp/" + hist_name + "_profile.png");

            int nBins = px->GetXaxis()->GetNbins();
            auto SD_h = new TH1F(hist_name + "_SD_h", hist_name + "_SD_h", nBins, xmin, xmax);
            for(int i = 1; i<= nBins; i++)
            {
                float error = px->GetBinError(i);
                float center = px->GetBinContent(i);
                float rel_error = 0;
                if (center > 0) rel_error = error/center;
                SD_h->SetBinContent(i, rel_error);
                //std::cout << i << ", " << error << ", " << center << ", " << SD_h->GetBinContent(i) << std::endl;
            }
            SD_h->GetXaxis()->SetTitle("reco energy [GeV]");
            SD_h->GetYaxis()->SetTitle("#sigma_{DLPHIN} / DLPHIN");
            SD_h->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            SD_h->GetYaxis()->SetRangeUser(0, 0.5);
            SD_h->Draw();
            mycanvas->SaveAs("plots_temp/" + hist_name + "_SD.png");
        }

        if(plot_ratio_h)
        {
            TString h1_name = hist_name + "_h";

            TH3F *h1 = (TH3F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            gStyle->SetOptStat(kFALSE);

            TProfile2D *pz = h1->Project3DProfile("yx");

            pz->SetTitle(h1_name);
            pz->GetXaxis()->SetTitle("ieta");
            pz->GetYaxis()->SetTitle("iphi");
            pz->GetZaxis()->SetRangeUser(0.5,1.5);
            pz->Draw("colz");

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + hist_name + "_profile.png");
        }
    }
    return 0;
}
