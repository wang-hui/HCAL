int plot_HCAL_compare()
{
    std::vector<TString> hist_list =
    {
        //"reco_vs_gen_depthG1_HE", "DLPHIN_vs_gen_depthG1_HE",
        //"reco_vs_gen_depthG1_HE_PUL", "reco_vs_gen_depthG1_HE_PUM", "reco_vs_gen_depthG1_HE_PUH",
        //"reco_vs_gen_depthE1_HE_PUL", "reco_vs_gen_depthE1_HE_PUM", "reco_vs_gen_depthE1_HE_PUH",
        //"reco_vs_gen_HB",
        //"reco_vs_gen_depthG1_HB", "DLPHIN_vs_gen_depthG1_HB",
        //"reco_vs_gen_depthE1_HB", "DLPHIN_vs_gen_depthE1_HB",
        //"reco_vs_gen_HB_PUL", "reco_vs_gen_HB_PUM", "reco_vs_gen_HB_PUH",

        //"aux_vs_gen_depthG1_HE",
        //"aux_vs_gen_depthG1_HE_PUL", "aux_vs_gen_depthG1_HE_PUM", "aux_vs_gen_depthG1_HE_PUH",
        //"aux_vs_gen_depthE1_HE",
        //"aux_vs_gen_depthE1_HE_PUL", "aux_vs_gen_depthE1_HE_PUM", "aux_vs_gen_depthE1_HE_PUH",
        //"aux_vs_gen_HB",
        //"aux_vs_gen_HB_PUL", "aux_vs_gen_HB_PUM", "aux_vs_gen_HB_PUH",

        //"raw_vs_gen_depthG1_HE",
        //"raw_vs_gen_depthG1_HE_PUL", "raw_vs_gen_depthG1_HE_PUM", "raw_vs_gen_depthG1_HE_PUH",
        //"raw_vs_gen_depthE1_HE",
        //"raw_vs_gen_depthE1_HE_PUL", "raw_vs_gen_depthE1_HE_PUM", "raw_vs_gen_depthE1_HE_PUH",
        //"raw_vs_gen_HB",
        //"raw_vs_gen_HB_PUL", "raw_vs_gen_HB_PUM", "raw_vs_gen_HB_PUH",

        //"reco_vs_gen_depthE1_HE_PUL", "raw_vs_gen_depthE1_HE_PUL", "DLPHIN_vs_gen_depthE1_HE_PUL",
        //"reco_vs_gen_depthE1_HE_PUH", "raw_vs_gen_depthE1_HE_PUH", "DLPHIN_vs_gen_depthE1_HE_PUH",
        //"reco_vs_gen_depthG1_HE_PUL", "raw_vs_gen_depthG1_HE_PUL", "DLPHIN_vs_gen_depthG1_HE_PUL",
        "reco_vs_gen_depthG1_HE_PUH", "raw_vs_gen_depthG1_HE_PUH", "DLPHIN_vs_gen_depthG1_HE_PUH",
    };

    std::vector<TString> file_list = {"result_UL_QCD_HT2000toInf_reco_1dHB_2dHE_test", "result_UL_QCD_HT2000toInf_reco_1dHB_2dHE_test", "result_UL_QCD_HT2000toInf_reco_1dHB_2dHE_test"};
    std::vector<TString> leg_list = {"MAHI", "M0", "DLPHIN"};
    //std::vector<TString> leg_list = {"low PU", "med PU", "high PU"};
    std::vector<int> color_list = {kBlack, kRed, kBlue};

    std::vector<TH1F*> SD_list;

    int rebin_x = 1;
    float px_scale = 0.1;
    float px_shift = 0;

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString file_name = file_list.at(i);
        TString hist_name = hist_list.at(i);

        TFile *f1 = new TFile("results/" + file_name + ".root");
        TH2F *h1 = (TH2F*)f1->Get(hist_name + "_h");

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
        h1->GetXaxis()->SetRangeUser(xmin, xmax);
        h1->GetYaxis()->SetTitle("reco energy [GeV]");
        h1->GetYaxis()->SetRangeUser(ymin, ymax);
        gPad->SetLogz();

        mycanvas->SetLeftMargin(0.15);
        mycanvas->SetRightMargin(0.15);
        mycanvas->SaveAs("plots_temp/" + file_name + "_" + hist_name + ".png");
        //mycanvas->SaveAs("plots_temp/" + file_name + "_"  + hist_name + ".pdf");

        TProfile *px = h1->ProfileX("px", 1, -1, "os");
        //px->BuildOptions(0, 0, "s");
        px->SetTitle(hist_name);
        //px->SetTitle("");
        //px->GetYaxis()->SetNdivisions(512);
        px->GetXaxis()->SetRangeUser(xmin + px_shift, xmax * px_scale);
        px->GetYaxis()->SetRangeUser(ymin, ymax * px_scale);
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

        TLine *l=new TLine(xmin + px_shift, ymin + px_shift, xmax * px_scale, ymax * px_scale);
        l->SetLineColor(kBlack);
        if(!hist_name.Contains("err"))
        {l->Draw("same");}

        mycanvas->SaveAs("plots_temp/" + file_name + "_"  + hist_name + "_profile.png");
        //mycanvas->SaveAs("plots_temp/" + file_name + "_"  + hist_name + "_profile.pdf");

        int nBins = px->GetXaxis()->GetNbins();
        TH1F *SD_h = new TH1F(hist_name + "_SD_h", hist_name + "_SD_h", nBins, xmin, xmax);
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
        SD_h->GetXaxis()->SetRangeUser(xmin + px_shift, xmax * px_scale);
        SD_h->GetYaxis()->SetRangeUser(0, 1.0);
        SD_h->Draw();
        SD_list.push_back(SD_h);
        mycanvas->SaveAs("plots_temp/" + file_name + "_"  + hist_name + "_SD.png");
        //mycanvas->SaveAs("plots_temp/" + file_name + "_"  + hist_name + "_SD.pdf");
    }

    TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
    gStyle->SetOptStat(kFALSE);

    TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);

    for(int i = 0; i < SD_list.size(); i++)
    {
        //std::cout << SD_list.at(i) << std::endl;
        auto my_hist = SD_list.at(i);
        my_hist->SetLineColor(color_list.at(i));
        if(i==0) my_hist->Draw();
        else my_hist->Draw("same");
        leg->AddEntry(my_hist,leg_list.at(i),"l");
    }
    leg->Draw("same");
    gPad->SetGrid();

    mycanvas->SaveAs("plots_temp/" + hist_list.at(0) + "_SD.png");
    //mycanvas->SaveAs("plots_temp/" + hist_list.at(0) + "_SD.pdf");

    return 0;
}
