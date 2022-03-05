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

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
//#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "SimCalorimetry/HcalSimAlgos/interface/HcalSimParameterMap.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/DLPHIN.h"

//ROOT includes
#include "TH1.h"
#include "TH2.h"
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
        float MaxSimHitTime_;

        edm::EDGetTokenT<std::vector<PCaloHit>> HcalHitsToken_;
        edm::EDGetTokenT<HBHEChannelInfoCollection> ChannelInfosToken_;
        edm::EDGetTokenT<HBHERecHitCollection> HBHERecHitsToken_;

        DLPHIN DLPHIN_;
        HcalSimParameterMap HcalSimParameterMap_;

        typedef std::pair<std::vector<float>, std::vector<float>> energy_time_pair;
        void add_info_to_map(std::map <int, energy_time_pair>& id_info_map, int id, float energy, float time);
        void make_id_info_map(std::map <int, energy_time_pair>& id_info_map, std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons);

        edm::Service<TFileService> FileService;
        TTree* OutTree;

        int Run, Event, LS;        
        int Subdet, Ieta, Iphi, Depth;
        int RawId, Flags;
        float RecoEnergy, AuxEnergy, RawEnergy, DLPHINEnergy;
        float Chi2, RespCorr, DLPHINRespCorr;
        float SimHitEnergy, SimHitTime, ArrivalTime; 
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
    MaxSimHitTime_(iConfig.getParameter<double>("MaxSimHitTime")),
    DLPHIN_(iConfig.getParameter<edm::ParameterSet>("DLPHINConfig")),
    HcalSimParameterMap_(iConfig.getParameter<edm::ParameterSet>("hcalSimParameters"))
{
    //now do what ever initialization is needed
    //std::cout << iConfig << std::endl;
    usesResource("TFileService");

    if(HaveSimHits_){HcalHitsToken_ = consumes<std::vector<PCaloHit>>(edm::InputTag("g4SimHits","HcalHits","SIM"));}
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
    Run = iEvent.id().run();
    Event = iEvent.id().event();
    LS = iEvent.id().luminosityBlock();

    edm::ESHandle<CaloGeometry> geometry;
    iSetup.get<CaloGeometryRecord>().get(geometry);
    auto theGeometry = geometry.product();

    std::map <int, energy_time_pair> id_info_map;
    if(HaveSimHits_) {
        edm::Handle<std::vector<PCaloHit>> SimHitsHandle;
        iEvent.getByToken(HcalHitsToken_, SimHitsHandle);
        auto SimHits = *SimHitsHandle;

        edm::ESHandle<HcalDDDRecConstants> pHRNDC;
        iSetup.get<HcalRecNumberingRecord>().get(pHRNDC);
        auto hcons = pHRNDC.product();

        make_id_info_map(id_info_map, SimHits, hcons);
    }

    edm::ESHandle<HcalDbService> DbServ;
    iSetup.get<HcalDbRecord>().get(DbServ);

    edm::Handle<HBHEChannelInfoCollection> ChannelInfosHandle;
    iEvent.getByToken(ChannelInfosToken_, ChannelInfosHandle);
    auto ChannelInfos = ChannelInfosHandle.product();

    edm::Handle<HBHERecHitCollection> HBHERecHitsHandle;
    iEvent.getByToken(HBHERecHitsToken_, HBHERecHitsHandle);
    auto HBHERecHits = *HBHERecHitsHandle;

    DLPHIN_.DLPHIN_run(*DbServ, ChannelInfos, &HBHERecHits);
    auto DLPHIN_inter_data = DLPHIN_.DLPHIN_inter_data;

    std::cout << HBHERecHits.size() << ", " << DLPHIN_inter_data.size() << std::endl;

    for (int i = 0; i < (int)HBHERecHits.size(); i++) {
        auto HBHERecHit = HBHERecHits[i];
        auto Hid = HBHERecHit.id();
        RawId = Hid.rawId();
        Subdet = Hid.subdet();
        Ieta = Hid.ieta();
        Iphi = Hid.iphi();
        Depth = Hid.depth();

        Flags = HBHERecHit.flags();
        RecoEnergy = HBHERecHit.energy();
        AuxEnergy = HBHERecHit.eaux();
        RawEnergy = HBHERecHit.eraw();
        Chi2 = HBHERecHit.chi2();

        auto DLPHIN_pair = DLPHIN_inter_data.at(i);
        DLPHINEnergy = DLPHIN_pair.first;
        DLPHINRespCorr = DLPHIN_pair.second;

        const HcalCalibrations& calib = DbServ->getHcalCalibrations(Hid);
        //auto rawgain = calib.rawgain(0);
        RespCorr = calib.respcorr();

        SimHitEnergy = 0.0;
        SimHitTime = 0.0;
        ArrivalTime = 0.0;

        if (HaveSimHits_) {
            auto it = id_info_map.find(RawId);
            if (it != id_info_map.end()) {
                auto SimHitEnergyVec = id_info_map.at(RawId).first;
                auto SimHitTimeVec = id_info_map.at(RawId).second;
                auto distance = theGeometry->getPosition(Hid).mag();
                auto TOF = distance * cm / c_light;  // Units of c_light: mm/ns

                SimHitEnergy = std::accumulate(SimHitEnergyVec.begin(), SimHitEnergyVec.end(), 0.0);
                int vec_size = SimHitEnergyVec.size();
                for(int i = 0; i < vec_size; i++) {
                    SimHitTime += SimHitTimeVec.at(i) * SimHitEnergyVec.at(i) / SimHitEnergy;
                }
                ArrivalTime = SimHitTime - TOF + 75;
            }
        }
        /*
        std::cout << Hid << ", " << RawId << ", " << SimHitEnergy
                  << ", " << RecoEnergy / RespCorr << ", " << DLPHINEnergy
                  << ", " << DLPHINRespCorr << ", " << RespCorr
                  << ", " << SimHitTime << ", " << ArrivalTime << std::endl;
        */
        OutTree->Fill();
    }
}


// ------------ method called once each job just before starting event loop  ------------
void DLPHIN_analyzer::beginJob() {
    OutTree = FileService->make<TTree>("DLPHINTree", "DLPHINTree");

    OutTree->Branch("Run", &Run, "Run/I");
    OutTree->Branch("Event", &Event, "Event/I");
    OutTree->Branch("LS", &LS, "LS/I");

    OutTree->Branch("Subdet", &Subdet, "Subdet/I");
    OutTree->Branch("Ieta", &Ieta, "Ieta/I");
    OutTree->Branch("Iphi", &Iphi, "Iphi/I");
    OutTree->Branch("Depth", &Depth, "Depth/I");
    OutTree->Branch("RawId", &RawId, "RawId/I");
    OutTree->Branch("Flags", &Flags, "Flags/I");

    OutTree->Branch("RecoEnergy", &RecoEnergy, "RecoEnergy/F");
    OutTree->Branch("AuxEnergy", &AuxEnergy, "AuxEnergy/F");
    OutTree->Branch("RawEnergy", &RawEnergy, "RawEnergy/F");
    OutTree->Branch("DLPHINEnergy", &DLPHINEnergy, "DLPHINEnergy/F");

    OutTree->Branch("Chi2", &Chi2, "Chi2/F");
    OutTree->Branch("RespCorr", &RespCorr, "RespCorr/F");
    OutTree->Branch("DLPHINRespCorr", &DLPHINRespCorr, "DLPHINRespCorr/F");

    OutTree->Branch("SimHitEnergy", &SimHitEnergy, "SimHitEnergy/F");
    OutTree->Branch("SimHitTime", &SimHitTime, "SimHitTime/F");
    OutTree->Branch("ArrivalTime", &ArrivalTime, "ArrivalTime/F");
}

// ------------ method called once each job just after ending the event loop  ------------
void DLPHIN_analyzer::endJob() {
}

void DLPHIN_analyzer::add_info_to_map(std::map <int, energy_time_pair>& id_info_map, int id, float energy, float time) {
    auto it = id_info_map.find(id);
    if (it != id_info_map.end()) {
        id_info_map.at(id).first.push_back(energy);
        id_info_map.at(id).second.push_back(time);
    }
    else {
        energy_time_pair temp_pair = {{energy},{time}};
        id_info_map[id] = temp_pair;
    }
}

void DLPHIN_analyzer::make_id_info_map(std::map <int, energy_time_pair>& id_info_map, std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons) {
    HcalHitRelabeller SimHitRelabeller(true);
    SimHitRelabeller.setGeometry(hcons);
    SimHitRelabeller.process(SimHits);

    for(auto SimHit : SimHits)
    {
        auto id = SimHit.id();
        auto energy = SimHit.energy();
        auto time = SimHit.time();

        HcalDetId hid(id);
        auto rawId = hid.rawId();
        auto subdet = hid.subdet();

        if ((subdet == 1 || subdet == 2) && time < MaxSimHitTime_) {
            float samplingFactor = 1.0;
            if (subdet == 1) {
                samplingFactor = HcalSimParameterMap_.hbParameters().samplingFactor(hid);
            }
            else {
                samplingFactor = HcalSimParameterMap_.heParameters().samplingFactor(hid);
            }
            add_info_to_map(id_info_map, rawId, energy * samplingFactor, time);
        }
    }
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
