int plot_HCAL_data()
{
    bool plot_2D = false;
    bool plot_3D = true;
    bool plot_2D_fit = false;

    std::vector<TString> hist_list =
    {
        //"DLPHIN_vs_gen",
        //"DLPHIN_vs_reco_depthG1_HE", "DLPHIN_vs_reco_depthE1_HE", "DLPHIN_vs_reco_depthG1_HB", "DLPHIN_vs_reco_depthE1_HB",
        //"ratio_ieta_depthG1_HB_EL", "ratio_ieta_depthG1_HE_EL", "ratio_ieta_depthE1_HB_EL", "ratio_ieta_depthE1_HE_EL",
        //"ratio_ieta_depthG1_HB_EH", "ratio_ieta_depthG1_HE_EH", "ratio_ieta_depthE1_HB_EH", "ratio_ieta_depthE1_HE_EH",
        "ratio_ieta_depth_HB", "ratio_ieta_depth_HE",
        //"ratio_ietaP_depthG1_HE", "ratio_ietaM_depthG1_HE", "ratio_ietaP_depthE1_HE", "ratio_ietaM_depthE1_HE",
        //"ratio_ietaP_depthG1_HB", "ratio_ietaM_depthG1_HB", "ratio_ietaP_depthE1_HB", "ratio_ietaM_depthE1_HB",
    };

    TFile *f1 = new TFile("results/DoubleMuon_Run2018A_Run_315512_DLPHIN_vs_MAHI.root");

    TFile out_file("DLPHIN_MAHI_ratio.root","RECREATE");

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        if(plot_2D)
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

        if(plot_3D)
        {
            TString h1_name = hist_name + "_h";

            TH3F *h1 = (TH3F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            gStyle->SetOptStat(kFALSE);
            gStyle->SetPaintTextFormat(".2f");

            TProfile2D *pz = h1->Project3DProfile("yx");

            pz->SetTitle(h1_name);
            pz->GetXaxis()->SetTitle("ieta");
            pz->GetYaxis()->SetTitle("depth");
            //pz->GetZaxis()->SetRangeUser(0.5,1.5);
            pz->Draw("colz");

            //int ieta24 = pz->GetXaxis()->FindBin(24);
            //int depth1 = pz->GetYaxis()->FindBin(1);
            //std::cout << "ieta24(" << ieta24 << "), depth1(" << depth1 << ") " << pz->GetBinContent(ieta24, depth1) << std::endl;

            TProfile2D* entries_h = (TProfile2D*)pz->Clone();
            entries_h->Reset();

            for(int i = 1; i <= pz->GetNbinsX(); i++)
            {
                for(int j = 1; j <= pz->GetNbinsY(); j++)
                {
                    int global_bin_number = pz->GetBin(i,j,0);
                    int entries = pz->GetBinEntries(global_bin_number);
                    float err = (float)sqrt(entries) / (float)entries;
                    if (entries > 0)
                    {
                        entries_h->SetBinContent(global_bin_number, err);
                        entries_h->SetBinEntries(global_bin_number, 1);
                        //std::cout << err << ", " << entries_h->GetBinContent(global_bin_number) << ", " << entries_h->GetBinEntries(global_bin_number) << std::endl;
                    }
                    //std::cout << "bin" << global_bin_number << "(" << i << "," << j << ") " << entries << ", " << entries_h->GetBinContent(global_bin_number) << std::endl;
                }
            }
            entries_h->Draw("sametext");

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + hist_name + "_profile.png");
            pz->Draw("text");
            mycanvas->SaveAs("plots_temp/" + hist_name + "_profile_test.png");

            out_file.cd();
            pz->Write();
        }

        if(plot_2D_fit)
        {
            TString h1_name = hist_name + "_h";

            TH2F *h1 = (TH2F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            gStyle->SetOptStat(kFALSE);

            h1->Draw("colz");
            h1->SetTitle(h1_name);
            h1->GetXaxis()->SetTitle("ieta");
            h1->GetYaxis()->SetTitle("DLPHIN/reco");
            //gPad->SetLogz();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + hist_name + ".png");

            TProfile *px = h1->ProfileX("px", 1, -1, "os");
            //px->BuildOptions(0, 0, "s");
            px->SetTitle(h1_name);
            //px->GetYaxis()->SetNdivisions(512);
            px->SetLineColor(kRed);
            //px->SetLineWidth(2);
            //px->SetMarkerStyle(8);
            px->GetXaxis()->SetTitle("ieta");
            px->GetYaxis()->SetTitle("DLPHIN/reco");
            px->GetYaxis()->SetRangeUser(0,2);
            px->Draw();
            gPad->SetGrid();
            px->Fit("pol0", "w");
            TF1 *f = (TF1*)px->GetListOfFunctions()->FindObject("pol0");
            TString fit_TS = "";
            if(f)
            {
                f->SetLineColor(kBlue);
                f->SetLineWidth(1);
                f->SetLineStyle(2); // 2 = "- - -"

                std::stringstream fit_SS;
                fit_SS << "fit = " << std::setprecision(2) << std::fixed << f->GetParameter(0) << " +- " << f->GetParError(0);
                fit_TS = fit_SS.str();
            }

            TLatex latex;
            latex.SetTextSize(0.04);
            latex.SetNDC();
            latex.DrawLatex(0.2,0.85,fit_TS);
            //TLine *l=new TLine(xmin, ymin, xmax * 0.8, ymax * 0.8);
            //l->SetLineColor(kBlack);
            //if(!hist_name.Contains("err"))
            //{l->Draw("same");}

            mycanvas->SaveAs("plots_temp/" + hist_name + "_profile.png");

        }
    }
    out_file.Close();
    return 0;
}
