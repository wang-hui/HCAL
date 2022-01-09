#ifndef RecoLocalCalo_HcalRecAlgos_DLPHIN_h
#define RecoLocalCalo_HcalRecAlgos_DLPHIN_h
// -*- C++ -*-
//
// Package:     RecoLocalCalo/HcalRecAlgos
// Class  :     DLPHIN
// 
/**\class DLPHIN DLPHIN.h RecoLocalCalo/HcalRecAlgos/interface/DLPHIN.h
                 DLPHIN.cc RecoLocalCalo/HcalRecAlgos/src/DLPHIN.cc

 Description: DLPHIN: Deep Learning Processes for HCAL INtegration

 Usage:
    <usage>

*/
//
// Original Author:  Hui Wang
//         Created:  Sun, 09 Jan 2022 18:52:56 GMT
//

// system include files
#include <iostream>

// user include files
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

// forward declarations

class DLPHIN {
public:
    DLPHIN(const edm::ParameterSet& conf);
    ~DLPHIN(){};

    // ---------- const member functions ---------------------

    // ---------- static member functions --------------------

    // ---------- member functions ---------------------------
    void get_channel_infos (HBHEChannelInfoCollection* channel_infos);
    void print_channel_infos();

private:
    // ---------- member data --------------------------------
    std::string MyString_;
    int MyInt_;
    HBHEChannelInfoCollection* channel_infos_;
};

#endif
