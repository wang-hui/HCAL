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

class HCALTestAna : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
	public:
		explicit HCALTestAna(const edm::ParameterSet&);
		~HCALTestAna();

		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


	private:
		virtual void beginJob() override;
		virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
		virtual void endJob() override;
		void sum_energy_per_rawId(std::map <int, float> & id_energy_map, int id, float energy);

		bool do_PU;
		edm::EDGetTokenT<std::vector<PCaloHit>> hcalhitsToken_;
		edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupInfoToken_;

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
	do_PU(iConfig.getUntrackedParameter<bool>("do_PU"))

{
	//now do what ever initialization is needed
	hcalhitsToken_ = consumes<std::vector<PCaloHit>>(edm::InputTag("g4SimHits","HcalHits","SIM"));
	pileupInfoToken_ = consumes<std::vector<PileupSummaryInfo>>(edm::InputTag("addPileupInfo","","HLT"));
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

void HCALTestAna::sum_energy_per_rawId(std::map <int, float> & id_energy_map, int id, float energy)
{
	std::map<int,float>::iterator it;
	it = id_energy_map.find(id);
	if (it != id_energy_map.end())
	{
		id_energy_map.at(id) = id_energy_map.at(id) + energy;
	}
	else
	{
		id_energy_map[id] = energy;
		//id_energy_map.insert (std::pair<int,float>(id, energy));
	}
}

void HCALTestAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

	Handle<std::vector<PCaloHit>> hcalhitsHandle;
	iEvent.getByToken(hcalhitsToken_, hcalhitsHandle);
	//   iEvent.getByLabel("g4SimHits", "HcalHits", hand);
	const std::vector<PCaloHit> * SimHits = hcalhitsHandle.product();

	ESHandle<HcalDDDRecConstants> pHRNDC;
	iSetup.get<HcalRecNumberingRecord>().get(pHRNDC);
	const HcalDDDRecConstants *hcons = &(*pHRNDC);

	ESHandle<HcalDbService> conditions;
	iSetup.get<HcalDbRecord>().get(conditions);

	//HcalSimParameterMap* theParameterMap;
	//theParameterMap(new HcalSimParameterMap(iConfig)),

	std::map <int, float> id_energy_map;
	//int Nhb = 0, Nhe = 0, Nho = 0, Nhf = 0;
	for(auto iter : *SimHits)
	{
		HcalDetId hid(iter.id());
		hid = HcalDetId(HcalHitRelabeller::relabel(iter.id(), hcons));
		auto rawId = hid.rawId();
		auto subdet = hid.subdet();
		auto depth = hid.depth();
		auto ieta = hid.ieta();
		auto ietaAbs = hid.ietaAbs();
		auto iphi = hid.iphi();

		auto energy = iter.energy();
		/*float samplingFactor;
		  if (subdet == HcalForward)
		  {
		  const HFSimParameters& pars = dynamic_cast<const HFSimParameters&>(theParameterMap->simParameters(hid));
		  samplingFactor = pars.samplingFactor();
		  }
		  else
		  {
		  const HcalSimParameters& pars = dynamic_cast<const HcalSimParameters&>(theParameterMap->simParameters(hid));
		  samplingFactor = pars.samplingFactor(hid);
		  }*/

		if(subdet == 1 || subdet == 2)
		{
			float samplingFactor = 0;
			if(subdet == 1 && ietaAbs-1 < (int)samplingFactors_hb.size()) samplingFactor = samplingFactors_hb.at(ietaAbs-1);
			if(subdet == 2 && ietaAbs-16 < (int)samplingFactors_he.size()) samplingFactor = samplingFactors_he.at(ietaAbs-16);
			if(samplingFactor == 0) std::cout << "miss-match samplingFactor" << std::endl;
			//std::cout << rawId << ", " << subdet << ", " << depth << ", " << ieta << ", " << iphi << ", " << energy << ", " << samplingFactor << std::endl;
			sum_energy_per_rawId(id_energy_map, rawId, energy * samplingFactor);
		}

		//==================a test of HcalHitRelabeller, to be commented out=========================
		/*
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

	//==================a test of unused rawId in HBHEChannelInfo, to be commented out===================
	/*
	   std::vector <int> unused_rawId = {1158694932, 1158694943, 1158706207, 1159219206, 1159219208, 1159219212, 1159219213, 1159219216, 1159219222, 1159219226, 1159219227, 1159219228, 1159219229, 1159219230, 1159219231, 1159219232, 1159219233, 1159219234, 1159219269, 1159219270, 1159743519, 1159754783};
	   for (int i = 0; i < (int)unused_rawId.size(); i++)
	   {
	   HcalDetId hid_test(unused_rawId.at(i));
	//hid_test = HcalDetId(HcalHitRelabeller::relabel(unused_rawId.at(i), hcons));
	std::cout << hid_test.rawId() << ", " << hid_test.subdet() << ", " << hid_test.depth() << ", " << hid_test.ieta() << ", " << hid_test.iphi() << std::endl;
	}
	*/
	//==========================================end of test==============================================
	//std::cout << "Total channel " << SimHits->size() << ", Nhb " << Nhb << ", Nhe " << Nhe << ", Nho " << Nho << ", Nhf " << Nhf << std::endl;
	//std::cout << "id_energy_map.size() = " << id_energy_map.size() << std::endl;

	//int true_npu = 0;
	float obs_npu = 0;

	if(do_PU)
	{
		Handle<std::vector<PileupSummaryInfo>> pileupInfo;
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
		std::cout << "gen: id, energy, PU" << std::endl;
	}
	else std::cout << "gen: id, energy" << std::endl;

	for(auto iter : id_energy_map)
	{
		auto rawId = iter.first;
		float RespCorr = 1;
		const HcalCalibrations& calibrations = conditions->getHcalCalibrations(rawId);
		//HcalRespCorr(rawId, RespCorr);
		RespCorr = calibrations.respcorr ();
		if(do_PU) std::cout << rawId << ", " << iter.second * RespCorr << ", " << obs_npu << std::endl;
		else std::cout << rawId << ", " << iter.second * RespCorr << std::endl;
	}
	//std::cout << "reco: TS1 raw charge, TS1 ped noise, TS1 ADC count, TS1 rise time, TS2 raw charge, TS2 ped noise, TS2 ADC count, TS2 rise time, TS3 raw charge, TS3 ped noise, TS3 ADC count, TS3 rise time, TS4 raw charge, TS4 ped noise, TS4 ADC count, TS4 rise time, TS5 raw charge, TS5 ped noise, TS5 ADC count, TS5 rise time, TS6 raw charge, TS6 ped noise, TS6 ADC count, TS6 rise time, TS7 raw charge, TS7 ped noise, TS7 ADC count, TS7 rise time, TS8 raw charge, TS8 ped noise, TS8 ADC count, TS8 rise time, raw energy, gain, reco energy, id, sub detector, depth, ieta, iphi" << std::endl;
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
void
HCALTestAna::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
