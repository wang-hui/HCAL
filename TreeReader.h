//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Apr 14 13:12:55 2022 by ROOT version 6.14/09
// from TTree DLPHINTree/DLPHINTree
// found on file: UL_p1TeV_pion_gun_RAW_noPU_TTree.root
//////////////////////////////////////////////////////////

#ifndef TreeReader_h
#define TreeReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class TreeReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           Run;
   Int_t           Event;
   Int_t           LS;
   Float_t         PU;
   vector<int>     *SubdetVec;
   vector<int>     *IetaVec;
   vector<int>     *IphiVec;
   vector<int>     *DepthVec;
   vector<unsigned int> *RawIdVec;
   vector<vector<float> > *RawChargesVec;
   vector<vector<float> > *PedestalsVec;
   vector<vector<float> > *TotNoisesVec;
   vector<float>   *RecoEnergyVec;
   vector<float>   *AuxEnergyVec;
   vector<float>   *RawEnergyVec;
   vector<float>   *DLPHINEnergyVec;
   vector<unsigned int> *FlagVec;
   vector<float>   *Chi2Vec;
   vector<float>   *RawGainVec;
   vector<float>   *RespCorrVec;
   vector<float>   *DLPHINRespCorrVec;
   vector<int>     *nSimHitsVec;
   vector<float>   *SimHitEnergyVec;
   vector<float>   *SimHitTimeVec;
   vector<float>   *ArrivalTimeVec;

   // List of branches
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_LS;   //!
   TBranch        *b_PU;   //!
   TBranch        *b_SubdetVec;   //!
   TBranch        *b_IetaVec;   //!
   TBranch        *b_IphiVec;   //!
   TBranch        *b_DepthVec;   //!
   TBranch        *b_RawIdVec;   //!
   TBranch        *b_RawChargesVec;   //!
   TBranch        *b_PedestalsVec;   //!
   TBranch        *b_TotNoisesVec;   //!
   TBranch        *b_RecoEnergyVec;   //!
   TBranch        *b_AuxEnergyVec;   //!
   TBranch        *b_RawEnergyVec;   //!
   TBranch        *b_DLPHINEnergyVec;   //!
   TBranch        *b_FlagVec;   //!
   TBranch        *b_Chi2Vec;   //!
   TBranch        *b_RawGainVec;   //!
   TBranch        *b_RespCorrVec;   //!
   TBranch        *b_DLPHINRespCorrVec;   //!
   TBranch        *b_nSimHitsVec;   //!
   TBranch        *b_SimHitEnergyVec;   //!
   TBranch        *b_SimHitTimeVec;   //!
   TBranch        *b_ArrivalTimeVec;   //!

   TreeReader(TString InputFileName_, TTree *tree=0);
   virtual ~TreeReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

    // user functions
    struct channel_info {
        int Subdet = 0;
        int Ieta = 0;
        int Iphi = 0;
        int Depth = 0;
        float SimHitEnergy = 0.0;
        float RecoEnergy = 0.0;
        float AuxEnergy = 0.0;
        float RawEnergy = 0.0;
        float DLPHINEnergy = 0.0;
    };
    typedef std::pair<int, int> int_int_pair;
    typedef std::map <int_int_pair, std::vector<channel_info>> ieta_iphi_info_map;
    TString InputFileName;

    TH2F* BookTH2F(TString Name, std::vector<float> XBinVec, std::vector<float> YBinVec);
    void make_test_plots();
    void make_1d_plots();
    void make_2d_plots();

    void add_info_to_map(ieta_iphi_info_map &IetaIphiInfoMap, const channel_info &ChannelInfo);

};

#endif

#ifdef TreeReader_cxx
TreeReader::TreeReader(TString InputFileName_, TTree *tree) :
    fChain(0),
    InputFileName(InputFileName_)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(InputFileName);
      if (!f || !f->IsOpen()) {
         f = new TFile(InputFileName);
      }
      TDirectory * dir = (TDirectory*)f->Get(InputFileName + ":/DLPHIN_analyzer");
      dir->GetObject("DLPHINTree",tree);

   }
   Init(tree);
}

TreeReader::~TreeReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TreeReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TreeReader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TreeReader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   SubdetVec = 0;
   IetaVec = 0;
   IphiVec = 0;
   DepthVec = 0;
   RawIdVec = 0;
   RawChargesVec = 0;
   PedestalsVec = 0;
   TotNoisesVec = 0;
   RecoEnergyVec = 0;
   AuxEnergyVec = 0;
   RawEnergyVec = 0;
   DLPHINEnergyVec = 0;
   FlagVec = 0;
   Chi2Vec = 0;
   RawGainVec = 0;
   RespCorrVec = 0;
   DLPHINRespCorrVec = 0;
   nSimHitsVec = 0;
   SimHitEnergyVec = 0;
   SimHitTimeVec = 0;
   ArrivalTimeVec = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("LS", &LS, &b_LS);
   fChain->SetBranchAddress("PU", &PU, &b_PU);
   fChain->SetBranchAddress("SubdetVec", &SubdetVec, &b_SubdetVec);
   fChain->SetBranchAddress("IetaVec", &IetaVec, &b_IetaVec);
   fChain->SetBranchAddress("IphiVec", &IphiVec, &b_IphiVec);
   fChain->SetBranchAddress("DepthVec", &DepthVec, &b_DepthVec);
   fChain->SetBranchAddress("RawIdVec", &RawIdVec, &b_RawIdVec);
   fChain->SetBranchAddress("RawChargesVec", &RawChargesVec, &b_RawChargesVec);
   fChain->SetBranchAddress("PedestalsVec", &PedestalsVec, &b_PedestalsVec);
   fChain->SetBranchAddress("TotNoisesVec", &TotNoisesVec, &b_TotNoisesVec);
   fChain->SetBranchAddress("RecoEnergyVec", &RecoEnergyVec, &b_RecoEnergyVec);
   fChain->SetBranchAddress("AuxEnergyVec", &AuxEnergyVec, &b_AuxEnergyVec);
   fChain->SetBranchAddress("RawEnergyVec", &RawEnergyVec, &b_RawEnergyVec);
   fChain->SetBranchAddress("DLPHINEnergyVec", &DLPHINEnergyVec, &b_DLPHINEnergyVec);
   fChain->SetBranchAddress("FlagVec", &FlagVec, &b_FlagVec);
   fChain->SetBranchAddress("Chi2Vec", &Chi2Vec, &b_Chi2Vec);
   fChain->SetBranchAddress("RawGainVec", &RawGainVec, &b_RawGainVec);
   fChain->SetBranchAddress("RespCorrVec", &RespCorrVec, &b_RespCorrVec);
   fChain->SetBranchAddress("DLPHINRespCorrVec", &DLPHINRespCorrVec, &b_DLPHINRespCorrVec);
   fChain->SetBranchAddress("nSimHitsVec", &nSimHitsVec, &b_nSimHitsVec);
   fChain->SetBranchAddress("SimHitEnergyVec", &SimHitEnergyVec, &b_SimHitEnergyVec);
   fChain->SetBranchAddress("SimHitTimeVec", &SimHitTimeVec, &b_SimHitTimeVec);
   fChain->SetBranchAddress("ArrivalTimeVec", &ArrivalTimeVec, &b_ArrivalTimeVec);
   Notify();
}

Bool_t TreeReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TreeReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TreeReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

TH2F* TreeReader::BookTH2F(TString Name, std::vector<float> XBinVec, std::vector<float> YBinVec) {
    return new TH2F(Name, Name, XBinVec[0], XBinVec[1], XBinVec[2], YBinVec[0], YBinVec[1], YBinVec[2]);
}

void TreeReader::add_info_to_map(ieta_iphi_info_map &IetaIphiInfoMap, const channel_info &ChannelInfo) {
    auto IetaIphi = std::make_pair(ChannelInfo.Ieta, ChannelInfo.Iphi);
    if (IetaIphiInfoMap.find(IetaIphi) != IetaIphiInfoMap.end()) {
        (IetaIphiInfoMap.at(IetaIphi))[ChannelInfo.Depth-1] = ChannelInfo;
    }
    else {
        int MaxDepth = 4;
        if (ChannelInfo.Subdet == 2) {MaxDepth = 7;}
        channel_info ChannelInfoTmp;
        std::vector<channel_info> ChannelInfoVecTmp(MaxDepth, ChannelInfoTmp);
        IetaIphiInfoMap[IetaIphi] = ChannelInfoVecTmp;
        (IetaIphiInfoMap.at(IetaIphi))[ChannelInfo.Depth-1] = ChannelInfo;
    }
}

#endif // #ifdef TreeReader_cxx
