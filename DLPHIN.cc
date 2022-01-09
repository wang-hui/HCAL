// -*- C++ -*-
//
// Package:     RecoLocalCalo/HcalRecAlgos
// Class  :     DLPHIN
// 
// Implementation:
//     [Notes on implementation]
//
// Original Author:  Hui Wang
//         Created:  Sun, 09 Jan 2022 18:52:56 GMT
//

// system include files

// user include files
#include "RecoLocalCalo/HcalRecAlgos/interface/DLPHIN.h"


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DLPHIN::DLPHIN(const edm::ParameterSet& conf)
    : MyString_(conf.getParameter<std::string>("MyString")),
    MyInt_(conf.getParameter<int>("MyInt"))
{
}
//
// member functions
//
void DLPHIN::get_channel_infos (HBHEChannelInfoCollection* channel_infos) {
    channel_infos_ = channel_infos;
}

void DLPHIN::print_channel_infos() {
    std::cout << MyString_ << " " << MyInt_ << std::endl;
    int channel_infos_size = channel_infos_->size();
    for (int i = 0; i < channel_infos_size; i++) {
        std::cout << (*channel_infos_)[i].id() << ", ";
    }   
    //for (auto channel_info : channel_infos_) {
    //    std::cout << channel_info->id() << ", ";
    //} 
    std::cout << std::endl;
}
//
// const member functions
//

//
// static member functions
//
