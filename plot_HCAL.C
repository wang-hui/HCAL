int plot_HCAL()
{
    bool plot_reco_vs_gen_ieta = false;
    bool plot_reco_vs_gen = true;
    bool plot_ratio = false;
    bool plot_1d = false;
    bool plot_1d_fit = false;

    int rebin_x = 1;
    float x_scale = 0.12;
    bool do_profile_study = false;

    std::vector<TString> hist_list =
    {
        //"gen", "genG0",
        //"genG0",

        //"reco_vs_gen",
        //"reco_vs_gen_depthG1_HE", "reco_vs_gen_depthE1_HE", "reco_vs_gen_depthG1_HB", "reco_vs_gen_depthE1_HB"
        //"reco_vs_gen_depthG1_HE", "reco_vs_gen_depthG1_HE_1_pulse", "reco_vs_gen_depthG1_HE_8_pulse",
        //"reco_vs_gen_depthE1_HE", "reco_vs_gen_depthE1_HE_1_pulse", "reco_vs_gen_depthE1_HE_8_pulse",
        //"reco_vs_gen_depthG1_HB", "reco_vs_gen_depthG1_HB_1_pulse", "reco_vs_gen_depthG1_HB_8_pulse",
        //"reco_vs_gen_depthE1_HB", "reco_vs_gen_depthE1_HB_1_pulse", "reco_vs_gen_depthE1_HB_8_pulse",

        //"reco_vs_gen_depthG1_HE", //"reco_vs_gen_depthG1_HE_PUL", "reco_vs_gen_depthG1_HE_PUH",
        //"reco_vs_gen_depthE1_HE", "reco_vs_gen_depthE1_HE_PUL", "reco_vs_gen_depthE1_HE_PUH",
        //"reco_vs_gen_HB", "reco_vs_gen_HB_PUL", "reco_vs_gen_HB_PUH",

        //"aux_vs_gen",
        //"aux_vs_gen_depthG1_HE", "aux_vs_gen_depthE1_HE", "aux_vs_gen_depthG1_HB", "aux_vs_gen_depthE1_HB",

        //"aux_vs_gen_depthG1_HE", //"aux_vs_gen_depthG1_HE_PUL", "aux_vs_gen_depthG1_HE_PUH",
        //"aux_vs_gen_depthE1_HE", "aux_vs_gen_depthE1_HE_PUL", "aux_vs_gen_depthE1_HE_PUH",
        //"aux_vs_gen_HB", "aux_vs_gen_HB_PUL", "aux_vs_gen_HB_PUH",

        //"raw_vs_gen_depthG1_HE", //"raw_vs_gen_depthG1_HE_PUL", "raw_vs_gen_depthG1_HE_PUH",
        //"raw_vs_gen_depthE1_HE", "raw_vs_gen_depthE1_HE_PUL", "raw_vs_gen_depthE1_HE_PUH",
        //"raw_vs_gen_HB", "raw_vs_gen_HB_PUL", "raw_vs_gen_HB_PUH",

        //"DLPHIN_vs_gen",
        //"DLPHIN_vs_mahi_HB", "DLPHIN_vs_mahi_depthG1_HE"
        //"DLPHIN_vs_mahi_depthE1_HE"
        //"DLPHIN_vs_gen_depthG1_HE", //"DLPHIN_vs_gen_depthE1_HE", "DLPHIN_vs_gen_depthG1_HB", "DLPHIN_vs_gen_depthE1_HB",
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
        //"weighted_time", "TS45_time", "arrival_time"
 
        "reco_vs_gen_depthE1_HB_iEta_15", "reco_vs_gen_depthG1_HB_iEta_15",
        "reco_vs_gen_depthE1_HB_iEta_16", "reco_vs_gen_depthG1_HB_iEta_16",
        "DLPHIN_vs_gen_depthE1_HB_iEta_15", "DLPHIN_vs_gen_depthG1_HB_iEta_15",
        "DLPHIN_vs_gen_depthE1_HB_iEta_16", "DLPHIN_vs_gen_depthG1_HB_iEta_16",

        //"DLPHIN_vs_gen_sum"
        //"depthE1_HB"
    };

    TFile *f1 = new TFile("results/UL_p1TeV_pion_gun_1d_training.root");
    //TFile *f1 = new TFile("results/result_UL_1TeV_pion_gun_PU_1dHB_2dHE.root");
    //TFile *f1 = new TFile("results/UL_RSGravitonToQuarkQuark_kMpl01_M_2000_RECO_DLPHIN_scaled_unfinished.root");

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
            h1->RebinX(rebin_x);
            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            h1->GetXaxis()->SetRangeUser(xmin, xmax * x_scale);
            h1->GetYaxis()->SetTitle("reco energy [GeV]");
            h1->GetYaxis()->SetRangeUser(ymin, ymax * x_scale);
            gPad->SetLogz();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.15);
            //mycanvas->SaveAs("plots_temp/" + hist_name + ".png");
            //mycanvas->SaveAs("plots_temp/" + hist_name + ".pdf");

            TProfile *px = h1->ProfileX("px", 1, -1, "os");
            //px->BuildOptions(0, 0, "s");
            px->SetTitle(hist_name);
            //px->SetTitle("");
            //px->GetYaxis()->SetNdivisions(512);
            px->GetXaxis()->SetRangeUser(xmin, xmax * x_scale);
            px->GetYaxis()->SetRangeUser(ymin, ymax * x_scale);
            px->SetLineColor(kRed);
            //px->SetLineWidth(2);
            //px->SetMarkerStyle(8);
            px->GetXaxis()->SetTitle("truth energy [GeV]");
            px->GetYaxis()->SetTitle("reco energy [GeV]");
            px->Draw("same");
            gPad->SetGrid();
            px->Fit("pol1", "w");
            TF1 *f = (TF1*)px->GetListOfFunctions()->FindObject("pol1");
            if(f)
            {
                f->SetLineColor(kBlue);
                f->SetLineWidth(1);
                f->SetLineStyle(2); // 2 = "- - -"
            }

            TLine *l=new TLine(xmin, ymin, xmax * x_scale, ymax * x_scale);
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
                SD_h->SetBinContent(i, error);
                //std::cout << i << ", " << error << ", " << center << ", " << SD_h->GetBinContent(i) << std::endl;
            }
            //SD_h->SetTitle("");
            SD_h->GetXaxis()->SetTitle("truth energy [GeV]");
            SD_h->GetYaxis()->SetTitle("#sigma_{reco energy}");
            SD_h->GetXaxis()->SetRangeUser(xmin, xmax * x_scale);
            //SD_h->GetYaxis()->SetRangeUser(0, 1);
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

            h1->GetXaxis()->SetTitle("time [GeV]");
            h1->GetXaxis()->SetRangeUser(0,100);
            h1->SetTitle(hist_name);
            h1->GetYaxis()->SetTitle("number of channels");

            //h1->Rebin(4);
            //h1->Scale(1.0/h1->GetEntries());
            h1->SetLineColor(kRed);
            h1->Draw("hist");

            //gPad->SetLogy();
            //gPad->SetGrid();

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

            int tot_bins = h1->GetNbinsX();
            float last_bin_content = h1->GetBinContent(tot_bins);
            std::cout << "tot " << tot_bins << " bins, last bin content " << last_bin_content << std::endl;

            for(int i = 1; i <= tot_bins; i++)
            {
                h2->SetBinContent(i,last_bin_content);
            }
            h2->Divide(h1);
            h2->Draw("hist");
            h2->SetTitle("HE_depth1_weight");
            h2->SetName("HE_depth1_weight");

            TFile out_file("HE_depth1_weight.root","RECREATE");
            h2->Write();
            out_file.Close();

            float fit_min = 0;
            std::ostringstream intercept;
            intercept.precision(10);
            intercept << std::fixed << h2->GetBinContent(h2->FindBin(fit_min));
            TString func_string = "tanh([0] * (x-" + std::to_string(fit_min) + ") + [1]) - tanh([1]) + " + intercept.str();
            std::cout << func_string << std::endl;

            //TF1 *f1 = new TF1("f1","tanh(0.0154716 * x - 1.4678) + 0.899238",0,100);  // fit "w"
            //TF1 *f1 = new TF1("f1","tanh(0.0450402 * x - 3.14331) + 0.996366",0,100);  // fit ""
            TF1 *f1 = new TF1("f1",func_string,fit_min,100);
            //f1->Draw("same");
            std::cout << "fit at 0 = " << f1->Eval(0) << std::endl;

            //f1->SetParameters(100, 0.0003, 0.00005);
            //h2->Fit("f1");
            h2->Fit("f1", "w");

            TF1 *f = (TF1*)h2->GetListOfFunctions()->FindObject("f1");
            if(f)
            {
                f->SetLineColor(kBlue);
                f->SetLineWidth(1);
                f->SetLineStyle(2); // 2 = "- - -"
                f->Draw("same");
            }

            TLegend* leg = new TLegend(0.6,0.5,0.9,0.7);
            leg->AddEntry(h2,"1/entries","l");
            leg->AddEntry(f,"fit","l");
            leg->Draw("same");

            gPad-> SetLogy();
            gPad->SetGrid();

            mycanvas->SetLeftMargin(0.15);
            mycanvas->SetRightMargin(0.1);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

            for(int i = 1; i <= tot_bins; i++)
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
