int Plot_compare()
{
    const bool plot_log = false;
    const bool shape_compare = true;
    std::vector<TString> hist_list = {"weighted_time_h", };//"arrival_time_h", "TS45_time_h"};
    std::vector<TString> hist2_list = hist_list;
    //hist2_list = {"DLPHIN_loss_depthE1_HE_genL_h"};

    TString f1_name = "result_UL_1TeV_pion_gun_noPU";
    TString f1_lag = "pion gun";
    TString f1_folder = "results/";
    TString f2_name = f1_name;
    f2_name = "result_UL_LLP_noPU";
    TString f2_lag = "LLP";
    TString f2_folder = "results/";
    TString hist_folder = "";

    TString x_title = "time [ns]";
    TString y_title = "channels";
    int rebin = 1;

    float x_min = -1;
    float x_max = -1;
    x_min = 0;
    x_max = 100;

    TFile *f1 = new TFile(f1_folder + f1_name + ".root");
    TFile *f2 = new TFile(f2_folder + f2_name + ".root");
    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);
        TString hist = hist_folder + hist_name;
        TString hist2_name = hist2_list.at(i);
        TString hist2 = hist_folder + hist2_name;

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
        gStyle->SetOptStat(kFALSE);

        mycanvas->SetLeftMargin(0.15);
        mycanvas->SetRightMargin(0.15);

        TPad *padup = new TPad("padup", "padup", 0, 0.3, 1, 1.0);
        padup -> SetBottomMargin(0);
        padup -> Draw();
        padup -> cd();

        if(plot_log) gPad-> SetLogy();

        TH1F *h1 = (TH1F*)f1->Get(hist);
        h1->Rebin(rebin);
        if(shape_compare)h1->Scale(1.0/h1->GetEntries());
        h1->SetLineColor(kRed);
        //h1->SetMarkerStyle(20);
        //h1->SetTitle("");
        h1->GetYaxis()->SetTitle(y_title);
        h1->GetYaxis()->SetTitleSize(0.045);
        h1->GetYaxis()->SetTitleOffset(0.8);

        h1->Draw("hist");
        h1->SetLineWidth(2);

        TH1F *h2 = (TH1F*)f2->Get(hist2);
        h2->Rebin(rebin);
        //if(shape_compare)h2->Scale(h1->GetEntries()/h2->GetEntries());
        if(shape_compare)h2->Scale(1.0/h2->GetEntries());

        if(x_min == -1) x_min = h1->GetXaxis()->GetXmin();
        if(x_max == -1) x_max = h1->GetXaxis()->GetXmax();
        h1->GetXaxis()->SetRangeUser(x_min, x_max);

        float y_max = max(h1->GetMaximum(), h2->GetMaximum()) * 1.2;
        h1->SetMaximum(y_max);
        if(plot_log)
        {
            h1->SetMinimum(1);
            h1->SetMaximum(y_max * 10);
        }

        TString f1_lag_temp = f1_lag;// + " (" + std::to_string(int(h1->Integral())) + ")";
        TString f2_lag_temp = f2_lag;// + " (" + std::to_string(int(h2->Integral())) + ")";

        h2->GetXaxis()->SetRangeUser(x_min, x_max);
        h2->SetLineColor(kBlue);
        h2->Draw("histsame");
        h2->SetLineWidth(2);

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
        ratio->GetXaxis()->SetLabelSize(0.08);
        ratio->GetXaxis()->SetTitle(x_title);
        ratio->GetXaxis()->SetTitleSize(0.1);

        ratio->GetYaxis()->SetLabelSize(0.08);
        ratio->GetYaxis()->SetTitle("ratio");
        ratio->GetYaxis()->SetTitleOffset(0.4);
        ratio->GetYaxis()->SetTitleSize(0.1);

        TLine *l=new TLine(x_min, 1.0, x_max, 1.0);
        l->SetLineColor(kRed);
        l->Draw();

        mycanvas->SaveAs("plots_temp/" + f1_name + "_" + hist_name + ".png");
    }

    return 0;
}
