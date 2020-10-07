int Plot_compare()
{
    const bool plot_log = false;
    const bool shape_compare = true;
    std::vector<TString> hist_list = {"respCorr_depthG1_HB_h", "respCorr_depthG1_HE_h", "respCorr_depthE1_HB_h", "respCorr_depthE1_HE_h"};
    std::vector<TString> hist2_list = hist_list;
    //hist2_list = {""};

    TString f1_name = "result_origin";
    TString f1_lag = "MC";
    TString f1_folder = "results_temp/";
    TString f2_name = f1_name;
    f2_name = "result_data_origin";
    TString f2_lag = "data";
    TString f2_folder = "results_temp/";
    TString hist_folder = "";

    int rebin = 1; 

    TFile *f1 = new TFile(f1_folder + f1_name + ".root");
    TFile *f2 = new TFile(f2_folder + f2_name + ".root");
    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);
        TString hist = hist_folder + hist_name;
        TString hist2_name = hist2_list.at(i);
        TString hist2 = hist_folder + hist2_name;

        TCanvas* mycanvas = new TCanvas();
        gStyle->SetOptStat(kFALSE);
        TPad *padup = new TPad("padup", "padup", 0, 0.3, 1, 1.0);
        padup -> SetBottomMargin(0);
        padup -> Draw();
        padup -> cd();

        if(plot_log) gPad-> SetLogy();

        TH1F *h1 = (TH1F*)f1->Get(hist);
        h1->Rebin(rebin);
        //if(shape_compare)h1->Scale(1.0/h1->GetEntries());
        h1->SetLineColor(kRed);
        h1->SetMarkerStyle(20);
        //h1->SetTitle("");
        h1->Draw();
        h1->SetLineWidth(2);

        TH1F *h2 = (TH1F*)f2->Get(hist2);
        h2->Rebin(rebin);
        if(shape_compare)h2->Scale(h1->GetEntries()/h2->GetEntries());

        float y_max = max(h1->GetMaximum(), h2->GetMaximum()) * 1.2;
        h1->SetMaximum(y_max);
        if(plot_log) h1->SetMaximum(y_max * 10);

        TString f1_lag_temp = f1_lag;// + " (" + std::to_string(int(h1->Integral())) + ")";
        TString f2_lag_temp = f2_lag;// + " (" + std::to_string(int(h2->Integral())) + ")";

        h2->SetLineColor(kBlue);
        h2->Draw("histsame");
        h2->SetLineWidth(3);

        TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
        leg->AddEntry(h1,f1_lag_temp,"l");
        leg->AddEntry(h2,f2_lag_temp,"l");
        leg->Draw("same");

        mycanvas -> cd();
        TPad *paddown = new TPad("paddown", "paddown", 0, 0, 1, 0.3);
        paddown -> SetTopMargin(0);
        paddown -> SetBottomMargin(0.3);
        paddown -> SetGrid();
        paddown -> Draw();
        paddown -> cd();

        TH1F *ratio = (TH1F*)h1->Clone();
        ratio->SetTitle("");
        ratio->Sumw2();
        ratio->Divide(h2);
        //ratio->SetLineColor(kBlack);;
        ratio->Draw("p");
        ratio->SetMinimum(0);
        ratio->SetMaximum(2);
        ratio->GetXaxis()->SetLabelSize(0.12);
        ratio->GetYaxis()->SetLabelSize(0.08);

        TLine *l=new TLine(ratio->GetXaxis()->GetXmin(), 1.0,ratio->GetXaxis()->GetXmax(), 1.0);
        l->SetLineColor(kRed);
        l->Draw();

        mycanvas->SaveAs("plots_temp/" + f1_name + "_" + hist_name + ".png");
    }

    return 0;
}
