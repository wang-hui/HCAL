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
DLPHIN::DLPHIN(const edm::ParameterSet& conf):
    DLPHIN_debug_(conf.getParameter<bool>("DLPHIN_debug")),
    DLPHIN_print_config_(conf.getParameter<bool>("DLPHIN_print_config")),
    DLPHIN_truncate_(conf.getParameter<bool>("DLPHIN_truncate")),
    DLPHIN_apply_respCorr_(conf.getParameter<bool>("DLPHIN_apply_respCorr")),
    DLPHIN_respCorr_name_((conf.getParameter<edm::FileInPath>("DLPHIN_respCorr_name")).fullPath()),
    DLPHIN_pb_2dHB_((conf.getParameter<edm::FileInPath>("DLPHIN_pb_2dHB")).fullPath()),
    DLPHIN_pb_2dHE_((conf.getParameter<edm::FileInPath>("DLPHIN_pb_2dHE")).fullPath()),
    MaxSimHitTime_(conf.getParameter<double>("MaxSimHitTime")),
    HcalSimParameterMap_(conf.getParameter<edm::ParameterSet>("hcalSimParameters"))
{
    if (DLPHIN_print_config_)
        print_config();

    // an ad hoc way to get respCorr. will update to get respCorr from db
    if (DLPHIN_apply_respCorr_) {
        TFile *DLPHIN_respCorr_file = new TFile(DLPHIN_respCorr_name_.c_str());
        for(int Depth = 1; Depth <= 7; Depth++) {
            std::string HistName = "D" + std::to_string(Depth);
            auto Hist = (TH1F*)DLPHIN_respCorr_file->Get(HistName.c_str());
            for(int Ieta = -29; Ieta <= 29; Ieta++) {
                auto RespCorr = Hist->GetBinContent(Hist->FindBin(Ieta));
                ieta_depth_respCorr_map[std::make_pair(Ieta, Depth)] = RespCorr;
            }
        }
        DLPHIN_respCorr_file->Close();
    }
    tensorflow::GraphDef *graphDef_2dHB = tensorflow::loadGraphDef(DLPHIN_pb_2dHB_);
    session_2dHB = tensorflow::createSession(graphDef_2dHB);
    tensorflow::GraphDef *graphDef_2dHE = tensorflow::loadGraphDef(DLPHIN_pb_2dHE_);
    session_2dHE = tensorflow::createSession(graphDef_2dHE);
}
//
// member functions
//
void DLPHIN::print_config() {
    std::cout << "=========================================================" << std::endl;
    std::cout << "DLPHIN_debug: " << DLPHIN_debug_ << std::endl;
    std::cout << "DLPHIN_apply_respCorr: " << DLPHIN_apply_respCorr_ << std::endl;
    std::cout << "DLPHIN_respCorr_name: " << DLPHIN_respCorr_name_ << std::endl;
    std::cout << "DLPHIN_truncate: " << DLPHIN_truncate_ << std::endl;
    std::cout << "DLPHIN_pb_2dHB: " << DLPHIN_pb_2dHB_ << std::endl;
    std::cout << "DLPHIN_pb_2dHE: " << DLPHIN_pb_2dHE_ << std::endl;
    std::cout << "=========================================================" << std::endl;
}

void DLPHIN::DLPHIN_run (const HcalDbService& DbServ, const HBHEChannelInfoCollection *ChannelInfos, HBHERecHitCollection *RecHits) {
    //std::cout << "nChannelInfos " << ChannelInfos->size() << ", nRecHits " << RecHits->size() << std::endl;
    DLPHIN_debug_infos.clear();
    
    std::vector<int_int_pair> HB_ieta_iphi_vec, HE_ieta_iphi_vec;
    std::vector<vec_depth_charge> HB_depth_charge_vec, HE_depth_charge_vec;
    preprocess(DbServ, ChannelInfos, HB_ieta_iphi_vec, HE_ieta_iphi_vec, HB_depth_charge_vec, HE_depth_charge_vec);

    //Initialize DLPHIN inputs tensors
    //ch_input: (charge - pedestal) * rawgain
    //ty_input: |ieta|
    //ma_input: whether a channel is real or a place holder
    int HB_tot_rows = HB_ieta_iphi_vec.size();
    int HE_tot_rows = HE_ieta_iphi_vec.size();

    tensorflow::Tensor HB_ch_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, HB_tot_col});
    tensorflow::Tensor HB_ty_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, 1});
    tensorflow::Tensor HB_ma_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, HB_depth_max});

    tensorflow::Tensor HE_ch_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, HE_tot_col});
    tensorflow::Tensor HE_ty_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, 1});
    tensorflow::Tensor HE_ma_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, HE_depth_max});

    //Fill DLPHIN inputs tensors
    process_inputs (HB_ieta_iphi_vec, HB_depth_charge_vec, HB_ch_input_2d, HB_ty_input_2d, HB_ma_input_2d);
    process_inputs (HE_ieta_iphi_vec, HE_depth_charge_vec, HE_ch_input_2d, HE_ty_input_2d, HE_ma_input_2d);

    //DLPHIN inference
    //outputs is a vector of one rank-3 tensor [pred:mask:depth]
    std::vector<tensorflow::Tensor> HB_outputs_2d, HE_outputs_2d;
    if (HB_tot_rows > 0)        // protection as TF don't like size=0 tensor
        tensorflow::run(session_2dHB,
        {{"net_charges",HB_ch_input_2d},{"types_input",HB_ty_input_2d},{"mask_input",HB_ma_input_2d}},
        {"output/Reshape"}, &HB_outputs_2d);
    if (HE_tot_rows > 0)
        tensorflow::run(session_2dHE,
        {{"net_charges",HE_ch_input_2d},{"types_input",HE_ty_input_2d},{"mask_input",HE_ma_input_2d}},
        {"output/Reshape"}, &HE_outputs_2d);
    /*
    std::cout << "HB_outputs_2d.size() " << HB_outputs_2d.size() <<
    ", tensor.shape().dim_size(0) " << HB_outputs_2d[0].shape().dim_size(0) <<
    ", tensor.shape().dim_size(1) " << HB_outputs_2d[0].shape().dim_size(1) <<
    ", tensor.shape().dim_size(2) " << HB_outputs_2d[0].shape().dim_size(2) << std::endl; 
    */

    //Save DLPHIN outputs to RecHits
    save_outputs (RecHits, HB_ieta_iphi_vec, HE_ieta_iphi_vec, HB_outputs_2d, HE_outputs_2d);
}

void DLPHIN::preprocess (const HcalDbService& DbServ,
                        const HBHEChannelInfoCollection *ChannelInfos,
                        std::vector<int_int_pair>& HB_ieta_iphi_vec,
                        std::vector<int_int_pair>& HE_ieta_iphi_vec,
                        std::vector<vec_depth_charge>& HB_depth_charge_vec,
                        std::vector<vec_depth_charge>& HE_depth_charge_vec
                        ) {
    for (const auto& ChannelInfo : *ChannelInfos) {
        auto Hid = ChannelInfo.id();
        auto Subdet = Hid.subdet();
        auto Depth = Hid.depth();
        auto Ieta = Hid.ieta();
        auto Iphi = Hid.iphi();

        const HcalCalibrations& Hcalib = DbServ.getHcalCalibrations(Hid);
        auto RawGain = Hcalib.rawgain(0);

        auto  ChargeVec = EmptyChargeVec;
        for (int iTS = 0; iTS < TS_max; iTS++) {
            ChargeVec.at(iTS) = (ChannelInfo.tsRawCharge(iTS) - ChannelInfo.tsPedestal(iTS)) * RawGain;
        }

        if (Subdet == 1) add_info_to_vec(HB_ieta_iphi_vec, HB_depth_charge_vec, Subdet, Depth, Ieta, Iphi, ChargeVec);
        else if (Subdet == 2) add_info_to_vec(HE_ieta_iphi_vec, HE_depth_charge_vec, Subdet, Depth, Ieta, Iphi, ChargeVec);
    }
}

void DLPHIN::add_info_to_vec (std::vector<int_int_pair>& ieta_iphi_vec,
                            std::vector<vec_depth_charge>& vec_depth_charge_vec,
                            const int& Subdet, const int& Depth, const int& Ieta, const int& Iphi,
                            const std::vector<float>& ChargeVec
                            ) {
    auto IetaIphi = std::make_pair(Ieta, Iphi);
    auto iter = std::find(ieta_iphi_vec.begin(), ieta_iphi_vec.end(), IetaIphi);
    if (iter != ieta_iphi_vec.end()) {
        int Row = iter - ieta_iphi_vec.begin();
        vec_depth_charge_vec.at(Row).at(Depth-1) = ChargeVec;
    }
    else {
        ieta_iphi_vec.push_back(IetaIphi);

        int MaxDepth = HB_depth_max;
        if (Subdet == 2) {MaxDepth = HE_depth_max;}
        auto ChargeVecTmp = EmptyChargeVec;
        vec_depth_charge VecChargeVecTmp(MaxDepth, ChargeVecTmp);
        vec_depth_charge_vec.push_back(VecChargeVecTmp);
        vec_depth_charge_vec.back().at(Depth-1) = ChargeVec;
    }
}

void DLPHIN::process_inputs (const std::vector<int_int_pair>& ieta_iphi_vec,
                            const std::vector<vec_depth_charge>& vec_depth_charge_vec,
                            tensorflow::Tensor& ch_input_2d,
                            tensorflow::Tensor& ty_input_2d,
                            tensorflow::Tensor& ma_input_2d
                            ) {
    const int nRow = vec_depth_charge_vec.size();
    for (int Row = 0; Row < nRow; Row++) {
        const auto& VecChargeVec = vec_depth_charge_vec.at(Row);
        const int nDepth = VecChargeVec.size();
        for (int iDepth = 0; iDepth < nDepth; iDepth++) {
            const auto& ChargeVec = VecChargeVec.at(iDepth);
            if (ChargeVec == EmptyChargeVec)
                ma_input_2d.tensor<float, 2>()(Row, iDepth) = 0.0;
            else
                ma_input_2d.tensor<float, 2>()(Row, iDepth) = 1.0;
            for (int iTS = 0; iTS < TS_max; iTS++) {
                ch_input_2d.tensor<float, 2>()(Row, iDepth * TS_max + iTS) = ChargeVec.at(iTS);
            }
        }

        ty_input_2d.tensor<float, 2>()(Row, 0) = fabs(ieta_iphi_vec.at(Row).first);
    }
}

void DLPHIN::save_outputs (HBHERecHitCollection *RecHits,
                          const std::vector<int_int_pair>& HB_ieta_iphi_vec,
                          const std::vector<int_int_pair>& HE_ieta_iphi_vec,
                          const std::vector<tensorflow::Tensor>& HB_outputs_2d,
                          const std::vector<tensorflow::Tensor>& HE_outputs_2d
                          ) {
    for (auto& RecHit : *RecHits) {
        auto Hid = RecHit.id();
        auto Subdet = Hid.subdet();
        auto Depth = Hid.depth();
        auto Ieta = Hid.ieta();
        auto Iphi = Hid.iphi();
        auto IetaIphi = std::make_pair(Ieta, Iphi);

        float pred = 0.0;
        float mask = 0.0;
        float respCorr = 1.0;

        if(Subdet == 1) {
            auto iter = std::find(HB_ieta_iphi_vec.begin(), HB_ieta_iphi_vec.end(), IetaIphi);
            if (iter != HB_ieta_iphi_vec.end()) {
                int Row = iter - HB_ieta_iphi_vec.begin();
                pred = HB_outputs_2d[0].tensor<float, 3>()(Row,0,Depth-1);
                mask = HB_outputs_2d[0].tensor<float, 3>()(Row,1,Depth-1);
            }
        }
        else if(Subdet == 2) {
            auto iter = std::find(HE_ieta_iphi_vec.begin(), HE_ieta_iphi_vec.end(), IetaIphi);
            if (iter != HE_ieta_iphi_vec.end()) {
                int Row = iter - HE_ieta_iphi_vec.begin();
                pred = HE_outputs_2d[0].tensor<float, 3>()(Row,0,Depth-1);
                mask = HE_outputs_2d[0].tensor<float, 3>()(Row,1,Depth-1);
            }
        }
        if(pred < 0) {std::cout << "Error! ReLU outputs < 0" << std::endl;}
        if(mask != 1) {std::cout << "Error! A real channel is masked" << std::endl;}
        if (DLPHIN_apply_respCorr_) {
            respCorr = ieta_depth_respCorr_map.at(std::make_pair(Ieta, Depth));
            if (respCorr <= 0) {std::cout << "Error! A real channel has wrong respCorr" << std::endl;}
        }
        if(DLPHIN_debug_) {
            // Save DLPHIN results in a public member, instead of overwriting the recHits
            DLPHIN_debug_infos.push_back(std::make_pair(pred, respCorr));
        }
        else {
            pred = pred * respCorr;
            if(DLPHIN_truncate_) {
                if(RecHit.energy() <= 0) {pred = 0;}
            }
            RecHit.setEnergy(pred);
        }
    }
}

void DLPHIN::SimHit_run (std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons, HBHERecHitCollection *RecHits) {
    SimHit_debug_infos.clear();
    std::map <int, energy_time_pair> id_info_map;
    make_id_info_map(id_info_map, SimHits, hcons);
    save_outputs (RecHits, id_info_map);
}

void DLPHIN::add_info_to_map(std::map <int, energy_time_pair>& id_info_map, const int& id, const float& energy, const float& time) {
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

void DLPHIN::make_id_info_map(std::map <int, energy_time_pair>& id_info_map, std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons) {
    HcalHitRelabeller SimHitRelabeller(true);
    SimHitRelabeller.setGeometry(hcons);
    SimHitRelabeller.process(SimHits);

    for(const auto& SimHit : SimHits)
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

void DLPHIN::save_outputs (HBHERecHitCollection *RecHits, const std::map <int, energy_time_pair>& id_info_map) {
    for (auto& RecHit : *RecHits) {
        auto hid = RecHit.id();
        auto rawId = hid.rawId();
        //auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        //auto iphi = hid.iphi();

        float pred = 0.0;
        float respCorr = 1.0;

        std::vector<float> SimHitEnergyVec, SimHitTimeVec;
        auto it = id_info_map.find(rawId);
        if (it != id_info_map.end()) {
            SimHitEnergyVec = id_info_map.at(rawId).first;
            SimHitTimeVec = id_info_map.at(rawId).second;
            pred = std::accumulate(SimHitEnergyVec.begin(), SimHitEnergyVec.end(), 0.0);
        }

        if (DLPHIN_apply_respCorr_) {
            respCorr = ieta_depth_respCorr_map.at(std::make_pair(ieta, depth));
            if (respCorr <= 0) {std::cout << "Error! A real channel has wrong respCorr" << std::endl;}
        }
        if(DLPHIN_debug_) {
            // Save SimHit results in a public member, instead of overwriting the recHits
            SimHit_debug_infos.push_back(std::make_pair(SimHitEnergyVec, SimHitTimeVec));
        }
        else {
            pred = pred * respCorr;
            RecHit.setEnergy(pred);
        }
    }
}

//
// const member functions
//

//
// static member functions
//
