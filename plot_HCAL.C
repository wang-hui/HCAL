int plot_HCAL()
{
	bool plot_reco_vs_gen_h = true;
	bool plot_ratio_h = true;

	TFile *f1 = new TFile("result_depth_test.root");

	if(plot_reco_vs_gen_h)
	{
		TString h1_name = "reco_vs_gen_depthG1_h";
		//h1_name = "reco_vs_gen_depthE1_HB_h";
		//h1_name = "reco_vs_gen_depthE1_HE_h";
		TH2F *h1 = (TH2F*)f1->Get(h1_name);

		TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
		gStyle->SetOptStat(kFALSE);

		h1->Draw("colz");
		h1->SetTitle("");
		h1->GetXaxis()->SetTitle("reco energy [GeV]");
		h1->GetYaxis()->SetTitle("truth energy [GeV]");
		gPad->SetLogz();

		mycanvas->SetLeftMargin(0.12);
		mycanvas->SetRightMargin(0.15);
		mycanvas->SaveAs("plots/" + h1_name + ".png");

		mycanvas->Clear();
		TProfile *px = h1->ProfileX("px");
		px->GetYaxis()->SetNdivisions(512);
		px->Draw();
		gPad->SetGrid();
		mycanvas->SaveAs("plots/" + h1_name + "_profile.png");
	}

	if(plot_ratio_h)
	{
		TString h1_name = "ratio_depthG1_h";
		//h1_name = "ratio_depthE1_HB_h";
		//h1_name = "ratio_depthE1_HE_h";
		TH1F *h1 = (TH1F*)f1->Get(h1_name);

		TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
		//gStyle->SetOptStat(kFALSE);

		h1->Draw();
		//h1->Fit("gaus","","",0.7,2);
		h1->GetXaxis()->SetTitle(h1->GetTitle());
		h1->SetTitle("");
		h1->GetYaxis()->SetTitle("number of channels");
		//gPad->SetLogz();

		mycanvas->SetLeftMargin(0.15);
		mycanvas->SetRightMargin(0.1);
		mycanvas->SaveAs("plots/" + h1_name + ".png");
	}

	return 0;
}
