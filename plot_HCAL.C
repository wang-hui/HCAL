int plot_HCAL()
{
    bool plot_reco_vs_gen_ieta = false;
    bool plot_reco_vs_gen_h = true;
    bool plot_ratio_h = false;
    bool plot_err_h = false;

    bool do_profile_study = false;

    std::vector<TString> hist_list =
    {
        //"reco_vs_gen",
        "reco_vs_gen_depthG1_HE", "reco_vs_gen_depthG1_HE_1_pulse", "reco_vs_gen_depthG1_HE_8_pulse",
        "reco_vs_gen_depthE1_HE", "reco_vs_gen_depthE1_HE_1_pulse", "reco_vs_gen_depthE1_HE_8_pulse",
        "reco_vs_gen_depthG1_HB", "reco_vs_gen_depthG1_HB_1_pulse", "reco_vs_gen_depthG1_HB_8_pulse",
        //"reco_vs_gen_depthE1_HB_ietaS15",
        "reco_vs_gen_depthE1_HB", "reco_vs_gen_depthE1_HB_1_pulse", "reco_vs_gen_depthE1_HB_8_pulse",

        //"aux_vs_gen",
        "aux_vs_gen_depthG1_HE", "aux_vs_gen_depthE1_HE", "aux_vs_gen_depthG1_HB", "aux_vs_gen_depthE1_HB",
        //"DLPHIN_vs_gen",
        "DLPHIN_vs_gen_depthG1_HE", "DLPHIN_vs_gen_depthE1_HE", "DLPHIN_vs_gen_depthG1_HB", "DLPHIN_vs_gen_depthE1_HB",
        //"reco_err",
        //"aux_err",
    };

    //TFile *f1 = new TFile("result_origin.root");
    TFile *f1 = new TFile("results/results_2018_1TeV_pion_gun_no_respCorr/2018_1TeV_pion_gun_PU0.root");

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        if(plot_reco_vs_gen_h)
        {
            TString h1_name = hist_name + "_h";

            TH2F *h1 = (TH2F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            gStyle->SetOptStat(kFALSE);

            auto xmin = h1->GetXaxis()->GetXmin();
            auto xmax = h1->GetXaxis()->GetXmax();
            auto ymin = h1->GetYaxis()->GetXmin();
            auto ymax = h1->GetYaxis()->GetXmax();

            h1->Draw("colz");
            h1->SetTitle(h1_name);
            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            //h1->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            h1->GetYaxis()->SetTitle("reco energy [GeV]");
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
            px->GetXaxis()->SetTitle("truth energy [GeV]");
            px->GetYaxis()->SetTitle("reco energy [GeV]");
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
            if (do_profile_study)
            {
                TProfile *py = h1->ProfileY("py");
                py->Draw();
                mycanvas->SaveAs("plots_temp/" + hist_name + "_profileY.png");
            }

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
            SD_h->GetXaxis()->SetTitle("truth energy [GeV]");
            SD_h->GetYaxis()->SetTitle("#sigma_{reco energy} / reco energy");
            SD_h->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            SD_h->GetYaxis()->SetRangeUser(0, 0.5);
            SD_h->Draw();
            mycanvas->SaveAs("plots_temp/" + hist_name + "_SD.png");
        }

        if(plot_reco_vs_gen_ieta)
        {
            int ieta_first, ieta_last;
            if (hist_name == "depthG1_HE" || hist_name == "depthE1_HE"){ieta_first = 16; ieta_last = 29;}
            if (hist_name == "depthE1_HB" || hist_name == "depthG1_HB"){ieta_first = 1; ieta_last = 16;}
            //if (hist_name == "depthG1_HB"){ieta_first = 14; ieta_last = 16;}

            for (int i = ieta_first; i <= ieta_last; i++)
            {
                TString h1_name = "reco_vs_gen_" + hist_name + "_iEta_" + to_string(i) + "_h";
                std::cout << h1_name << std::endl;
                TH2F *h1 = (TH2F*)f1->Get(h1_name);

                TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
                gStyle->SetOptStat(kFALSE);

                auto xmin = h1->GetXaxis()->GetXmin();
                auto xmax = h1->GetXaxis()->GetXmax() * 0.8;
                auto ymin = h1->GetYaxis()->GetXmin();
                auto ymax = h1->GetYaxis()->GetXmax() * 0.8;

                h1->Draw("colz");
                h1->SetTitle(h1_name);
                h1->GetXaxis()->SetTitle("truth energy [GeV]");
                h1->GetXaxis()->SetRangeUser(xmin, xmax);
                h1->GetYaxis()->SetTitle("reco energy [GeV]");
                h1->GetYaxis()->SetRangeUser(ymin, ymax);
                gPad->SetLogz();

                mycanvas->SetLeftMargin(0.15);
                mycanvas->SetRightMargin(0.15);
                mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

                TProfile *px = h1->ProfileX("px");
                px->SetTitle(h1_name);
                //px->GetYaxis()->SetNdivisions(512);
                px->GetYaxis()->SetRangeUser(ymin, ymax);
                px->SetLineColor(kRed);
                //px->SetLineWidth(2);
                //px->SetMarkerStyle(8);
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

                TLine *l=new TLine(xmin, ymin, xmax, ymax);
                l->SetLineColor(kBlack);
                l->Draw("same");

                mycanvas->SaveAs("plots_temp/" + h1_name + "_profile.png");
            }
        }

        if(plot_ratio_h)
        {
            TString h1_name = "ratio_" + hist_name + "_h";
            TH1F *h1 = (TH1F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            //gStyle->SetOptStat(kFALSE);

            h1->Draw();
            //h1->Fit("gaus","","",0.7,2);
            h1->GetXaxis()->SetTitle(h1->GetTitle());
            h1->SetTitle(h1_name);
            h1->GetYaxis()->SetTitle("number of channels");
            //gPad->SetLogz();

            TLine *l=new TLine(1, h1->GetMinimum(),1, h1->GetMaximum());
            l->SetLineColor(kBlack);
            l->Draw("same");

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
        }

        if(plot_err_h)
        {
            TString h1_name = hist_name + "_err_vs_gen_h";
            TH1F *h1 = (TH1F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            //gStyle->SetOptStat(kFALSE);

            h1->Draw();
            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            h1->SetTitle(h1_name);
            h1->GetYaxis()->SetTitle("|reco-truth|/truth");

            //TLine *l=new TLine(1, h1->GetMinimum(),1, h1->GetMaximum());
            //l->SetLineColor(kBlack);
            //l->Draw("same");

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
        }
    }
    return 0;
}
