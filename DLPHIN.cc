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
    DLPHIN_pb_2dHB_(conf.getParameter<std::string>("DLPHIN_pb_2dHB")),
    DLPHIN_pb_2dHE_(conf.getParameter<std::string>("DLPHIN_pb_2dHE"))
{
    int HB_row = 0;
    for(int ieta = -HB_ieta_max; ieta <= -HB_ieta_min; ieta ++)
    {
        for(int iphi = 1; iphi <= iphi_max ; iphi ++)
        {
            HB_ieta_iphi_row_map[std::make_pair(ieta, iphi)] = HB_row;
            HB_row++;
        }
    }
    for(int ieta = HB_ieta_min; ieta <= HB_ieta_max; ieta ++)
    {
        for(int iphi = 1; iphi <= iphi_max; iphi ++)
        {
            HB_ieta_iphi_row_map[std::make_pair(ieta, iphi)] = HB_row;
            HB_row++;
        }
    }

    int HE_row = 0;
    for(int ieta = -HE_ieta_max; ieta <= -HE_ieta_min; ieta ++)
    {
        for(int iphi = 1; iphi <= iphi_max ; iphi ++)
        {
            HE_ieta_iphi_row_map[std::make_pair(ieta, iphi)] = HE_row;
            HE_row++;
        }
    }
    for(int ieta = HE_ieta_min; ieta <= HE_ieta_max; ieta ++)
    {
        for(int iphi = 1; iphi <= iphi_max; iphi ++)
        {
            HE_ieta_iphi_row_map[std::make_pair(ieta, iphi)] = HE_row;
            HE_row++;
        }
    }

    tensorflow::GraphDef *graphDef_2dHB = tensorflow::loadGraphDef(DLPHIN_pb_2dHB_);
    session_2dHB = tensorflow::createSession(graphDef_2dHB);
    tensorflow::GraphDef *graphDef_2dHE = tensorflow::loadGraphDef(DLPHIN_pb_2dHE_);
    session_2dHE = tensorflow::createSession(graphDef_2dHE);
}
//
// member functions
//
void DLPHIN::DLPHIN_run (const HcalDbService& DbServ, const HBHEChannelInfoCollection *ChannelInfos, HBHERecHitCollection *RecHits) {
    //std::cout << "nChannelInfos " << ChannelInfos->size() << ", nRecHits " << RecHits->size() << std::endl;

    //=========== a test for 2d tensor, to be commented out ===================
    /*
    tensorflow::Tensor test_empty(tensorflow::DT_FLOAT, {2, 10});
    tensorflow::Tensor test_2d = test_empty;
    for (int i = 0; i < 20; i++)
    {
       test_2d.flat<float>()(i) = float(i);
    }
    for (int i = 0; i < 2; i++)
    {
       for(int j = 0; j < 10; j++)
       {
            std::cout << i << ", " << j << ": " << test_empty.tensor<float, 2>()(i,j) << ", " << test_2d.tensor<float, 2>()(i,j) << std::endl;
        }
    }
    */
    //======================== end of test ====================================

    //Initialize DLPHIN inputs tensors
    //ch_input: (charge - pedestal) * rawgain
    //ty_input: |ieta|
    //ma_input: whether a channel is real or a place holder
    tensorflow::Tensor HB_ch_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, HB_tot_col});
    for (int i = 0; i < HB_tot_rows * HB_tot_col; i++) {HB_ch_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor HB_ty_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, 1});
    for (int i = 0; i < HB_tot_rows; i++) {HB_ty_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor HB_ma_input_2d(tensorflow::DT_FLOAT, {HB_tot_rows, HB_depth_max});
    for (int i = 0; i < HB_tot_rows * HB_depth_max; i++) {HB_ma_input_2d.flat<float>()(i) = 0.0;}

    tensorflow::Tensor HE_ch_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, HE_tot_col});
    for (int i = 0; i < HE_tot_rows * HE_tot_col; i++) {HE_ch_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor HE_ty_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, 1});
    for (int i = 0; i < HE_tot_rows; i++) {HE_ty_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor HE_ma_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, HE_depth_max});
    for (int i = 0; i < HE_tot_rows * HE_depth_max; i++) {HE_ma_input_2d.flat<float>()(i) = 0.0;}

    //Fill DLPHIN inputs tensors
    process_inputs (DbServ, ChannelInfos,
                    HB_ch_input_2d, HB_ty_input_2d, HB_ma_input_2d,
                    HE_ch_input_2d, HE_ty_input_2d, HE_ma_input_2d);

    //DLPHIN inference
    //outputs is a vector of one rank-3 tensor [pred:mask:depth]
    std::vector<tensorflow::Tensor> HB_outputs_2d, HE_outputs_2d;
    tensorflow::run(session_2dHB, {{"net_charges",HB_ch_input_2d},{"types_input",HB_ty_input_2d},{"mask_input",HB_ma_input_2d}}, {"output/Reshape"}, &HB_outputs_2d);
    tensorflow::run(session_2dHE, {{"net_charges",HE_ch_input_2d},{"types_input",HE_ty_input_2d},{"mask_input",HE_ma_input_2d}}, {"output/Reshape"}, &HE_outputs_2d);
    //std::cout << "HB_outputs_2d.size() " << HB_outputs_2d.size() << ", tensor.shape().dim_size(0) " << HB_outputs_2d[0].shape().dim_size(0) << ", tensor.shape().dim_size(1) " << HB_outputs_2d[0].shape().dim_size(1) << ", tensor.shape().dim_size(2) " << HB_outputs_2d[0].shape().dim_size(2) << std::endl;   

    //Save DLPHIN outputs to RecHits
    save_outputs (RecHits, HB_outputs_2d, HE_outputs_2d);
}

void DLPHIN::process_inputs (const HcalDbService& DbServ,
                            const HBHEChannelInfoCollection *ChannelInfos,
                            tensorflow::Tensor& HB_ch_input_2d,
                            tensorflow::Tensor& HB_ty_input_2d,
                            tensorflow::Tensor& HB_ma_input_2d,
                            tensorflow::Tensor& HE_ch_input_2d,
                            tensorflow::Tensor& HE_ty_input_2d,
                            tensorflow::Tensor& HE_ma_input_2d
                            ) {
    int nChannelInfos = ChannelInfos->size();
    for (int iChannelInfo = 0; iChannelInfo < nChannelInfos; iChannelInfo++) {
        auto ChannelInfo = (*ChannelInfos)[iChannelInfo];
        auto hid = ChannelInfo.id();
        //auto rawId = hid.rawId();
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        auto iphi = hid.iphi();

        const HcalCalibrations& calib = DbServ.getHcalCalibrations(hid);
        auto rawgain = calib.rawgain(0);

        /*
        bool rawgain_check = (calib.rawgain(0) == calib.rawgain(1) &&
                            calib.rawgain(0) == calib.rawgain(2) &&
                            calib.rawgain(0) == calib.rawgain(3));
        std::cout << hid << ": " << rawgain_check << std::endl;
        */

        int Col = (depth - 1) * 8;
        int HB_row = -1;
        int HE_row = -1;
        if(subdet == 1)
        {
            HB_row = HB_ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            HB_ty_input_2d.tensor<float, 2>()(HB_row, 0) = fabs(ieta);
            HB_ma_input_2d.tensor<float, 2>()(HB_row, depth - 1) = 1.0;
        }
        else if(subdet == 2)
        {
            HE_row = HE_ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            HE_ty_input_2d.tensor<float, 2>()(HE_row, 0) = fabs(ieta);
            HE_ma_input_2d.tensor<float, 2>()(HE_row, depth - 1) = 1.0;
        }

        for (int iTS = 0; iTS < TS_max; ++iTS)
        {
            auto charge = ChannelInfo.tsRawCharge(iTS);
            auto ped = ChannelInfo.tsPedestal(iTS);
            float ch_input_TS = (charge - ped) * rawgain;

            if(subdet == 1)
            {HB_ch_input_2d.tensor<float, 2>()(HB_row, Col + iTS) = ch_input_TS;}
            else if(subdet == 2)
            {HE_ch_input_2d.tensor<float, 2>()(HE_row, Col + iTS) = ch_input_TS;}
        }
    }
}

void DLPHIN::save_outputs (HBHERecHitCollection *RecHits,
                          const std::vector<tensorflow::Tensor>& HB_outputs_2d,
                          const std::vector<tensorflow::Tensor>& HE_outputs_2d
                          ){
    int nRecHits = RecHits->size();
    for (int iRecHit = 0; iRecHit < nRecHits; iRecHit++) {
        auto RecHit = (*RecHits)[iRecHit];
        auto hid = RecHit.id();
        //auto rawId = hid.rawId();
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        auto iphi = hid.iphi();

        float pred = 0.0;
        float mask = 0.0;
        if(subdet == 1)
        {
            auto HB_row = HB_ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            pred = HB_outputs_2d[0].tensor<float, 3>()(HB_row,0,depth-1);
            mask = HB_outputs_2d[0].tensor<float, 3>()(HB_row,1,depth-1);
        }
        else if(subdet == 2)
        {
            auto HE_row = HE_ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            pred = HE_outputs_2d[0].tensor<float, 3>()(HE_row,0,depth-1);
            mask = HE_outputs_2d[0].tensor<float, 3>()(HE_row,1,depth-1);
        }
        //if(pred < 0) {std::cout << "Error! ReLU outputs < 0" << std::endl;}
        if(mask != 1) {std::cout << "Error! A real channel is masked" << std::endl;}
        //if(subdet == 2) std::cout << hid << ": " << RecHit.energy() << ", " << pred << std::endl;
        (*RecHits)[iRecHit].setEnergy(pred);
    }
}

//
// const member functions
//

//
// static member functions
//
