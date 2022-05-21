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
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"

#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "SimCalorimetry/HcalSimAlgos/interface/HcalSimParameterMap.h"

#include "TFile.h"
#include "TH1.h"

// forward declarations

class DLPHIN {
public:
    DLPHIN(const edm::ParameterSet& conf);
    ~DLPHIN(){};

    // ---------- const member functions ---------------------

    // ---------- static member functions --------------------

    // ---------- member functions ---------------------------
    void DLPHIN_run (const HcalDbService& DbServ, const HBHEChannelInfoCollection *ChannelInfos, HBHERecHitCollection *RecHits);
    void SimHit_run (std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons, HBHERecHitCollection *RecHits);

    typedef std::pair<float, float> pred_respCorr_pair;
    typedef std::pair<std::vector<float>, std::vector<float>> energy_time_pair;
    std::vector<pred_respCorr_pair> DLPHIN_debug_infos;
    std::vector<energy_time_pair> SimHit_debug_infos;

private:
    // ---------- member data --------------------------------
    const int iphi_max = 72;
    const int TS_max = 8;

    const int HE_ieta_min = 16;
    const int HE_ieta_max = 29;
    const int HE_tot_rows = (HE_ieta_max - HE_ieta_min + 1) * iphi_max * 2;
    const int HE_depth_max = 7;
    const int HE_tot_col = TS_max * HE_depth_max;

    const int HB_ieta_min = 1;
    const int HB_ieta_max = 16;
    const int HB_tot_rows = (HB_ieta_max - HB_ieta_min + 1) * iphi_max * 2;
    const int HB_depth_max = 4;
    const int HB_tot_col = TS_max * HB_depth_max;

    typedef std::pair<int, int> int_int_pair;
    std::map <int_int_pair, int> HB_ieta_iphi_row_map, HE_ieta_iphi_row_map;

    bool DLPHIN_debug_;
    bool DLPHIN_print_config_;
    void print_config();

    bool DLPHIN_apply_respCorr_;
    std::string DLPHIN_respCorr_name_;
    std::map <int_int_pair, float> ieta_depth_respCorr_map;

    bool DLPHIN_truncate_;

    std::string DLPHIN_pb_2dHB_, DLPHIN_pb_2dHE_;
    tensorflow::Session *session_2dHB, *session_2dHE;

    void process_inputs (const HcalDbService& DbServ,
                        const HBHEChannelInfoCollection *ChannelInfos,
                        tensorflow::Tensor& HB_ch_input_2d,
                        tensorflow::Tensor& HB_ty_input_2d,
                        tensorflow::Tensor& HB_ma_input_2d,
                        tensorflow::Tensor& HE_ch_input_2d,
                        tensorflow::Tensor& HE_ty_input_2d,
                        tensorflow::Tensor& HE_ma_input_2d
                        );

    void save_outputs (HBHERecHitCollection *RecHits,
                      const std::vector<tensorflow::Tensor>& HB_outputs_2d,
                      const std::vector<tensorflow::Tensor>& HE_outputs_2d
                      );

    float MaxSimHitTime_;
    HcalSimParameterMap HcalSimParameterMap_;

    void add_info_to_map(std::map <int, energy_time_pair>& id_info_map, const int& id, const float& energy, const float& time);

    void make_id_info_map(std::map <int, energy_time_pair>& id_info_map, std::vector<PCaloHit>& SimHits, const HcalDDDRecConstants *hcons);

    void save_outputs (HBHERecHitCollection *RecHits, const std::map <int, energy_time_pair>& id_info_map);
};

#endif
