int plot_HCAL()
{
    bool plot_reco_vs_gen_ieta = false;
    bool plot_reco_vs_gen_h = true;
    bool plot_ratio_h = false;
    bool plot_err_h = false;

    bool do_profile_study = false;

    //TString hist_name = "depthG1_HE";
    //TString hist_name = "depthE1_HE";
    //TString hist_name = "depthG1_HB";
    //TString hist_name = "depthE1_HB_ietaS15";
    //TString hist_name = "depthE1_HB";
    //TString hist_name = "reco";
    TString hist_name = "aux";
    //TString hist_name = "reco_err";
    //TString hist_name = "aux_err";

    TFile *f1 = new TFile("result_origin.root");

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

            h1->Draw("colz");
            h1->SetTitle(h1_name);
            h1->GetXaxis()->SetTitle("truth energy [GeV]");
            h1->GetYaxis()->SetTitle("reco energy [GeV]");
            gPad->SetLogz();

            mycanvas->SetLeftMargin(0.12);
            mycanvas->SetRightMargin(0.15);
            mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

            mycanvas->Clear();
            TProfile *px = h1->ProfileX("px");
            px->SetTitle(h1_name);
            //px->GetYaxis()->SetNdivisions(512);
            px->GetYaxis()->SetRangeUser(h1->GetYaxis()->GetXmin(), h1->GetYaxis()->GetXmax());
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

            TLine *l=new TLine(h1->GetXaxis()->GetXmin(), h1->GetYaxis()->GetXmin(),h1->GetXaxis()->GetXmax(), h1->GetYaxis()->GetXmax());
            l->SetLineColor(kBlack);
            l->Draw("same");

            mycanvas->SaveAs("plots_temp/" + h1_name + "_profile.png");
        }
    }

    if(plot_reco_vs_gen_h)
    {
        TString h1_name = hist_name + "_vs_gen_h";
        if(!hist_name.Contains("reco") && !hist_name.Contains("aux"))
        {h1_name = "reco_vs_gen_" + hist_name + "_h";}

        TH2F *h1 = (TH2F*)f1->Get(h1_name);

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
        gStyle->SetOptStat(kFALSE);

        h1->Draw("colz");
        h1->SetTitle(h1_name);
        h1->GetXaxis()->SetTitle("truth energy [GeV]");
        h1->GetYaxis()->SetTitle("reco energy [GeV]");
        gPad->SetLogz();

        mycanvas->SetLeftMargin(0.12);
        mycanvas->SetRightMargin(0.15);
        if (do_profile_study)
        {
            //h1->GetXaxis()->SetRangeUser(0,20);
            //h1->GetYaxis()->SetRangeUser(0,20);
        }
        mycanvas->SaveAs("plots_temp/" + h1_name + ".png");

        mycanvas->Clear();
        TProfile *px = h1->ProfileX("px");
        px->SetTitle(h1_name);
        //px->GetYaxis()->SetNdivisions(512);
        px->GetYaxis()->SetRangeUser(h1->GetYaxis()->GetXmin(), h1->GetYaxis()->GetXmax());
        px->SetLineColor(kRed);
        //px->SetLineWidth(2);
        //px->SetMarkerStyle(8);
        px->Draw();
        gPad->SetGrid();
        if (do_profile_study)
        {
            for (int i = 1; i<=15; i++)
            {
                std::cout << px->GetBinContent(i) << ", " << px->GetBinError(i) << std::endl;
            }
        }
        px->Fit("pol1", "w");
        TF1 *f = (TF1*)px->GetListOfFunctions()->FindObject("pol1");
        if(f)
        {
            f->SetLineColor(kBlue);
            f->SetLineWidth(1);
            f->SetLineStyle(2); // 2 = "- - -"
        }

        TLine *l=new TLine(h1->GetXaxis()->GetXmin(), h1->GetYaxis()->GetXmin(),h1->GetXaxis()->GetXmax(), h1->GetYaxis()->GetXmax());
        l->SetLineColor(kBlack);
        if(!hist_name.Contains("err"))
        {l->Draw("same");}

        mycanvas->SaveAs("plots_temp/" + h1_name + "_profile.png");
        if (do_profile_study)
        {
            TProfile *py = h1->ProfileY("py");
            py->Draw();
            mycanvas->SaveAs("plots_temp/" + h1_name + "_profileY.png");
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

    return 0;
}
