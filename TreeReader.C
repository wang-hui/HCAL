#define TreeReader_cxx
#include "TreeReader.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void TreeReader::Loop()
{
//   In a ROOT session, you can do:
//      root> .L TreeReader.C
//      root> TreeReader t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

void TreeReader::make_1d_plots() {
    auto OutputFileName = InputFileName;
    OutputFileName.ReplaceAll(".root", "_plots.root");
    OutputFileName.ReplaceAll("results/", "");
    auto OutputFile = new TFile(OutputFileName, "RECREATE");

    std::vector<float> EnergyBins = {100,0,200};
    std::vector<float> RatioBins = {100,0,2};

    // book histos in OutputFile gDirectory
    // recHits vs simHits
    auto Reco_vs_SimHit = BookTH2F("Reco_vs_SimHit", EnergyBins, EnergyBins);
    auto Reco_vs_SimHit_HB = BookTH2F("Reco_vs_SimHit_HB", EnergyBins, EnergyBins);
    auto Reco_vs_SimHit_ieta1516 = BookTH2F("Reco_vs_SimHit_ieta1516", EnergyBins, EnergyBins);
    auto Reco_vs_SimHit_HE = BookTH2F("Reco_vs_SimHit_HE", EnergyBins, EnergyBins);
    auto Reco_vs_SimHit_HE_ietaL = BookTH2F("Reco_vs_SimHit_HE_ietaL", EnergyBins, EnergyBins);
    auto Reco_vs_SimHit_HE_ietaH = BookTH2F("Reco_vs_SimHit_HE_ietaH", EnergyBins, EnergyBins);

    auto Aux_vs_SimHit = BookTH2F("Aux_vs_SimHit", EnergyBins, EnergyBins);
    auto Aux_vs_SimHit_HB = BookTH2F("Aux_vs_SimHit_HB", EnergyBins, EnergyBins);
    auto Aux_vs_SimHit_ieta1516 = BookTH2F("Aux_vs_SimHit_ieta1516", EnergyBins, EnergyBins);
    auto Aux_vs_SimHit_HE = BookTH2F("Aux_vs_SimHit_HE", EnergyBins, EnergyBins);
    auto Aux_vs_SimHit_HE_ietaL = BookTH2F("Aux_vs_SimHit_HE_ietaL", EnergyBins, EnergyBins);
    auto Aux_vs_SimHit_HE_ietaH = BookTH2F("Aux_vs_SimHit_HE_ietaH", EnergyBins, EnergyBins);

    auto Raw_vs_SimHit = BookTH2F("Raw_vs_SimHit", EnergyBins, EnergyBins);
    auto Raw_vs_SimHit_HB = BookTH2F("Raw_vs_SimHit_HB", EnergyBins, EnergyBins);
    auto Raw_vs_SimHit_ieta1516 = BookTH2F("Raw_vs_SimHit_ieta1516", EnergyBins, EnergyBins);
    auto Raw_vs_SimHit_HE = BookTH2F("Raw_vs_SimHit_HE", EnergyBins, EnergyBins);
    auto Raw_vs_SimHit_HE_ietaL = BookTH2F("Raw_vs_SimHit_HE_ietaL", EnergyBins, EnergyBins);
    auto Raw_vs_SimHit_HE_ietaH = BookTH2F("Raw_vs_SimHit_HE_ietaH", EnergyBins, EnergyBins);

    auto DLPHIN_vs_SimHit = BookTH2F("DLPHIN_vs_SimHit", EnergyBins, EnergyBins);
    auto DLPHIN_vs_SimHit_HB = BookTH2F("DLPHIN_vs_SimHit_HB", EnergyBins, EnergyBins);
    auto DLPHIN_vs_SimHit_ieta1516 = BookTH2F("DLPHIN_vs_SimHit_ieta1516", EnergyBins, EnergyBins);
    auto DLPHIN_vs_SimHit_HE = BookTH2F("DLPHIN_vs_SimHit_HE", EnergyBins, EnergyBins);
    auto DLPHIN_vs_SimHit_HE_ietaL = BookTH2F("DLPHIN_vs_SimHit_HE_ietaL", EnergyBins, EnergyBins);
    auto DLPHIN_vs_SimHit_HE_ietaH = BookTH2F("DLPHIN_vs_SimHit_HE_ietaH", EnergyBins, EnergyBins);

    // recHits/simHits vs simHits
    auto RecoRatio_vs_SimHit = BookTH2F("RecoRatio_vs_SimHit", EnergyBins, RatioBins);
    auto RecoRatio_vs_SimHit_HB = BookTH2F("RecoRatio_vs_SimHit_HB", EnergyBins, RatioBins);
    auto RecoRatio_vs_SimHit_ieta1516 = BookTH2F("RecoRatio_vs_SimHit_ieta1516", EnergyBins, RatioBins);
    auto RecoRatio_vs_SimHit_HE = BookTH2F("RecoRatio_vs_SimHit_HE", EnergyBins, RatioBins);
    auto RecoRatio_vs_SimHit_HE_ietaL = BookTH2F("RecoRatio_vs_SimHit_HE_ietaL", EnergyBins, RatioBins);
    auto RecoRatio_vs_SimHit_HE_ietaH = BookTH2F("RecoRatio_vs_SimHit_HE_ietaH", EnergyBins, RatioBins);

    auto AuxRatio_vs_SimHit = BookTH2F("AuxRatio_vs_SimHit", EnergyBins, RatioBins);
    auto AuxRatio_vs_SimHit_HB = BookTH2F("AuxRatio_vs_SimHit_HB", EnergyBins, RatioBins);
    auto AuxRatio_vs_SimHit_ieta1516 = BookTH2F("AuxRatio_vs_SimHit_ieta1516", EnergyBins, RatioBins);
    auto AuxRatio_vs_SimHit_HE = BookTH2F("AuxRatio_vs_SimHit_HE", EnergyBins, RatioBins);
    auto AuxRatio_vs_SimHit_HE_ietaL = BookTH2F("AuxRatio_vs_SimHit_HE_ietaL", EnergyBins, RatioBins);
    auto AuxRatio_vs_SimHit_HE_ietaH = BookTH2F("AuxRatio_vs_SimHit_HE_ietaH", EnergyBins, RatioBins);

    auto RawRatio_vs_SimHit = BookTH2F("RawRatio_vs_SimHit", EnergyBins, RatioBins);
    auto RawRatio_vs_SimHit_HB = BookTH2F("RawRatio_vs_SimHit_HB", EnergyBins, RatioBins);
    auto RawRatio_vs_SimHit_ieta1516 = BookTH2F("RawRatio_vs_SimHit_ieta1516", EnergyBins, RatioBins);
    auto RawRatio_vs_SimHit_HE = BookTH2F("RawRatio_vs_SimHit_HE", EnergyBins, RatioBins);
    auto RawRatio_vs_SimHit_HE_ietaL = BookTH2F("RawRatio_vs_SimHit_HE_ietaL", EnergyBins, RatioBins);
    auto RawRatio_vs_SimHit_HE_ietaH = BookTH2F("RawRatio_vs_SimHit_HE_ietaH", EnergyBins, RatioBins);

    auto DLPHINRatio_vs_SimHit = BookTH2F("DLPHINRatio_vs_SimHit", EnergyBins, RatioBins);
    auto DLPHINRatio_vs_SimHit_HB = BookTH2F("DLPHINRatio_vs_SimHit_HB", EnergyBins, RatioBins);
    auto DLPHINRatio_vs_SimHit_ieta1516 = BookTH2F("DLPHINRatio_vs_SimHit_ieta1516", EnergyBins, RatioBins);
    auto DLPHINRatio_vs_SimHit_HE = BookTH2F("DLPHINRatio_vs_SimHit_HE", EnergyBins, RatioBins);
    auto DLPHINRatio_vs_SimHit_HE_ietaL = BookTH2F("DLPHINRatio_vs_SimHit_HE_ietaL", EnergyBins, RatioBins);
    auto DLPHINRatio_vs_SimHit_HE_ietaH = BookTH2F("DLPHINRatio_vs_SimHit_HE_ietaH", EnergyBins, RatioBins);

    auto TotEntries = fChain->GetEntries();
    //TotEntries = 100;

    for (int iEntry=0; iEntry<TotEntries; iEntry++) {
        fChain->GetEntry(iEntry);
        if (iEntry%10000000 == 0) std::cout << "Processed " << iEntry << " entries" << std::endl;
        //std::cout << RawIdVec->size() << std::endl;

        for (int iHit=0; iHit<RawIdVec->size(); iHit++) {
            auto SimHitEnergy = SimHitEnergyVec->at(iHit);

            auto RecoEnergy = RecoEnergyVec->at(iHit);
            auto AuxEnergy = AuxEnergyVec->at(iHit);
            auto RawEnergy = RawEnergyVec->at(iHit);
            auto DLPHINEnergy = DLPHINEnergyVec->at(iHit);
    
            auto RespCorr = RespCorrVec->at(iHit);
            auto Ieta = IetaVec->at(iHit);
    
            auto RecoEnergy_raw = RecoEnergy / RespCorr;
            auto AuxEnergy_raw = AuxEnergy / RespCorr;
            auto RawEnergy_raw = RawEnergy / RespCorr;
            auto IetaAbs = abs(Ieta);
    
            // recHits vs simHits
            Reco_vs_SimHit->Fill(SimHitEnergy, RecoEnergy_raw);
            Aux_vs_SimHit->Fill(SimHitEnergy, AuxEnergy_raw);
            Raw_vs_SimHit->Fill(SimHitEnergy, RawEnergy_raw);
            DLPHIN_vs_SimHit->Fill(SimHitEnergy, DLPHINEnergy);
    
            if (IetaAbs <= 14) {
                Reco_vs_SimHit_HB->Fill(SimHitEnergy, RecoEnergy_raw);
                Aux_vs_SimHit_HB->Fill(SimHitEnergy, AuxEnergy_raw);
                Raw_vs_SimHit_HB->Fill(SimHitEnergy, RawEnergy_raw);
                DLPHIN_vs_SimHit_HB->Fill(SimHitEnergy, DLPHINEnergy);
            }
            else if (IetaAbs <= 16) {
                Reco_vs_SimHit_ieta1516->Fill(SimHitEnergy, RecoEnergy_raw);
                Aux_vs_SimHit_ieta1516->Fill(SimHitEnergy, AuxEnergy_raw);
                Raw_vs_SimHit_ieta1516->Fill(SimHitEnergy, RawEnergy_raw);
                DLPHIN_vs_SimHit_ieta1516->Fill(SimHitEnergy, DLPHINEnergy);
            }
            else if (IetaAbs <= 29) {
                Reco_vs_SimHit_HE->Fill(SimHitEnergy, RecoEnergy_raw);
                Aux_vs_SimHit_HE->Fill(SimHitEnergy, AuxEnergy_raw);
                Raw_vs_SimHit_HE->Fill(SimHitEnergy, RawEnergy_raw);
                DLPHIN_vs_SimHit_HE->Fill(SimHitEnergy, DLPHINEnergy);
    
                if (IetaAbs <= 26) {
                    Reco_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, RecoEnergy_raw);
                    Aux_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, AuxEnergy_raw);
                    Raw_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, RawEnergy_raw);
                    DLPHIN_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, DLPHINEnergy);
                }
                else {
                    Reco_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, RecoEnergy_raw);
                    Aux_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, AuxEnergy_raw);
                    Raw_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, RawEnergy_raw);
                    DLPHIN_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, DLPHINEnergy);
                }
            }
    
            // recHits/simHits vs simHits
            if (SimHitEnergy > 0) {
                RecoRatio_vs_SimHit->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                AuxRatio_vs_SimHit->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                RawRatio_vs_SimHit->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                DLPHINRatio_vs_SimHit->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
    
                if (IetaAbs <= 14) {
                    RecoRatio_vs_SimHit_HB->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                    AuxRatio_vs_SimHit_HB->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                    RawRatio_vs_SimHit_HB->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                    DLPHINRatio_vs_SimHit_HB->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
                }
                else if (IetaAbs <= 16) {
                    RecoRatio_vs_SimHit_ieta1516->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                    AuxRatio_vs_SimHit_ieta1516->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                    RawRatio_vs_SimHit_ieta1516->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                    DLPHINRatio_vs_SimHit_ieta1516->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
                }
                else if (IetaAbs <= 29) {
                    RecoRatio_vs_SimHit_HE->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                    AuxRatio_vs_SimHit_HE->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                    RawRatio_vs_SimHit_HE->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                    DLPHINRatio_vs_SimHit_HE->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
    
                    if (IetaAbs <= 26) {
                        RecoRatio_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                        AuxRatio_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                        RawRatio_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                        DLPHINRatio_vs_SimHit_HE_ietaL->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
                    }
                    else {
                        RecoRatio_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, RecoEnergy_raw / SimHitEnergy);
                        AuxRatio_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, AuxEnergy_raw / SimHitEnergy);
                        RawRatio_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, RawEnergy_raw / SimHitEnergy);
                        DLPHINRatio_vs_SimHit_HE_ietaH->Fill(SimHitEnergy, DLPHINEnergy / SimHitEnergy);
                    }
                }
            }
        }
    }

    OutputFile->Write();
    OutputFile->Close();
}
