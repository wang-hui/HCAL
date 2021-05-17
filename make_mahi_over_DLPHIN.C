int make_mahi_over_DLPHIN()
{
    bool plot_mahi_over_DLPHIN = true;

    bool px_SD = true;

    std::vector<TString> hist_list =
    {
        "HB",
        "HE",
    };

    TFile *f1 = new TFile("results/result_UL_1TeV_pion_gun_PU_1dHB_2dHE.root");
    TFile out_file("mahi_over_DLPHIN.root","RECREATE");

    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        if(plot_mahi_over_DLPHIN)
        {
            int ieta_first, ieta_last, depth_first, depth_last;
            if (hist_name == "HB")
            {
                ieta_first = 1;
                ieta_last = 16;
                depth_first = 1;
                depth_last = 2;
            }
            if (hist_name == "HE")
            {
                ieta_first = 16;
                ieta_last = 29;
                depth_first = 1;
                depth_last = 7;
            }

            for (int i = ieta_first; i <= ieta_last; i++)
            {
                for (int j = depth_first; j <= depth_last; j++)
                {
                    TString h1_name = "mahi_over_DLPHIN_" + hist_name + "_iEta_" + to_string(i) + "_depth_" + to_string(j) + "_h";
                    std::cout << h1_name << std::endl;
                    TH2F *h1 = (TH2F*)f1->Get(h1_name);

                    TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
                    gStyle->SetOptStat(kFALSE);

                    h1->Draw("colz");
                    h1->SetTitle(h1_name);
                    h1->GetXaxis()->SetTitle("DLPHIN energy [GeV]");
                    h1->GetYaxis()->SetTitle("MAHI / DLPHIN");
                    gPad->SetLogz();

                    mycanvas->SetLeftMargin(0.15);
                    mycanvas->SetRightMargin(0.15);
                    mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

                    TProfile *px = h1->ProfileX();
                    if(px_SD)px->BuildOptions(0, 0, "s");
                    px->SetTitle(h1_name);
                    px->GetYaxis()->SetTitle(h1->GetYaxis()->GetTitle());
                    px->SetLineColor(kRed);
                    px->SetMinimum(0);
                    //px->SetLineWidth(2);
                    //px->SetMarkerStyle(8);
                    px->Draw();
                    px->Write();
                    gPad->SetGrid();
                    mycanvas->SaveAs("plots_temp/" + h1_name + "_profile.png");
                }
            }
        }
    }

    out_file.Close();
    return 0;
}
