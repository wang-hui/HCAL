int plot_time()
{
    bool plot_time_vs_gen = true;
    bool plot_ieta_vs_depth = false;

    //TString hist_name = "HB";
    TString hist_name = "HE";

    TFile *f1 = new TFile("gen_hist.root");

    if(plot_time_vs_gen)
    {
        TString h1_folder = "myAna/";
        TString h1_name = "channel_energy_vs_time_h";

        TH2F *h1 = (TH2F*)f1->Get(h1_folder + h1_name);

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
        gStyle->SetOptStat(kFALSE);

        h1->Draw("colz");
        h1->SetTitle(h1_name);
        //h1->GetXaxis()->SetRangeUser(0,100);
        h1->GetXaxis()->SetTitle("truth energy [GeV]");
        h1->GetYaxis()->SetTitle("weighted time [ns]");
        gPad->SetLogz();

        mycanvas->SetLeftMargin(0.12);
        mycanvas->SetRightMargin(0.15);
        mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
    }

    if(plot_ieta_vs_depth)
    {
        int ieta_first, ieta_last;
        if (hist_name == "HB"){ieta_first = 1; ieta_last = 16;}
        if (hist_name == "HE"){ieta_first = 16; ieta_last = 29;}
        int depth_first, depth_last;
        if (hist_name == "HB"){depth_first = 1; depth_last = 5;}
        if (hist_name == "HE"){depth_first = 1; depth_last = 8;}

        auto mean_time_h=new TH2F("mean_time_h","mode time ieta vs depth",ieta_last+1-ieta_first,ieta_first,ieta_last+1, depth_last+1-depth_first, depth_first, depth_last+1);

        for (int i = ieta_first; i <= ieta_last; i++)
        {
            for (int j = depth_first; j <= depth_last; j++)
            {
                TString h1_name = "weighted_time_" + hist_name + "_iEta_" + to_string(i) + "_depth_" + to_string(j) + "_h";
                std::cout << h1_name << std::endl;
                TH1F *h1 = (TH1F*)f1->Get(h1_name);

                TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
                //gStyle->SetOptStat(kFALSE);
                gPad->SetLogy();
                //TGaxis::SetMaxDigits(2);

                float mean_time = h1->GetBinLowEdge(h1->GetMaximumBin());
                int x = i + 1 - ieta_first;
                int y = j + 1 - depth_first;
                std::cout << "x = " << x << ", y = " << y << ", mean time = " << mean_time << std::endl;
                mean_time_h->SetBinContent(x, y, mean_time);

                h1->Draw();
                h1->SetTitle(h1_name);
                h1->GetXaxis()->SetTitle("weighted simHit time [ns]");
                h1->GetYaxis()->SetTitle("number of channels");

                mycanvas->SetLeftMargin(0.12);
                mycanvas->SetRightMargin(0.15);
                mycanvas->SaveAs("plots_temp/" + h1_name + ".png");
            }
        }

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);

        gStyle->SetOptStat(kFALSE);
        //mean_time_h->SetMaximum(mean_time_h->GetMaximum() * 1.2);
        //mean_time_h->SetMinimum(0);
        mean_time_h->GetXaxis()->SetTitle("ieta");
        mean_time_h->GetYaxis()->SetTitle("depth");
        mean_time_h->Draw("colz");

        mycanvas->SetLeftMargin(0.12);
        mycanvas->SetRightMargin(0.15);
        mycanvas->SaveAs("plots_temp/mean_time_" + hist_name + ".png");
    }
    return 0;
}
