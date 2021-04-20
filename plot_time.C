int plot_time()
{
    //TFile *f1 = new TFile("results/result_UL_LLP_noPU.root");
    //TFile *f1 = new TFile("results/result_UL_1TeV_pion_gun_noPU.root");
    TFile *f1 = new TFile("results_temp/result_pion_gun_noPU_origin.root");

    bool plot_weighted_time_vs_gen = false;
    bool plot_TS45_time_vs_gen = false;
    bool plot_arrival_time_vs_gen = false;
    bool plot_TS45_vs_arrival = true;

    bool plot_charge_vs_TS = false;
    bool plot_abnormal_charge_vs_TS = false;
    bool plot_weighted_time_vs_ieta = false;
    bool plot_TS45_time_vs_ieta = false;
    bool plot_arrival_time_vs_ieta = false;

    //TString sub_det = "HB";
    TString sub_det = "HE";
    TString hist_name = "weighted_time_vs_gen";
    TString x_title = "weighted time [ns]";
    TString y_title = "weighted time [ns]";

    float x_min = 0;
    float x_max = 50;
    float y_min = 10;
    float y_max = 40;

    bool plot_time_vs_gen = false;
    bool plot_time_vs_ieta = false;

    if(plot_weighted_time_vs_gen)
    {
        plot_time_vs_gen = true;
        hist_name = "weighted_time_vs_gen";
        y_title = "weighted time [ns]";
        x_title = "truth energy [GeV]";
        x_min = 0;
        x_max = 100;
    }

    if(plot_TS45_time_vs_gen)
    {
        plot_time_vs_gen = true;
        hist_name = "TS45_time_vs_gen";
        y_title = "TS45 time [ns]";
        x_title = "truth energy [GeV]";
        x_min = 0;
        x_max = 100;
    }

    if(plot_TS45_vs_arrival)
    {
        plot_time_vs_gen = true;
        hist_name = "TS45_vs_arrival_HB";
        y_title = "TS45 time [ns]";
        x_title = "arrival [ns]";
        x_min = 75;
        x_max = 100;
    }

    if(plot_arrival_time_vs_gen)
    {
        plot_time_vs_gen = true;
        hist_name = "arrival_time_vs_gen";
        y_title = "arrival time [ns]";
        x_title = "truth energy [GeV]";
        x_min = 0;
        x_max = 1000;
    }

    if(plot_charge_vs_TS)
    {
        plot_time_vs_gen = true;
        hist_name = "charge_vs_TS";
        y_title = "charge distribution";
        x_title = "TS";
        x_min = 0;
        x_max = 8;
    }

    if(plot_abnormal_charge_vs_TS)
    {
        plot_time_vs_gen = true;
        hist_name = "abnormal_charge_vs_TS";
        y_title = "charge distribution";
        x_title = "TS";
        x_min = 0;
        x_max = 8;
    }

    if(plot_weighted_time_vs_ieta)
    {
        plot_time_vs_ieta = true;
        hist_name = "weighted_time_";
        x_title = "weighted time [ns]";
    }

    if(plot_TS45_time_vs_ieta)
    {
        plot_time_vs_ieta = true;
        hist_name = "TS45_time_";
        x_title = "TS45 time [ns]";
        x_min = 76;
        x_max = 99;
        y_min = 80;
        y_max = 90;
    }

    if(plot_arrival_time_vs_ieta)
    {
        plot_time_vs_ieta = true;
        hist_name = "arrival_time_";
        x_title = "arrival time [ns]";
        x_min = 76;
        x_max = 99;
        y_min = 75;
        y_max = 90;
    }

    if(plot_time_vs_gen)
    {
        TString h1_folder = "";
        TString h1_name = hist_name + "_h";

        TH2F *h1 = (TH2F*)f1->Get(h1_folder + h1_name);

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
        gStyle->SetOptStat(kFALSE);

        h1->Draw("colz");
        h1->SetTitle(hist_name);
        h1->GetXaxis()->SetRangeUser(x_min,x_max);
        h1->GetXaxis()->SetTitle(x_title);
        h1->GetYaxis()->SetTitle(y_title);
        gPad->SetLogz();

        TProfile *px = h1->ProfileX();
        //px->BuildOptions(0, 0, "s");
        px->SetLineColor(kRed);
        px->Draw("histsame");

        float test_energy = 90;
        //std::cout << px->GetXaxis()->FindBin(test_energy) << std::endl;
        //std::cout << px->FindBin(test_energy) << std::endl;
        std::cout << "profile time at " << test_energy << " is " << px->GetBinContent(px->GetXaxis()->FindBin(test_energy)) << std::endl;

        mycanvas->SetLeftMargin(0.12);
        mycanvas->SetRightMargin(0.15);
        mycanvas->SaveAs("plots_temp/" + hist_name + ".png");
    }

    if(plot_time_vs_ieta)
    {
        int ieta_first, ieta_last;
        if (sub_det == "HB"){ieta_first = 1; ieta_last = 16;}
        if (sub_det == "HE"){ieta_first = 16; ieta_last = 29;}
        int depth_first, depth_last;
        if (sub_det == "HB"){depth_first = 1; depth_last = 2;}
        if (sub_det == "HE"){depth_first = 1; depth_last = 7;}

        auto mean_time_h=new TH2F(hist_name + sub_det,hist_name + sub_det,ieta_last+1-ieta_first,ieta_first,ieta_last+1, depth_last+1-depth_first, depth_first, depth_last+1);

        for (int i = ieta_first; i <= ieta_last; i++)
        {
            for (int j = depth_first; j <= depth_last; j++)
            {
                TString h1_name = hist_name + sub_det + "_iEta_" + to_string(i) + "_depth_" + to_string(j) + "_h";
                std::cout << h1_name << std::endl;
                TH1F *h1 = (TH1F*)f1->Get(h1_name);

                TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
                //gStyle->SetOptStat(kFALSE);
                gPad->SetLogy();
                //TGaxis::SetMaxDigits(2);

                //float mean_time = h1->GetBinLowEdge(h1->GetMaximumBin());
                int x = i + 1 - ieta_first;
                int y = j + 1 - depth_first;
                float mean_time_full = h1->GetMean();
                TH1F *h2 = (TH1F*)h1->Clone();
                h2->GetXaxis()->SetRangeUser(x_min,x_max);
                float mean_time_range = h2->GetMean();
                std::cout << "x = " << x << ", y = " << y << ", mean_time_full = " << mean_time_full << ", mean_time_range = " << mean_time_range << std::endl;
                mean_time_h->SetBinContent(x, y, mean_time_range);

                h1->Draw();
                h1->SetTitle(h1_name);
                h1->GetXaxis()->SetTitle(x_title);
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
        mean_time_h->SetMinimum(y_min);
        mean_time_h->SetMaximum(y_max);

        mycanvas->SetLeftMargin(0.12);
        mycanvas->SetRightMargin(0.15);
        mycanvas->SaveAs("plots_temp/" + hist_name + sub_det + ".png");
    }
    return 0;
}
