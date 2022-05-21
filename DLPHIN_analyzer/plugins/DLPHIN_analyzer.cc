// -*- C++ -*-
//
// Package:    HCAL/DLPHIN_analyzer
// Class:      DLPHIN_analyzer
//
/**\class DLPHIN_analyzer DLPHIN_analyzer.cc HCAL/DLPHIN_analyzer/plugins/DLPHIN_analyzer.cc

 Description: An EDAnalyzer to study recHits, DLPHIN and simHits

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Hui Wang
//         Created:  Mon, 31 Jan 2022 16:56:29 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/DLPHIN.h"

//ROOT includes
#include "TTree.h"
#include "TMath.h"

//STL headers
#include <vector>
#include <string>
#include <iostream>

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class DLPHIN_analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit DLPHIN_analyzer(const edm::ParameterSet&);
        ~DLPHIN_analyzer();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        // ----------member data ---------------------------
        bool HaveSimHits_;

        edm::EDGetTokenT<std::vector<PCaloHit>> HcalHitsToken_;
        edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PUInfosToken_;
        edm::EDGetTokenT<HBHEChannelInfoCollection> ChannelInfosToken_;
        edm::EDGetTokenT<HBHERecHitCollection> HBHERecHitsToken_;

        DLPHIN DLPHIN_;
        void clear_vectors();

        edm::Service<TFileService> FileService;
        TTree* OutTree;

        // ----------varialbes to be filled in TTree ---------------------------
        int Run, Event, LS;
        float PU;

        std::vector<int> SubdetVec, IetaVec, IphiVec, DepthVec;
        std::vector<unsigned int> RawIdVec;
        std::vector<std::vector<float>> RawChargesVec, PedestalsVec, TotNoisesVec; // plural form for vec of vec
        std::vector<float> RecoEnergyVec, AuxEnergyVec, RawEnergyVec, DLPHINEnergyVec;
        std::vector<unsigned int> FlagVec;
        std::vector<float> Chi2Vec, RawGainVec, RespCorrVec, DLPHINRespCorrVec;
        std::vector<int> nSimHitsVec;
        std::vector<float> SimHitEnergyVec, SimHitTimeVec, ArrivalTimeVec;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DLPHIN_analyzer::DLPHIN_analyzer(const edm::ParameterSet& iConfig):
    HaveSimHits_(iConfig.getParameter<bool>("HaveSimHits")),
    DLPHIN_(iConfig.getParameter<edm::ParameterSet>("DLPHINConfig"))
{
    //now do what ever initialization is needed
    //std::cout << iConfig << std::endl;
    usesResource("TFileService");

    if(HaveSimHits_) {
        PUInfosToken_ = consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo"));
        HcalHitsToken_ = consumes<std::vector<PCaloHit>>(edm::InputTag("g4SimHits","HcalHits","SIM"));
    }
    ChannelInfosToken_ = consumes<HBHEChannelInfoCollection>(edm::InputTag("hbheprereco"));
    HBHERecHitsToken_ = consumes<HBHERecHitCollection>(edm::InputTag("hbheprereco"));
}


DLPHIN_analyzer::~DLPHIN_analyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void DLPHIN_analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    clear_vectors();

    Run = iEvent.id().run();
    Event = iEvent.id().event();
    LS = iEvent.id().luminosityBlock();

    edm::ESHandle<CaloGeometry> geometry;
    iSetup.get<CaloGeometryRecord>().get(geometry);
    auto theGeometry = geometry.product();

    edm::ESHandle<HcalDbService> DbServ;
    iSetup.get<HcalDbRecord>().get(DbServ);

    edm::Handle<HBHEChannelInfoCollection> ChannelInfosHandle;
    iEvent.getByToken(ChannelInfosToken_, ChannelInfosHandle);
    auto ChannelInfos = ChannelInfosHandle.product();

    edm::Handle<HBHERecHitCollection> HBHERecHitsHandle;
    iEvent.getByToken(HBHERecHitsToken_, HBHERecHitsHandle);
    auto HBHERecHits = *HBHERecHitsHandle;

    DLPHIN_.DLPHIN_run(*DbServ, ChannelInfos, &HBHERecHits);
    auto DLPHIN_debug_infos = DLPHIN_.DLPHIN_debug_infos;

    PU = 0.0;
    std::vector<DLPHIN::energy_time_pair> SimHit_debug_infos;
    if(HaveSimHits_) {
        edm::Handle<std::vector<PileupSummaryInfo>> PUInfosHandle;
        iEvent.getByToken(PUInfosToken_, PUInfosHandle);
        auto PUInfos = PUInfosHandle.product();

        for (auto PUInfo : *PUInfos) {
            if (PUInfo.getBunchCrossing() == 0) {
                //PU = PUInfo.getTrueNumInteractions();
                PU = PUInfo.getPU_NumInteractions();
                break;
            }
        }

        edm::Handle<std::vector<PCaloHit>> SimHitsHandle;
        iEvent.getByToken(HcalHitsToken_, SimHitsHandle);
        auto SimHits = *SimHitsHandle;

        edm::ESHandle<HcalDDDRecConstants> pHRNDC;
        iSetup.get<HcalRecNumberingRecord>().get(pHRNDC);
        auto hcons = pHRNDC.product();

        DLPHIN_.SimHit_run(SimHits, hcons, &HBHERecHits);
        SimHit_debug_infos = DLPHIN_.SimHit_debug_infos;
    }

    int nRecHits = HBHERecHits.size();
    //std::cout << nRecHits << ", " << DLPHIN_inter_data.size() << std::endl;

    for (int iRecHit = 0; iRecHit < nRecHits; iRecHit++) {
        auto HBHERecHit = HBHERecHits[iRecHit];
        auto Hid = HBHERecHit.id();
        auto RawId = Hid.rawId();
        auto Subdet = Hid.subdet();
        auto Ieta = Hid.ieta();
        auto Iphi = Hid.iphi();
        auto Depth = Hid.depth();

        auto RecoEnergy = HBHERecHit.energy();
        auto AuxEnergy = HBHERecHit.eaux();
        auto RawEnergy = HBHERecHit.eraw();
        auto Chi2 = HBHERecHit.chi2();
        auto Flag = HBHERecHit.flags();

        std::vector<float> RawCharges(8), Pedestals(8), TotNoises(8);

        auto ChannelInfo = (*ChannelInfos)[iRecHit];
        for (int iTS = 0; iTS < 8; ++iTS)
        {
            auto RawCharge = ChannelInfo.tsRawCharge(iTS);
            auto Pedestal = ChannelInfo.tsPedestal(iTS);
            auto NetCharge = RawCharge - Pedestal;

            //ADC granularity
            const float norm_ = (1.f / std::sqrt(12));
            auto noiseADC = norm_ * ChannelInfo.tsDFcPerADC(iTS);
            //Electronic pedestal
            auto pedWidth = ChannelInfo.tsPedestalWidth(iTS);
            //Photostatistics
            auto noisePhotoSq = (NetCharge > pedWidth) ? (NetCharge * ChannelInfo.fcByPE()) : 0.f;

            RawCharges[iTS] = RawCharge;
            Pedestals[iTS] = Pedestal;
            TotNoises[iTS] = std::sqrt(noiseADC*noiseADC + pedWidth*pedWidth + noisePhotoSq);
        }

        auto DLPHIN_debug_info = DLPHIN_debug_infos.at(iRecHit);
        auto DLPHINEnergy = DLPHIN_debug_info.first;
        auto DLPHINRespCorr = DLPHIN_debug_info.second;

        const HcalCalibrations& calib = DbServ->getHcalCalibrations(Hid);
        auto RawGain = calib.rawgain(0);
        auto RespCorr = calib.respcorr();

        int nSimHits = 0;
        float SimHitEnergy = 0.0;
        float SimHitTime = 0.0;
        float ArrivalTime = 0.0;

        if (HaveSimHits_) {
            auto SimHit_debug_info = SimHit_debug_infos.at(iRecHit);
            auto SimHitEnergyVec = SimHit_debug_info.first;
            auto SimHitTimeVec = SimHit_debug_info.second;
            auto distance = theGeometry->getPosition(Hid).mag();
            auto TOF = distance * cm / c_light;  // Units of c_light: mm/ns

            SimHitEnergy = std::accumulate(SimHitEnergyVec.begin(), SimHitEnergyVec.end(), 0.0);
            nSimHits = SimHitEnergyVec.size();
            for(int iSimHit = 0; iSimHit < nSimHits; iSimHit++) {
                SimHitTime += SimHitTimeVec.at(iSimHit) * SimHitEnergyVec.at(iSimHit) / SimHitEnergy;
            }
            ArrivalTime = SimHitTime - TOF + 75;
        }
        /*
        std::cout << Hid << ", " << RawId << ", " << SimHitEnergy
                  << ", " << RecoEnergy / RespCorr << ", " << DLPHINEnergy
                  << ", " << DLPHINRespCorr << ", " << RespCorr
                  << ", " << SimHitTime << ", " << ArrivalTime << std::endl;
        */
        SubdetVec.push_back(Subdet);
        IetaVec.push_back(Ieta);
        IphiVec.push_back(Iphi);
        DepthVec.push_back(Depth);
        RawIdVec.push_back(RawId);

        RawChargesVec.push_back(RawCharges);
        PedestalsVec.push_back(Pedestals);
        TotNoisesVec.push_back(TotNoises);

        RecoEnergyVec.push_back(RecoEnergy);
        AuxEnergyVec.push_back(AuxEnergy);
        RawEnergyVec.push_back(RawEnergy);
        DLPHINEnergyVec.push_back(DLPHINEnergy);

        FlagVec.push_back(Flag);
        Chi2Vec.push_back(Chi2);
        RawGainVec.push_back(RawGain);
        RespCorrVec.push_back(RespCorr);
        DLPHINRespCorrVec.push_back(DLPHINRespCorr);

        nSimHitsVec.push_back(nSimHits);
        SimHitEnergyVec.push_back(SimHitEnergy);
        SimHitTimeVec.push_back(SimHitTime);
        ArrivalTimeVec.push_back(ArrivalTime);
    }
    OutTree->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void DLPHIN_analyzer::beginJob() {
    OutTree = FileService->make<TTree>("DLPHINTree", "DLPHINTree");

    OutTree->Branch("Run", &Run, "Run/I");
    OutTree->Branch("Event", &Event, "Event/I");
    OutTree->Branch("LS", &LS, "LS/I");
    OutTree->Branch("PU", &PU, "PU/F");

    OutTree->Branch("SubdetVec", &SubdetVec);
    OutTree->Branch("IetaVec", &IetaVec);
    OutTree->Branch("IphiVec", &IphiVec);
    OutTree->Branch("DepthVec", &DepthVec);
    OutTree->Branch("RawIdVec", &RawIdVec);

    OutTree->Branch("RawChargesVec", &RawChargesVec);
    OutTree->Branch("PedestalsVec", &PedestalsVec);
    OutTree->Branch("TotNoisesVec", &TotNoisesVec);

    OutTree->Branch("RecoEnergyVec", &RecoEnergyVec);
    OutTree->Branch("AuxEnergyVec", &AuxEnergyVec);
    OutTree->Branch("RawEnergyVec", &RawEnergyVec);
    OutTree->Branch("DLPHINEnergyVec", &DLPHINEnergyVec);

    OutTree->Branch("FlagVec", &FlagVec);
    OutTree->Branch("Chi2Vec", &Chi2Vec);
    OutTree->Branch("RawGainVec", &RawGainVec);
    OutTree->Branch("RespCorrVec", &RespCorrVec);
    OutTree->Branch("DLPHINRespCorrVec", &DLPHINRespCorrVec);

    OutTree->Branch("nSimHitsVec", &nSimHitsVec);
    OutTree->Branch("SimHitEnergyVec", &SimHitEnergyVec);
    OutTree->Branch("SimHitTimeVec", &SimHitTimeVec);
    OutTree->Branch("ArrivalTimeVec", &ArrivalTimeVec);
}

// ------------ method called once each job just after ending the event loop  ------------
void DLPHIN_analyzer::endJob() {
}

void DLPHIN_analyzer::clear_vectors() {
    SubdetVec.clear();
    IetaVec.clear();
    IphiVec.clear();
    DepthVec.clear();
    RawIdVec.clear();

    RawChargesVec.clear();
    PedestalsVec.clear();
    TotNoisesVec.clear();

    RecoEnergyVec.clear();
    AuxEnergyVec.clear();
    RawEnergyVec.clear();
    DLPHINEnergyVec.clear();

    FlagVec.clear();
    Chi2Vec.clear();
    RawGainVec.clear();
    RespCorrVec.clear();
    DLPHINRespCorrVec.clear();

    nSimHitsVec.clear();
    SimHitEnergyVec.clear();
    SimHitTimeVec.clear();
    ArrivalTimeVec.clear();
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void DLPHIN_analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DLPHIN_analyzer);
