// -*- C++ -*-
//
// Package:    HCALTest/HCALTestAna
// Class:      HCALTestAna
//
/**\class HCALTestAna HCALTestAna.cc HCALTest/HCALTestAna/plugins/HCALTestAna.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//calo headers
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalTestNumbering.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "SimCalorimetry/HcalSimAlgos/interface/HcalSimParameters.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
//#include "CondFormats/HcalObjects/interface/HcalRespCorr.h"
#include "samplingFactor.h"

// ROOT includes
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TMath.h"

//STL headers
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class HCALTestAna : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit HCALTestAna(const edm::ParameterSet&);
        ~HCALTestAna();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;
        void sum_energy_per_rawId(std::map <int, std::vector<float>> & id_energy_map, int id, float energy);

        bool print_1d;
        bool print_2d;
        bool do_PU;
        bool is_run3_relVal;
        float min_simHit_energy;
        edm::EDGetTokenT<std::vector<PCaloHit>> hcalhitsToken_;
        edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupInfoToken_;

        TH2F * simHit_energy_vs_time_h;
        TH2F * channel_energy_vs_time_h;
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
HCALTestAna::HCALTestAna(const edm::ParameterSet& iConfig):
    print_1d(iConfig.getUntrackedParameter<bool>("print_1d")),
    print_2d(iConfig.getUntrackedParameter<bool>("print_2d")),
    do_PU(iConfig.getUntrackedParameter<bool>("do_PU")),
    is_run3_relVal(iConfig.getUntrackedParameter<bool>("is_run3_relVal")),
    min_simHit_energy(iConfig.getUntrackedParameter<double>("min_simHit_energy"))

{
    //now do what ever initialization is needed
    hcalhitsToken_ = consumes<std::vector<PCaloHit>>(edm::InputTag("g4SimHits","HcalHits","SIM"));
    //pileupInfoToken_ = consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo","","HLT"));
    pileupInfoToken_ = consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo"));

    edm::Service<TFileService> fs;
    //fTree = fs->make<TTree>("fTree","fTree");
    simHit_energy_vs_time_h = fs->make<TH2F>("simHit_energy_vs_time_h", "simHit energy vs time", 200, 0.0, 1000.0, 100, 0.0, 500.0);
    channel_energy_vs_time_h = fs->make<TH2F>("channel_energy_vs_time_h", "channel energy vs time", 200, 0.0, 1000.0, 100, 0.0, 500.0);
}


HCALTestAna::~HCALTestAna()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------

void HCALTestAna::sum_energy_per_rawId(std::map <int, std::vector<float>> & id_energy_map, int id, float energy)
{
    std::map<int,std::vector<float>>::iterator it;
    it = id_energy_map.find(id);
    if (it != id_energy_map.end()) id_energy_map.at(id).push_back(energy);
    else id_energy_map[id] = {energy};
}

void HCALTestAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<std::vector<PCaloHit>> hcalhitsHandle;
    iEvent.getByToken(hcalhitsToken_, hcalhitsHandle);
    //iEvent.getByLabel("g4SimHits", "HcalHits", hand);
    const std::vector<PCaloHit> * SimHits = hcalhitsHandle.product();

    edm::ESHandle<HcalDDDRecConstants> pHRNDC;
    iSetup.get<HcalRecNumberingRecord>().get(pHRNDC);
    const HcalDDDRecConstants *hcons = &(*pHRNDC);

    edm::ESHandle<HcalDbService> conditions;
    iSetup.get<HcalDbRecord>().get(conditions);

    //HcalSimParameterMap* theParameterMap;
    //theParameterMap(new HcalSimParameterMap(iConfig)),

    typedef std::pair<int, int> ieta_iphi_pair;
    std::map <ieta_iphi_pair, std::vector<float>> ieta_iphi_energy_map;
    std::vector<float> depth_vec(7,0.0);
    for(int ieta = -29; ieta <= -16; ieta ++)
    {
        for(int iphi = 1; iphi <= 72; iphi ++)
        {
            ieta_iphi_energy_map[std::make_pair(ieta, iphi)] = depth_vec;
        }
    }
    for(int ieta = 16; ieta <= 29; ieta ++)
    {
        for(int iphi = 1; iphi <= 72; iphi ++)
        {
            ieta_iphi_energy_map[std::make_pair(ieta, iphi)] = depth_vec;
        }
    }

    std::map <int, std::vector<float>> id_energy_map, id_time_map;
    //int Nhb = 0, Nhe = 0, Nho = 0, Nhf = 0;
    for(auto iter : *SimHits)
    {
        HcalDetId hid(iter.id());
        hid = HcalDetId(HcalHitRelabeller::relabel(iter.id(), hcons));
        auto rawId = hid.rawId();
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        //auto ieta = hid.ieta();
        auto ietaAbs = hid.ietaAbs();
        //auto iphi = hid.iphi();

        auto energy = iter.energy();
        auto time = iter.time();

        /*
        float samplingFactor;
        if (subdet == HcalForward)
        {
            const HFSimParameters& pars = dynamic_cast<const HFSimParameters&>(theParameterMap->simParameters(hid));
            samplingFactor = pars.samplingFactor();
        }
        else
        {
            const HcalSimParameters& pars = dynamic_cast<const HcalSimParameters&>(theParameterMap->simParameters(hid));
            samplingFactor = pars.samplingFactor(hid);
        }
        */

        if(subdet == 1 || subdet == 2)
        {
            float samplingFactor = 0;
            float digi_SF = 1;
            if(subdet == 1 && ietaAbs-1 < (int)samplingFactors_hb.size())
            {
                samplingFactor = samplingFactors_hb.at(ietaAbs-1);
                //factor 0.5 for HB depth1, except for |ieta|=16 depth1
                if (is_run3_relVal && depth == 1 && ietaAbs != 16) digi_SF = 0.5;
            }
            if(subdet == 2 && ietaAbs-16 < (int)samplingFactors_he.size())
            {
                samplingFactor = samplingFactors_he.at(ietaAbs-16);
                //factor 1.2 for HE depth1
                if (depth == 1) digi_SF = 1.2;
            }
            if(samplingFactor == 0) std::cout << "Error! miss-match samplingFactor" << std::endl;
            //std::cout << rawId << ", " << subdet << ", " << depth << ", " << ieta << ", " << iphi << ", " << energy << ", " << samplingFactor << std::endl;
            sum_energy_per_rawId(id_energy_map, rawId, energy * samplingFactor * digi_SF);
            sum_energy_per_rawId(id_time_map, rawId, time);
            simHit_energy_vs_time_h->Fill(energy * samplingFactor * digi_SF, time);
        }

        //==================a test of HcalHitRelabeller, to be commented out=========================
        /*
        HcalDetId test_hid(rawId);
        std::cout << hid.ieta() << ", " << test_hid.ieta() << std::endl;
        std::cout << hid.iphi() << ", " << test_hid.iphi() << std::endl;
        std::cout << hid.depth() << ", " << test_hid.depth() << std::endl;
        int det, z, depth, eta, phi, lay;
        HcalTestNumbering::unpackHcalIndex(iter.id(), det, z, depth, eta, phi, lay);
        if (det == 1) Nhb ++;
        if (det == 2) Nhe ++;
        if (det == 3) Nho ++;
        if (det == 4) Nhf ++;
        std::cout << "ieta " << ieta << ", " << eta << std::endl;
        std::cout << "iphi " << iphi << ", " << phi << std::endl;
        std::cout << "subdet " << subdet << ", " << det << std::endl;
        //if(abs(ieta)>41) std::cout << ieta << ", " << iphi << ", " << subdet << std::endl;
        if(det == 1 || det == 2) std::cout << det << ", " << z << ", " << depth << ", " << eta << ", " << phi << ", " << lay << std::endl;
        */
        //=================================end of test===============================================
    }

    //int true_npu = 0;
    float obs_npu = 0;

    if(do_PU)
    {
        edm::Handle<std::vector<PileupSummaryInfo>> pileupInfo;
        iEvent.getByToken(pileupInfoToken_, pileupInfo);

        for (std::vector<PileupSummaryInfo>::const_iterator pvi = pileupInfo->begin(); pvi != pileupInfo->end(); ++pvi)
        {
            int bx = pvi->getBunchCrossing();
            if (bx == 0) {
                //true_npu = pvi->getTrueNumInteractions();
                obs_npu = pvi->getPU_NumInteractions();
                break;
            }
        }
        if(print_1d) std::cout << "gen: id, raw truth energy, median time, weighted time, PU" << std::endl;
    }
    else if(print_1d) std::cout << "gen: id, raw truth energy, median time, weighted time" << std::endl;

    for(auto iter : id_energy_map)
    {
        auto rawId = iter.first;
        //float RespCorr = 1;
        //const HcalCalibrations& calibrations = conditions->getHcalCalibrations(rawId);
        //RespCorr = calibrations.respcorr();
        HcalDetId hid(rawId);
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        auto iphi = hid.iphi();

        auto energy_vec = iter.second;
        float energy_sum = std::accumulate(energy_vec.begin(), energy_vec.end(), 0.0);

        auto time_vec = id_time_map.at(rawId);
        int vec_size = time_vec.size();
        float weighted_time = 0.0;
        for(int i = 0; i < vec_size; i++)
        {
            float simHit_energy = energy_vec.at(i);
            if (simHit_energy > min_simHit_energy)
            {weighted_time += time_vec.at(i) * simHit_energy / energy_sum;}
            //==============a test for simHit time, to be comment out===========
            /*
            if (energy_sum > 500)
            {
                if (i == 0)
                {
                    std::cout << "energy_sum = " << energy_sum << ", vec_size = " << vec_size << std::endl;
                    std::cout << "index, energy, time " << std::endl;
                }
                std::cout << i << ", " << simHit_energy << ", " << time_vec.at(i) << std::endl;
            }
            */
            //====================end of test====================================
        }
        channel_energy_vs_time_h->Fill(energy_sum, weighted_time);
        std::sort(time_vec.begin(), time_vec.end());
        float median_time = time_vec.at(vec_size/2);

        if(subdet == 2)
        {ieta_iphi_energy_map.at(std::make_pair(ieta, iphi)).at(depth - 1) = energy_sum;}

        //============= test un-reconstructed channels =========================
        /*
        if(depth == 1 && ieta == -19 && (iphi == 24 || iphi == 23))
        {
            std::cout << "find channel " << rawId << ", depth " << depth << ", ieta " << ieta << ", iphi " << iphi << ", energy " << energy_sum << ", time " << weighted_time << std::endl;
        }
        */
        //============= test end ===============================================

        if(print_1d)
        {
            if(do_PU) std::cout << rawId << ", " << energy_sum << ", " << median_time << ", " << weighted_time << ", " << obs_npu << std::endl;
            else std::cout << rawId << ", " << energy_sum << ", " << median_time << ", " << weighted_time << ", " << std::endl;
        }
    }

    if(print_2d)
    {
        std::cout << "gen: ieta, iphi, d1 raw truth energy, d2 raw truth energy, d3 raw truth energy, d4 raw truth energy, d5 raw truth energy, d6 raw truth energy, d7 raw truth energy" << std::endl;
        for(auto iter : ieta_iphi_energy_map)
        {
            auto key = iter.first;
            auto value = iter.second;
            std::cout << key.first << ", " << key.second;
            for(int i = 0; i < (int)value.size(); i++)
            {
                std::cout << ", " << value.at(i);
            }
            std::cout << std::endl;
        }
    }
}


// ------------ method called once each job just before starting event loop  ------------
void HCALTestAna::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void HCALTestAna::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HCALTestAna::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
DEFINE_FWK_MODULE(HCALTestAna);
