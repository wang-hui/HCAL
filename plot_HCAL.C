int plot_HCAL()
{
    bool plot_reco_vs_gen_ieta = false;
    bool plot_reco_vs_gen = false;
    bool plot_ratio = false;
    bool plot_1d = false;
    bool plot_1d_fit = true;

    bool do_profile_study = false;

    std::vector<TString> hist_list =
    {
        //"gen", "genG0",
        "genG0",

        //"reco_vs_gen",
        //"reco_vs_gen_depthG1_HE", "reco_vs_gen_depthE1_HE", "reco_vs_gen_depthG1_HB", "reco_vs_gen_depthE1_HB"
        //"reco_vs_gen_depthG1_HE", "reco_vs_gen_depthG1_HE_1_pulse", "reco_vs_gen_depthG1_HE_8_pulse",
        //"reco_vs_gen_depthE1_HE", "reco_vs_gen_depthE1_HE_1_pulse", "reco_vs_gen_depthE1_HE_8_pulse",
        //"reco_vs_gen_depthG1_HB", "reco_vs_gen_depthG1_HB_1_pulse", "reco_vs_gen_depthG1_HB_8_pulse",
        //"reco_vs_gen_depthE1_HB_ietaS15",
        //"reco_vs_gen_depthE1_HB", "reco_vs_gen_depthE1_HB_1_pulse", "reco_vs_gen_depthE1_HB_8_pulse",

        //"reco_vs_gen_depthG1_HE", "reco_vs_gen_depthG1_HE_PUL", "reco_vs_gen_depthG1_HE_PUH",
        //"reco_vs_gen_depthE1_HE", "reco_vs_gen_depthE1_HE_PUL", "reco_vs_gen_depthE1_HE_PUH",
        //"reco_vs_gen_HB", "reco_vs_gen_HB_PUL", "reco_vs_gen_HB_PUH",

        //"aux_vs_gen",
        //"aux_vs_gen_depthG1_HE", "aux_vs_gen_depthE1_HE", "aux_vs_gen_depthG1_HB", "aux_vs_gen_depthE1_HB",

        //"aux_vs_gen_depthG1_HE", "aux_vs_gen_depthG1_HE_PUL", "aux_vs_gen_depthG1_HE_PUH",
        //"aux_vs_gen_depthE1_HE", "aux_vs_gen_depthE1_HE_PUL", "aux_vs_gen_depthE1_HE_PUH",
        //"aux_vs_gen_HB", "aux_vs_gen_HB_PUL", "aux_vs_gen_HB_PUH",

        //"DLPHIN_vs_gen",
        //"DLPHIN_vs_gen_depthG1_HE", "DLPHIN_vs_gen_depthE1_HE", "DLPHIN_vs_gen_depthG1_HB", "DLPHIN_vs_gen_depthE1_HB",
        //"reco_err",
        //"aux_err",

        //"reco_ratio_HB",
        //"reco_ratio_HB_genL", "reco_ratio_HB_genM",
        //"reco_ratio_depthG1_HE",
        //"reco_ratio_depthG1_HE_genL", "reco_ratio_depthG1_HE_genM",
        //"reco_ratio_depthE1_HE",
        //"reco_ratio_depthE1_HE_genL", "reco_ratio_depthE1_HE_genM",

        //"aux_ratio_HB",
        //"aux_ratio_HB_genL", "aux_ratio_HB_genM",
        //"aux_ratio_depthG1_HE",
        //"aux_ratio_depthG1_HE_genL", "aux_ratio_depthG1_HE_genM",
        //"aux_ratio_depthE1_HE",
        //"aux_ratio_depthE1_HE_genL", "aux_ratio_depthE1_HE_genM",

        //"raw_ratio_HB",
        //"raw_ratio_HB_genL", "raw_ratio_HB_genM",
        //"raw_ratio_depthG1_HE",
        //"raw_ratio_depthG1_HE_genL", "raw_ratio_depthG1_HE_genM",
        //"raw_ratio_depthE1_HE",
        //"raw_ratio_depthE1_HE_genL", "raw_ratio_depthE1_HE_genM",

        //"DLPHIN_ratio_HB",
        //"DLPHIN_ratio_HB_genL", "DLPHIN_ratio_HB_genM",
        //"DLPHIN_ratio_depthG1_HE",
        //"DLPHIN_ratio_depthG1_HE_genL", "DLPHIN_ratio_depthG1_HE_genM",
        //"DLPHIN_ratio_depthE1_HE",
        //"DLPHIN_ratio_depthE1_HE_genL", "DLPHIN_ratio_depthE1_HE_genM",
    };

    TFile *f1 = new TFile("results_temp/result_origin.root");
    //TFile *f1 = new TFile("results/result_UL_1TeV_pion_gun_PU.root");

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        if(plot_reco_vs_gen)
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
            h1->SetTitle(hist_name);
            //h1->SetTitle("");
            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            //h1->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            h1->GetYaxis()->SetTitle("reco energy [GeV]");
            //h1->GetYaxis()->SetRangeUser(ymin, ymax * 0.8);
            gPad->SetLogz();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + hist_name + ".png");
            //mycanvas->SaveAs("plots_temp/" + hist_name + ".pdf");

            TProfile *px = h1->ProfileX("px", 1, -1, "os");
            //px->BuildOptions(0, 0, "s");
            px->SetTitle(hist_name);
            //px->SetTitle("");
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
            //mycanvas->SaveAs("plots_temp/" + hist_name + "_profile.pdf");
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
            //SD_h->SetTitle("");
            SD_h->GetXaxis()->SetTitle("truth energy [GeV]");
            SD_h->GetYaxis()->SetTitle("#sigma_{reco energy} / reco energy");
            SD_h->GetXaxis()->SetRangeUser(xmin, xmax * 0.8);
            SD_h->GetYaxis()->SetRangeUser(0, 1);
            SD_h->Draw();
            mycanvas->SaveAs("plots_temp/" + hist_name + "_SD.png");
            //mycanvas->SaveAs("plots_temp/" + hist_name + "_SD.pdf");
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

        if(plot_ratio)
        {
            TString h1_name = hist_name + "_h";
            TH1F *h1 = (TH1F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            //gStyle->SetOptStat(kFALSE);

            h1->Draw("e");
            auto h1_mean = h1->GetMean();
            auto h1_std = h1->GetStdDev();
            auto max_bin = h1->GetMaximumBin();
            if(max_bin != 1)
            {h1_mean = h1->GetXaxis()->GetBinCenter(max_bin);}
            //std::cout << h1_mean << ", " << h1_std << std::endl;
            h1->Fit("gaus", "", "", h1_mean - h1_std, h1_mean + h1_std);
            auto f1 = h1->GetFunction("gaus");
            auto c = f1->GetParameter(0);
            auto mu = f1->GetParameter(1);
            auto sigma = f1->GetParameter(2);
            auto c_err = f1->GetParError(0);
            auto mu_err = f1->GetParError(1);
            auto sigma_err = f1->GetParError(2);
            //std::cout << c << ", " << mu << ", " << sigma << std::endl;

            std::stringstream s1;
            s1 << "#bf{#mu = " << std::setprecision(2) << mu << " #pm " << mu_err << "}";
            TString TS1 = s1.str();
            std::stringstream s2;
            s2 << "#bf{#sigma = " << std::setprecision(2) << sigma << " #pm " << sigma_err << "}";
            TString TS2 = s2.str();

            h1->GetXaxis()->SetTitle("reco / gen energy");
            h1->GetXaxis()->SetRangeUser(0.5,1.5);
            h1->SetTitle(hist_name);
            h1->GetYaxis()->SetTitle("number of channels");
            //gPad->SetLogz();

            TLine *l = new TLine(1, h1->GetMinimum(),1, h1->GetMaximum());
            l->SetLineColor(kBlack);
            l->Draw("same");

            TLatex latex;
            latex.SetTextSize(0.04);
            latex.SetNDC();
            latex.DrawLatex(0.6,0.75,TS1);
            latex.DrawLatex(0.6,0.7,TS2);

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
        }

        if(plot_1d)
        {
            TString h1_name = hist_name + "_h";
            TH1F *h1 = (TH1F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            //gStyle->SetOptStat(kFALSE);

            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            h1->GetXaxis()->SetRangeUser(0,100);
            h1->SetTitle(hist_name);
            //h1->GetYaxis()->SetTitle("|reco-truth|/truth");

            //h1->Rebin(4);
            //h1->Scale(1.0/h1->GetEntries());
            h1->SetLineColor(kRed);
            h1->Draw("hist");

            gPad-> SetLogy();
            gPad->SetGrid();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
        }

        if(plot_1d_fit)
        {
            TString h1_name = hist_name + "_h";
            TH1F *h1 = (TH1F*)f1->Get(h1_name);

            TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
            gStyle->SetOptStat(kFALSE);

            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            h1->GetXaxis()->SetRangeUser(-1,100);
            h1->SetTitle(hist_name);
            //h1->GetYaxis()->SetTitle("|reco-truth|/truth");

            h1->Sumw2();
            //h1->Rebin(4);
            //h1->Scale(1.0/h1->GetEntries());
            h1->SetLineColor(kRed);
            //h1->Draw("hist");

            TH1F* h2 = (TH1F*)h1->Clone();
            h2->Reset();
            for(int i = 1; i <= h1->GetNbinsX(); i++)
            {
                h2->SetBinContent(i,10);
            }
            h2->Divide(h1);
            h2->Draw("hist");

            //TF1 *f1 = new TF1("f1","tanh(x / [0]) + 0.000169981",0,100);
            TF1 *f1 = new TF1("f1","tanh([0] * x + [1]) + 0.000169981 - tanh([1])",0,100);
            //TF1 *f1 = new TF1("f1","tanh(x/2.1925E3) + 0.000169981",-1,100);
            //f1->Draw("same");

            //f1->SetParameters(100, 0.0003, 0.00005);
            h2->Fit("f1", "w");

            TF1 *f = (TF1*)h2->GetListOfFunctions()->FindObject("f1");
            if(f)
            {
                f->SetLineColor(kBlue);
                f->SetLineWidth(1);
                f->SetLineStyle(2); // 2 = "- - -"
                f->Draw("same");
            }

            gPad-> SetLogy();
            gPad->SetGrid();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

            for(int i = 1; i <= h2->GetNbinsX(); i++)
            {
                if((i-1)%50 == 0)
                {
                    std::cout << "(" << h2->GetBinLowEdge(i) << "," << h2->GetBinContent(i) << "), ";
                }
            }
        }
    }
    return 0;
}
