// -*- C++ -*-
//
// Package:    RecoLocalCalo/HcalRecProducers
// Class:      HBHEPhase1Reconstructor
// 
/**\class HBHEPhase1Reconstructor HBHEPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/plugins/HBHEPhase1Reconstructor.cc

 Description: Phase 1 reconstruction module for HB/HE

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Igor Volobouev
//         Created:  Tue, 21 Jun 2016 00:56:40 GMT
//
//


// system include files
#include <cmath>
#include <utility>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/METReco/interface/HcalPhase1FlagLabels.h"

#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"

#include "CalibFormats/CaloObjects/interface/CaloSamples.h"

#include "CalibCalorimetry/HcalAlgos/interface/HcalSiPMnonlinearity.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHEStatusBitSetter.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHEPulseShapeFlag.h"

#include "CondFormats/HcalObjects/interface/HBHENegativeEFilter.h"
#include "CondFormats/DataRecord/interface/HBHENegativeEFilterRcd.h"

// Parser for Phase 1 HB/HE reco algorithms
#include "RecoLocalCalo/HcalRecAlgos/interface/parseHBHEPhase1AlgoDescription.h"

// Fetcher for reco algorithm data
#include "RecoLocalCalo/HcalRecAlgos/interface/fetchHcalAlgoData.h"

#include "TFile.h"
#include "TProfile2D.h"
#include "TF1.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
// Some helper functions
namespace {
    // Class for making SiPM/QIE11 look like HPD/QIE8. HPD/QIE8
    // needs only pedestal and gain to convert charge into energy.
    // Due to nonlinearities, response of SiPM/QIE11 is substantially
    // more complicated. It is possible to calculate all necessary
    // quantities from the charge and the info stored in the DB every
    // time the raw charge is needed. However, it does not make sense
    // to retrieve DB contents stored by channel for every time slice.
    // Because of this, we look things up only once, in the constructor.
    template<class DFrame>
    class RawChargeFromSample
    {
    public:
        inline RawChargeFromSample(const int sipmQTSShift,
                                   const int sipmQNTStoSum,
                                   const HcalDbService& cond,
                                   const HcalDetId id,
                                   const CaloSamples& cs,
                                   const int soi,
                                   const DFrame& frame,
                                   const int maxTS) {}

        inline double getRawCharge(const double decodedCharge,
                                   const double pedestal) const
            {return decodedCharge;}
    };

    template<>
    class RawChargeFromSample<QIE11DataFrame>
    {
    public:
        inline RawChargeFromSample(const int sipmQTSShift,
                                   const int sipmQNTStoSum,
                                   const HcalDbService& cond,
                                   const HcalDetId id,
                                   const CaloSamples& cs,
                                   const int soi,
                                   const QIE11DataFrame& frame,
                                   const int maxTS)
            : siPMParameter_(*cond.getHcalSiPMParameter(id)),
              fcByPE_(siPMParameter_.getFCByPE()),
              corr_(cond.getHcalSiPMCharacteristics()->getNonLinearities(siPMParameter_.getType()))
        {
            if (fcByPE_ <= 0.0)
                throw cms::Exception("HBHEPhase1BadDB")
                    << "Invalid fC/PE conversion factor for SiPM " << id
                    << std::endl;

            const HcalCalibrations& calib = cond.getHcalCalibrations(id);
            const int firstTS = std::max(soi + sipmQTSShift, 0);
            const int lastTS = std::min(firstTS + sipmQNTStoSum, maxTS);
            double sipmQ = 0.0;

            for (int ts = firstTS; ts < lastTS; ++ts)
            {
                const double pedestal = calib.pedestal(frame[ts].capid());
                sipmQ += (cs[ts] - pedestal);
            }

            const double effectivePixelsFired = sipmQ/fcByPE_;
            factor_ = corr_.getRecoCorrectionFactor(effectivePixelsFired);
        }

        inline double getRawCharge(const double decodedCharge,
                                   const double pedestal) const
        {
            return (decodedCharge - pedestal)*factor_ + pedestal;

            // Old version of TS-by-TS corrections looked as follows:
            // const double sipmQ = decodedCharge - pedestal;
            // const double nPixelsFired = sipmQ/fcByPE_;
            // return sipmQ*corr_.getRecoCorrectionFactor(nPixelsFired) + pedestal;
       }

    private:
        const HcalSiPMParameter& siPMParameter_;
        double fcByPE_;
        HcalSiPMnonlinearity corr_;
        double factor_;
    };

    float getTDCTimeFromSample(const QIE11DataFrame::Sample& s)
    {
        return HcalSpecialTimes::getTDCTime(s.tdc());
    }

    float getTDCTimeFromSample(const HcalQIESample&)
    {
        return HcalSpecialTimes::UNKNOWN_T_NOTDC;
    }

    float getDifferentialChargeGain(const HcalQIECoder& coder,
                                    const HcalQIEShape& shape,
                                    const unsigned adc,
                                    const unsigned capid,
                                    const bool isQIE11)
    {
        // We have 5-bit ADC mantissa in QIE8 and 6-bit in QIE11
        static const unsigned mantissaMaskQIE8 = 0x1f;
        static const unsigned mantissaMaskQIE11 = 0x3f;

        const float q = coder.charge(shape, adc, capid);
        const unsigned mantissaMask = isQIE11 ? mantissaMaskQIE11 : mantissaMaskQIE8;
        const unsigned mantissa = adc & mantissaMask;

        // First, check if we are in the two lowest or two highest ADC
        // values for this range. Assume that they have the lowest and
        // the highest gain in the range, respectively.
        if (mantissa == 0U || mantissa == mantissaMask - 1U)
            return coder.charge(shape, adc+1U, capid) - q;
        else if (mantissa == 1U || mantissa == mantissaMask)
            return q - coder.charge(shape, adc-1U, capid);
        else
        {
            const float qup = coder.charge(shape, adc+1U, capid);
            const float qdown = coder.charge(shape, adc-1U, capid);
            const float upGain = qup - q;
            const float downGain = q - qdown;
            const float averageGain = (qup - qdown)/2.f;
            if (std::abs(upGain - downGain) < 0.01f*averageGain)
                return averageGain;
            else
            {
                // We are in the gain transition region.
                // Need to determine if we are in the lower
                // gain ADC count or in the higher one.
                // This can be done by figuring out if the
                // "up" gain is more consistent then the
                // "down" gain.
                const float q2up = coder.charge(shape, adc+2U, capid);
                const float q2down = coder.charge(shape, adc-2U, capid);
                const float upGain2 = q2up - qup;
                const float downGain2 = qdown - q2down;
                if (std::abs(upGain2 - upGain) < std::abs(downGain2 - downGain))
                    return upGain;
                else
                    return downGain;
            }
        }
    }

    // The first element of the pair indicates presence of optical
    // link errors. The second indicated presence of capid errors.
    std::pair<bool,bool> findHWErrors(const HBHEDataFrame& df,
                                      const unsigned len)
    {
        bool linkErr = false;
        bool capidErr = false;
        if (len)
        {
            int expectedCapid = df[0].capid();
            for (unsigned i=0; i<len; ++i)
            {
                if (df[i].er())
                    linkErr = true;
                if (df[i].capid() != expectedCapid)
                    capidErr = true;
                expectedCapid = (expectedCapid + 1) % 4;
            }
        }
        return std::pair<bool,bool>(linkErr, capidErr);
    }

    std::pair<bool,bool> findHWErrors(const QIE11DataFrame& df,
                                      const unsigned /* len */)
    {
        return std::pair<bool,bool>(df.linkError(), df.capidError());
    }

    std::unique_ptr<HBHEStatusBitSetter> parse_HBHEStatusBitSetter(
        const edm::ParameterSet& psdigi)
    {
        return std::make_unique<HBHEStatusBitSetter>(
            psdigi.getParameter<double>("nominalPedestal"),
            psdigi.getParameter<double>("hitEnergyMinimum"),
            psdigi.getParameter<int>("hitMultiplicityThreshold"),
            psdigi.getParameter<std::vector<edm::ParameterSet> >("pulseShapeParameterSets"));
    }

    std::unique_ptr<HBHEPulseShapeFlagSetter> parse_HBHEPulseShapeFlagSetter(
        const edm::ParameterSet& psPulseShape, const bool setLegacyFlags)
    {
        return std::make_unique<HBHEPulseShapeFlagSetter>(
            psPulseShape.getParameter<double>("MinimumChargeThreshold"),
            psPulseShape.getParameter<double>("TS4TS5ChargeThreshold"),
            psPulseShape.getParameter<double>("TS3TS4ChargeThreshold"),
            psPulseShape.getParameter<double>("TS3TS4UpperChargeThreshold"),
            psPulseShape.getParameter<double>("TS5TS6ChargeThreshold"),
            psPulseShape.getParameter<double>("TS5TS6UpperChargeThreshold"),
            psPulseShape.getParameter<double>("R45PlusOneRange"),
            psPulseShape.getParameter<double>("R45MinusOneRange"),
            psPulseShape.getParameter<unsigned int>("TrianglePeakTS"),
            psPulseShape.getParameter<std::vector<double> >("LinearThreshold"),
            psPulseShape.getParameter<std::vector<double> >("LinearCut"),
            psPulseShape.getParameter<std::vector<double> >("RMS8MaxThreshold"),
            psPulseShape.getParameter<std::vector<double> >("RMS8MaxCut"),
            psPulseShape.getParameter<std::vector<double> >("LeftSlopeThreshold"),
            psPulseShape.getParameter<std::vector<double> >("LeftSlopeCut"),
            psPulseShape.getParameter<std::vector<double> >("RightSlopeThreshold"),
            psPulseShape.getParameter<std::vector<double> >("RightSlopeCut"),
            psPulseShape.getParameter<std::vector<double> >("RightSlopeSmallThreshold"),
            psPulseShape.getParameter<std::vector<double> >("RightSlopeSmallCut"),
            psPulseShape.getParameter<std::vector<double> >("TS4TS5LowerThreshold"),
            psPulseShape.getParameter<std::vector<double> >("TS4TS5LowerCut"),
            psPulseShape.getParameter<std::vector<double> >("TS4TS5UpperThreshold"),
            psPulseShape.getParameter<std::vector<double> >("TS4TS5UpperCut"),
            psPulseShape.getParameter<bool>("UseDualFit"),
            psPulseShape.getParameter<bool>("TriangleIgnoreSlow"),
            setLegacyFlags);
    }
}


//
// class declaration
//
class HBHEPhase1Reconstructor : public edm::stream::EDProducer<>
{
public:
    explicit HBHEPhase1Reconstructor(const edm::ParameterSet&);
    ~HBHEPhase1Reconstructor() override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void beginRun(edm::Run const&, edm::EventSetup const&) override;
    void endRun(edm::Run const&, edm::EventSetup const&) override;
    void produce(edm::Event&, const edm::EventSetup&) override;

    // Configuration parameters
    std::string algoConfigClass_;
    bool processQIE8_;
    bool processQIE11_;
    bool saveInfos_;
    bool saveDroppedInfos_;
    bool makeRecHits_;
    bool dropZSmarkedPassed_;
    bool tsFromDB_;
    bool recoParamsFromDB_;
    bool saveEffectivePedestal_;
    bool use8ts_;
    int sipmQTSShift_;
    int sipmQNTStoSum_;

    // Parameters for turning status bit setters on/off
    bool setNegativeFlagsQIE8_;
    bool setNegativeFlagsQIE11_;
    bool setNoiseFlagsQIE8_;
    bool setNoiseFlagsQIE11_;
    bool setPulseShapeFlagsQIE8_;
    bool setPulseShapeFlagsQIE11_;

    // DLPHIN parameters
    std::string DLPHIN_pb_d1HB_, DLPHIN_pb_dg1HB_, DLPHIN_pb_d1HE_, DLPHIN_pb_dg1HE_, DLPHIN_pb_SF_, DLPHIN_pb_2dHE_;
    bool DLPHIN_scale_, DLPHIN_save_, DLPHIN_apply_respCorr_, DLPHIN_truncate_, DLPHIN_print_1d_, DLPHIN_print_2d_;

    // Other members
    edm::EDGetTokenT<HBHEDigiCollection> tok_qie8_;
    edm::EDGetTokenT<QIE11DigiCollection> tok_qie11_;
    std::unique_ptr<AbsHBHEPhase1Algo> reco_;
    std::unique_ptr<AbsHcalAlgoData> recoConfig_;
    std::unique_ptr<HcalRecoParams> paramTS_;

    // Struct for DLPHIN
    tensorflow::Session *session_d1HB, *session_dg1HB, *session_d1HE, *session_dg1HE;
    tensorflow::Session *session_2dHE;
    TProfile2D *ratio_HB, *ratio_HE;
    //TF1 *DLPHIN_rand;
    struct DLPHIN_input {HBHEChannelInfo channel_info; double resp_corr; HBHERecHit rec_hit;};
    std::vector<DLPHIN_input> DLPHIN_input_vec;
    std::vector<float> DLPHIN_output;

    // Status bit setters
    const HBHENegativeEFilter* negEFilter_;    // We don't manage this pointer
    std::unique_ptr<HBHEStatusBitSetter> hbheFlagSetterQIE8_;
    std::unique_ptr<HBHEStatusBitSetter> hbheFlagSetterQIE11_;
    std::unique_ptr<HBHEPulseShapeFlagSetter> hbhePulseShapeFlagSetterQIE8_;
    std::unique_ptr<HBHEPulseShapeFlagSetter> hbhePulseShapeFlagSetterQIE11_;

    // For the function below, arguments "infoColl" and/or "rechits"
    // are allowed to be null.
    template<class DataFrame, class Collection>
    void processData(const Collection& coll,
                     const HcalDbService& cond,
                     const HcalChannelQuality& qual,
                     const HcalSeverityLevelComputer& severity,
                     const bool isRealData,
                     HBHEChannelInfo* info,
                     HBHEChannelInfoCollection* infoColl,
                     HBHERecHitCollection* rechits,
                     const bool use8ts);

    // Function to run DLPHIN
    void run_dlphin(std::vector<DLPHIN_input> Dinput_vec, std::vector<float>& Doutput);
    void save_dlphin(std::vector<float> Doutput, HBHERecHitCollection* rechits);

    // Methods for setting rechit status bits
    void setAsicSpecificBits(const HBHEDataFrame& frame, const HcalCoder& coder,
                             const HBHEChannelInfo& info, const HcalCalibrations& calib,
                             HBHERecHit* rh);
    void setAsicSpecificBits(const QIE11DataFrame& frame, const HcalCoder& coder,
                             const HBHEChannelInfo& info, const HcalCalibrations& calib,
                             HBHERecHit* rh);
    void setCommonStatusBits(const HBHEChannelInfo& info, const HcalCalibrations& calib,
                             HBHERecHit* rh);

    void runHBHENegativeEFilter(const HBHEChannelInfo& info, HBHERecHit* rh);
};

//
// constructors and destructor
//
HBHEPhase1Reconstructor::HBHEPhase1Reconstructor(const edm::ParameterSet& conf)
    : algoConfigClass_(conf.getParameter<std::string>("algoConfigClass")),
      processQIE8_(conf.getParameter<bool>("processQIE8")),
      processQIE11_(conf.getParameter<bool>("processQIE11")),
      saveInfos_(conf.getParameter<bool>("saveInfos")),
      saveDroppedInfos_(conf.getParameter<bool>("saveDroppedInfos")),
      makeRecHits_(conf.getParameter<bool>("makeRecHits")),
      dropZSmarkedPassed_(conf.getParameter<bool>("dropZSmarkedPassed")),
      tsFromDB_(conf.getParameter<bool>("tsFromDB")),
      recoParamsFromDB_(conf.getParameter<bool>("recoParamsFromDB")),
      saveEffectivePedestal_(conf.getParameter<bool>("saveEffectivePedestal")),
      use8ts_(conf.getParameter<bool>("use8ts")),
      sipmQTSShift_(conf.getParameter<int>("sipmQTSShift")),
      sipmQNTStoSum_(conf.getParameter<int>("sipmQNTStoSum")),
      setNegativeFlagsQIE8_(conf.getParameter<bool>("setNegativeFlagsQIE8")),
      setNegativeFlagsQIE11_(conf.getParameter<bool>("setNegativeFlagsQIE11")),
      setNoiseFlagsQIE8_(conf.getParameter<bool>("setNoiseFlagsQIE8")),
      setNoiseFlagsQIE11_(conf.getParameter<bool>("setNoiseFlagsQIE11")),
      setPulseShapeFlagsQIE8_(conf.getParameter<bool>("setPulseShapeFlagsQIE8")),
      setPulseShapeFlagsQIE11_(conf.getParameter<bool>("setPulseShapeFlagsQIE11")),
      DLPHIN_pb_d1HB_(conf.getParameter<std::string>("DLPHIN_pb_d1HB")),
      DLPHIN_pb_dg1HB_(conf.getParameter<std::string>("DLPHIN_pb_dg1HB")),
      DLPHIN_pb_d1HE_(conf.getParameter<std::string>("DLPHIN_pb_d1HE")),
      DLPHIN_pb_dg1HE_(conf.getParameter<std::string>("DLPHIN_pb_dg1HE")),
      DLPHIN_pb_SF_(conf.getParameter<std::string>("DLPHIN_pb_SF")),
      DLPHIN_pb_2dHE_(conf.getParameter<std::string>("DLPHIN_pb_2dHE")),
      DLPHIN_scale_(conf.getParameter<bool>("DLPHIN_scale")),
      DLPHIN_save_(conf.getParameter<bool>("DLPHIN_save")),
      DLPHIN_apply_respCorr_(conf.getParameter<bool>("DLPHIN_apply_respCorr")),
      DLPHIN_truncate_(conf.getParameter<bool>("DLPHIN_truncate")),
      DLPHIN_print_1d_(conf.getParameter<bool>("DLPHIN_print_1d")),
      DLPHIN_print_2d_(conf.getParameter<bool>("DLPHIN_print_2d")),
      reco_(parseHBHEPhase1AlgoDescription(conf.getParameter<edm::ParameterSet>("algorithm"))),
      negEFilter_(nullptr)
{
    // Check that the reco algorithm has been successfully configured
    if (!reco_.get())
        throw cms::Exception("HBHEPhase1BadConfig")
            << "Invalid HBHEPhase1Algo algorithm configuration"
            << std::endl;

    // Configure the status bit setters that have been turned on
    if (setNoiseFlagsQIE8_)
        hbheFlagSetterQIE8_ = parse_HBHEStatusBitSetter(
            conf.getParameter<edm::ParameterSet>("flagParametersQIE8"));

    if (setNoiseFlagsQIE11_)
        hbheFlagSetterQIE11_ = parse_HBHEStatusBitSetter(
            conf.getParameter<edm::ParameterSet>("flagParametersQIE11"));

    if (setPulseShapeFlagsQIE8_)
        hbhePulseShapeFlagSetterQIE8_ = parse_HBHEPulseShapeFlagSetter(
            conf.getParameter<edm::ParameterSet>("pulseShapeParametersQIE8"),
            conf.getParameter<bool>("setLegacyFlagsQIE8"));

    if (setPulseShapeFlagsQIE11_)
        hbhePulseShapeFlagSetterQIE11_ = parse_HBHEPulseShapeFlagSetter(
            conf.getParameter<edm::ParameterSet>("pulseShapeParametersQIE11"),
            conf.getParameter<bool>("setLegacyFlagsQIE11"));

    // Consumes and produces statements
    if (processQIE8_)
        tok_qie8_ = consumes<HBHEDigiCollection>(
            conf.getParameter<edm::InputTag>("digiLabelQIE8"));

    if (processQIE11_)
        tok_qie11_ = consumes<QIE11DigiCollection>(
            conf.getParameter<edm::InputTag>("digiLabelQIE11"));

    if (saveInfos_)
        produces<HBHEChannelInfoCollection>();

    if (makeRecHits_)
        produces<HBHERecHitCollection>();

    //std::cout << "HBHEPhase1Reconstructor is called" << std::endl;
    // Keep DLPHIN sessions in memory during process
    tensorflow::GraphDef *graphDef_d1HB = tensorflow::loadGraphDef(DLPHIN_pb_d1HB_);
    session_d1HB = tensorflow::createSession(graphDef_d1HB);

    tensorflow::GraphDef *graphDef_dg1HB = tensorflow::loadGraphDef(DLPHIN_pb_dg1HB_);
    session_dg1HB = tensorflow::createSession(graphDef_dg1HB);

    tensorflow::GraphDef *graphDef_d1HE = tensorflow::loadGraphDef(DLPHIN_pb_d1HE_);
    session_d1HE = tensorflow::createSession(graphDef_d1HE);

    tensorflow::GraphDef *graphDef_dg1HE = tensorflow::loadGraphDef(DLPHIN_pb_dg1HE_);
    session_dg1HE = tensorflow::createSession(graphDef_dg1HE);

    tensorflow::GraphDef *graphDef_2dHE = tensorflow::loadGraphDef(DLPHIN_pb_2dHE_);
    session_2dHE = tensorflow::createSession(graphDef_2dHE);

    //DLPHIN_rand = new TF1("DLPHIN_rand", "1", 0.5, 2);
    
    if(DLPHIN_scale_)
    {
        TFile *ratio_file = new TFile(DLPHIN_pb_SF_.c_str());
        ratio_HB = (TProfile2D*)ratio_file->Get("ratio_ieta_depth_HB_h_pyx");
        ratio_HE = (TProfile2D*)ratio_file->Get("ratio_ieta_depth_HE_h_pyx");
    }
}


HBHEPhase1Reconstructor::~HBHEPhase1Reconstructor()
{
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//
template<class DFrame, class Collection>
void HBHEPhase1Reconstructor::processData(const Collection& coll,
                                          const HcalDbService& cond,
                                          const HcalChannelQuality& qual,
                                          const HcalSeverityLevelComputer& severity,
                                          const bool isRealData,
                                          HBHEChannelInfo* channelInfo,
                                          HBHEChannelInfoCollection* infos,
                                          HBHERecHitCollection* rechits,
                                          const bool use8ts_)
{
    // If "saveDroppedInfos_" flag is set, fill the info with something
    // meaningful even if the database tells us to drop this channel.
    // Note that this flag affects only "infos", the rechits are still
    // not going to be constructed from such channels.
    const bool skipDroppedChannels = !(infos && saveDroppedInfos_);

    // Iterate over the input collection
    for (typename Collection::const_iterator it = coll.begin();
         it != coll.end(); ++it)
    {
        const DFrame& frame(*it);
        const HcalDetId cell(frame.id());

        // Protection against calibration channels which are not
        // in the database but can still come in the QIE11DataFrame
        // in the laser calibs, etc.
        const HcalSubdetector subdet = cell.subdet();
        if (!(subdet == HcalSubdetector::HcalBarrel ||
              subdet == HcalSubdetector::HcalEndcap ||
              subdet == HcalSubdetector::HcalOuter))
            continue;

        // Check if the database tells us to drop this channel
        const HcalChannelStatus* mydigistatus = qual.getValues(cell.rawId());
        const bool taggedBadByDb = severity.dropChannel(mydigistatus->getValue());
        if (taggedBadByDb && skipDroppedChannels)
            continue;

        // Check if the channel is zero suppressed
        bool dropByZS = false;
        if (dropZSmarkedPassed_)
            if (frame.zsMarkAndPass())
                dropByZS = true;
        if (dropByZS && skipDroppedChannels)
            continue;

        // Basic ADC decoding tools
        const HcalRecoParam* param_ts = paramTS_->getValues(cell.rawId());
        const HcalCalibrations& calib = cond.getHcalCalibrations(cell);
        const HcalCalibrationWidths& calibWidth = cond.getHcalCalibrationWidths(cell);
        const HcalQIECoder* channelCoder = cond.getHcalCoder(cell);
        const HcalQIEShape* shape = cond.getHcalShape(channelCoder);
        const HcalCoderDb coder(*channelCoder, *shape);

        // needed for the dark current in the M2
        const HcalSiPMParameter& siPMParameter(*cond.getHcalSiPMParameter(cell));
        const double darkCurrent = siPMParameter.getDarkCurrent();
        const double fcByPE = siPMParameter.getFCByPE();
        const double lambda = cond.getHcalSiPMCharacteristics()->getCrossTalk(siPMParameter.getType());

        // ADC to fC conversion
        CaloSamples cs;
        coder.adc2fC(frame, cs);

        // Prepare to iterate over time slices
        const bool saveEffectivePeds = channelInfo->hasEffectivePedestals();
        const int nRead = cs.size();
        const int maxTS = std::min(nRead, static_cast<int>(HBHEChannelInfo::MAXSAMPLES));
        const int soi = tsFromDB_ ? param_ts->firstSample() : frame.presamples();
        const RawChargeFromSample<DFrame> rcfs(sipmQTSShift_, sipmQNTStoSum_, 
                                               cond, cell, cs, soi, frame, maxTS);
        int soiCapid = 4;
        
        // Use only 8 TSs when there are 10 TSs 
        const int shiftOneTS = use8ts_ && maxTS == static_cast<int>(HBHEChannelInfo::MAXSAMPLES) ? 1 : 0;
        const int nCycles = maxTS - shiftOneTS;

        // Go over time slices and fill the samples
        for (int inputTS = shiftOneTS; inputTS < nCycles; ++inputTS)
        {
            auto s(frame[inputTS]);
            const uint8_t adc = s.adc();
            const int capid = s.capid();
            //optionally store "effective" pedestal (measured with bias voltage on)
            // = QIE contribution + SiPM contribution (from dark current + crosstalk)
            const double pedestal = saveEffectivePeds ? calib.effpedestal(capid) : calib.pedestal(capid);
            const double pedestalWidth = saveEffectivePeds ? calibWidth.effpedestal(capid) : calibWidth.pedestal(capid);
            const double gain = calib.respcorrgain(capid);
            const double gainWidth = calibWidth.gain(capid);
            //always use QIE-only pedestal for this computation
            const double rawCharge = rcfs.getRawCharge(cs[inputTS], calib.pedestal(capid));
            const float t = getTDCTimeFromSample(s);
            const float dfc = getDifferentialChargeGain(*channelCoder, *shape, adc,
                                                        capid, channelInfo->hasTimeInfo()); 
            const int fitTS = inputTS-shiftOneTS;
            channelInfo->setSample(fitTS, adc, dfc, rawCharge,
                                   pedestal, pedestalWidth,
                                   gain, gainWidth, t);
            if (inputTS == soi)
                soiCapid = capid;
        }

        // Fill the overall channel info items 
        const int maxFitTS = maxTS-2*shiftOneTS;
        const int fitSoi = soi-shiftOneTS;
        const int pulseShapeID = param_ts->pulseShapeID();
        const std::pair<bool,bool> hwerr = findHWErrors(frame, maxTS);
        channelInfo->setChannelInfo(cell, pulseShapeID, maxFitTS, fitSoi, soiCapid,
                                    darkCurrent, fcByPE, lambda,
                                    hwerr.first, hwerr.second,
                                    taggedBadByDb || dropByZS);

        // If needed, add the channel info to the output collection
        const bool makeThisRechit = !channelInfo->isDropped();
        if (infos && (saveDroppedInfos_ || makeThisRechit))
            infos->push_back(*channelInfo);

        // Reconstruct the rechit
        if (rechits && makeThisRechit)
        {
            const HcalRecoParam* pptr = nullptr;
            if (recoParamsFromDB_)
                pptr = param_ts;
            HBHERecHit rh = reco_->reconstruct(*channelInfo, pptr, calib, isRealData);
            if (rh.id().rawId())
            {
                setAsicSpecificBits(frame, coder, *channelInfo, calib, &rh);
                setCommonStatusBits(*channelInfo, calib, &rh);
                rechits->push_back(rh);

                // Save DLPHIN_input
                DLPHIN_input_vec.push_back((DLPHIN_input){*channelInfo, calib.respcorr(), rh});
            }
        }
    }
}

void HBHEPhase1Reconstructor::setCommonStatusBits(
    const HBHEChannelInfo& /* info */, const HcalCalibrations& /* calib */,
    HBHERecHit* /* rh */)
{
}

void HBHEPhase1Reconstructor::setAsicSpecificBits(
    const HBHEDataFrame& frame, const HcalCoder& coder,
    const HBHEChannelInfo& info, const HcalCalibrations& calib,
    HBHERecHit* rh)
{
    if (setNoiseFlagsQIE8_)
        hbheFlagSetterQIE8_->rememberHit(*rh);

    if (setPulseShapeFlagsQIE8_)
        hbhePulseShapeFlagSetterQIE8_->SetPulseShapeFlags(*rh, frame, coder, calib);

    if (setNegativeFlagsQIE8_)
        runHBHENegativeEFilter(info, rh);
}

void HBHEPhase1Reconstructor::setAsicSpecificBits(
    const QIE11DataFrame& frame, const HcalCoder& coder,
    const HBHEChannelInfo& info, const HcalCalibrations& calib,
    HBHERecHit* rh)
{
    if (setNoiseFlagsQIE11_)
        hbheFlagSetterQIE11_->rememberHit(*rh);

    if (setPulseShapeFlagsQIE11_)
        hbhePulseShapeFlagSetterQIE11_->SetPulseShapeFlags(*rh, frame, coder, calib);

    if (setNegativeFlagsQIE11_)
        runHBHENegativeEFilter(info, rh);
}

void HBHEPhase1Reconstructor::runHBHENegativeEFilter(const HBHEChannelInfo& info,
                                                     HBHERecHit* rh)
{
    double ts[HBHEChannelInfo::MAXSAMPLES];
    const unsigned nRead = info.nSamples();
    for (unsigned i=0; i<nRead; ++i)
        ts[i] = info.tsCharge(i);
    const bool passes = negEFilter_->checkPassFilter(info.id(), &ts[0], nRead);
    if (!passes)
        rh->setFlagField(1, HcalPhase1FlagLabels::HBHENegativeNoise);
}

// ------------ method called to produce the data  ------------
void
HBHEPhase1Reconstructor::produce(edm::Event& e, const edm::EventSetup& eventSetup)
{
    using namespace edm;

    // Get the Hcal topology
    ESHandle<HcalTopology> htopo;
    eventSetup.get<HcalRecNumberingRecord>().get(htopo);
    paramTS_->setTopo(htopo.product());

    // Fetch the calibrations
    ESHandle<HcalDbService> conditions;
    eventSetup.get<HcalDbRecord>().get(conditions);

    ESHandle<HcalChannelQuality> p;
    eventSetup.get<HcalChannelQualityRcd>().get("withTopo", p);
 
    ESHandle<HcalSeverityLevelComputer> mycomputer;
    eventSetup.get<HcalSeverityLevelComputerRcd>().get(mycomputer);

    // Configure the negative energy filter
    ESHandle<HBHENegativeEFilter> negEHandle;
    if (setNegativeFlagsQIE8_ || setNegativeFlagsQIE11_)
    {
        eventSetup.get<HBHENegativeEFilterRcd>().get(negEHandle);
        negEFilter_ = negEHandle.product();
    }

    // Find the input data
    unsigned maxOutputSize = 0;
    Handle<HBHEDigiCollection> hbDigis;
    if (processQIE8_)
    {
        e.getByToken(tok_qie8_, hbDigis);
        maxOutputSize += hbDigis->size();
    }

    Handle<QIE11DigiCollection> heDigis;
    if (processQIE11_)
    {
        e.getByToken(tok_qie11_, heDigis);
        maxOutputSize += heDigis->size();
    }

    // Create new output collections
    std::unique_ptr<HBHEChannelInfoCollection> infos;
    if (saveInfos_)
    {
        infos = std::make_unique<HBHEChannelInfoCollection>();
        infos->reserve(maxOutputSize);
    }

    std::unique_ptr<HBHERecHitCollection> out;
    if (makeRecHits_)
    {
        out = std::make_unique<HBHERecHitCollection>();
        out->reserve(maxOutputSize);
    }

    //Clear DLPHIN input/output vectors for each event
    DLPHIN_input_vec.clear();
    DLPHIN_output.clear();

    // Process the input collections, filling the output ones
    const bool isData = e.isRealData();
    if (processQIE8_)
    {
        if (setNoiseFlagsQIE8_)
            hbheFlagSetterQIE8_->Clear();

        HBHEChannelInfo channelInfo(false,false);
        processData<HBHEDataFrame>(*hbDigis, *conditions, *p, *mycomputer,
                                   isData, &channelInfo, infos.get(), out.get(), use8ts_);
        if (setNoiseFlagsQIE8_)
            hbheFlagSetterQIE8_->SetFlagsFromRecHits(*out);
    }

    if (processQIE11_)
    {
        if (setNoiseFlagsQIE11_)
            hbheFlagSetterQIE11_->Clear();

        HBHEChannelInfo channelInfo(true,saveEffectivePedestal_);
        processData<QIE11DataFrame>(*heDigis, *conditions, *p, *mycomputer,
                                    isData, &channelInfo, infos.get(), out.get(), use8ts_);
        if (setNoiseFlagsQIE11_)
            hbheFlagSetterQIE11_->SetFlagsFromRecHits(*out);
    }

    // Run DLPHIN
    run_dlphin(DLPHIN_input_vec, DLPHIN_output);
    if(DLPHIN_save_)save_dlphin(DLPHIN_output, out.get());

    // Add the output collections to the event record
    if (saveInfos_)
        e.put(std::move(infos));
    if (makeRecHits_)
        e.put(std::move(out));
}

// ------------ method called when starting to processes a run  ------------
void
HBHEPhase1Reconstructor::beginRun(edm::Run const& r, edm::EventSetup const& es)
{
    edm::ESHandle<HcalRecoParams> p;
    es.get<HcalRecoParamsRcd>().get(p);
    paramTS_ = std::make_unique<HcalRecoParams>(*p.product());

    if (reco_->isConfigurable())
    {
        recoConfig_ = fetchHcalAlgoData(algoConfigClass_, es);
        if (!recoConfig_.get())
            throw cms::Exception("HBHEPhase1BadConfig")
                << "Invalid HBHEPhase1Reconstructor \"algoConfigClass\" parameter value \""
                << algoConfigClass_ << '"' << std::endl;
        if (!reco_->configure(recoConfig_.get()))
            throw cms::Exception("HBHEPhase1BadConfig")
                << "Failed to configure HBHEPhase1Algo algorithm from EventSetup"
                << std::endl;
    }

    if (setNoiseFlagsQIE8_ || setNoiseFlagsQIE11_)
    {
        edm::ESHandle<HcalFrontEndMap> hfemap;
        es.get<HcalFrontEndMapRcd>().get(hfemap);
        if (hfemap.isValid())
        {
            if (setNoiseFlagsQIE8_)
                hbheFlagSetterQIE8_->SetFrontEndMap(hfemap.product());
            if (setNoiseFlagsQIE11_)
                hbheFlagSetterQIE11_->SetFrontEndMap(hfemap.product());
        }
        else
            edm::LogWarning("EventSetup") <<
                "HBHEPhase1Reconstructor failed to get HcalFrontEndMap!" << std::endl;
    }

    reco_->beginRun(r, es);
}

void
HBHEPhase1Reconstructor::endRun(edm::Run const&, edm::EventSetup const&)
{
    reco_->endRun();
}

#define add_param_set(name) /**/       \
    edm::ParameterSetDescription name; \
    name.setAllowAnything();           \
    desc.add<edm::ParameterSetDescription>(#name, name)

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HBHEPhase1Reconstructor::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;

    desc.add<edm::InputTag>("digiLabelQIE8");
    desc.add<edm::InputTag>("digiLabelQIE11");
    desc.add<std::string>("algoConfigClass");
    desc.add<bool>("processQIE8");
    desc.add<bool>("processQIE11");
    desc.add<bool>("saveInfos");
    desc.add<bool>("saveDroppedInfos");
    desc.add<bool>("makeRecHits");
    desc.add<bool>("dropZSmarkedPassed");
    desc.add<bool>("tsFromDB");
    desc.add<bool>("recoParamsFromDB");
    desc.add<bool>("saveEffectivePedestal", false);
    desc.add<bool>("use8ts", false);
    desc.add<int>("sipmQTSShift", 0);
    desc.add<int>("sipmQNTStoSum", 3);
    desc.add<bool>("setNegativeFlagsQIE8");
    desc.add<bool>("setNegativeFlagsQIE11");
    desc.add<bool>("setNoiseFlagsQIE8");
    desc.add<bool>("setNoiseFlagsQIE11");
    desc.add<bool>("setPulseShapeFlagsQIE8");
    desc.add<bool>("setPulseShapeFlagsQIE11");
    desc.add<bool>("setLegacyFlagsQIE8");
    desc.add<bool>("setLegacyFlagsQIE11");
    desc.add<std::string>("DLPHIN_pb_d1HB");
    desc.add<std::string>("DLPHIN_pb_dg1HB");
    desc.add<std::string>("DLPHIN_pb_d1HE");
    desc.add<std::string>("DLPHIN_pb_dg1HE");
    desc.add<std::string>("DLPHIN_pb_SF");
    desc.add<std::string>("DLPHIN_pb_2dHE");
    desc.add<bool>("DLPHIN_scale");
    desc.add<bool>("DLPHIN_save");
    desc.add<bool>("DLPHIN_apply_respCorr");
    desc.add<bool>("DLPHIN_truncate");
    desc.add<bool>("DLPHIN_print_1d");
    desc.add<bool>("DLPHIN_print_2d");

    desc.add<edm::ParameterSetDescription>("algorithm", fillDescriptionForParseHBHEPhase1Algo());
    add_param_set(flagParametersQIE8);
    add_param_set(flagParametersQIE11);
    add_param_set(pulseShapeParametersQIE8);
    add_param_set(pulseShapeParametersQIE11);
    
    descriptions.addDefault(desc);
}

void HBHEPhase1Reconstructor::run_dlphin(std::vector<DLPHIN_input> Dinput_vec, std::vector<float>& Doutput)
{
    const int HE_depth_max = 7;
    const int HE_ieta_min = 16;
    const int HE_ieta_max = 29;
    const int HE_tot_rows = (HE_ieta_max - HE_ieta_min + 1) * 2 * 72;

    tensorflow::Tensor ch_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, 56});
    for (int i = 0; i < HE_tot_rows * 56; i++) {ch_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor ty_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, 1});
    for (int i = 0; i < HE_tot_rows; i++) {ty_input_2d.flat<float>()(i) = 0.0;}
    tensorflow::Tensor ma_input_2d(tensorflow::DT_FLOAT, {HE_tot_rows, HE_depth_max});
    for (int i = 0; i < HE_tot_rows * HE_depth_max; i++) {ma_input_2d.flat<float>()(i) = 0.0;}

    //=========== a test for 2d tensor, to be commented out ===================
    /*
    tensorflow::Tensor test_2d(tensorflow::DT_FLOAT, {2, 10});
    for (int i = 0; i < 20; i++)
    {
        test_2d.flat<float>()(i) = float(i);
    }
    for (int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 10; j++)
        {std::cout << i << ", " << j << ": " << test_2d.tensor<float, 2>()(i,j) << std::endl;}
    }
    */
    //======================== end of test ====================================

    typedef std::pair<int, int> ieta_iphi_pair;
    std::map <ieta_iphi_pair, std::vector<std::vector<float>>> ieta_iphi_energy_map;
    std::vector<float> channel_vec(23, 0.0);
    std::vector<std::vector<float>> depth_vec(HE_depth_max, channel_vec);
    std::map <ieta_iphi_pair, int> ieta_iphi_row_map;
    int row = 0;
    for(int ieta = -HE_ieta_max; ieta <= -HE_ieta_min; ieta ++)
    {   
        for(int iphi = 1; iphi <= 72; iphi ++)
        {   
            ieta_iphi_energy_map[std::make_pair(ieta, iphi)] = depth_vec;
            ieta_iphi_row_map[std::make_pair(ieta, iphi)] = row;
            row++;
        }
    }
    for(int ieta = HE_ieta_min; ieta <= HE_ieta_max; ieta ++)
    {
        for(int iphi = 1; iphi <= 72; iphi ++)
        {
            ieta_iphi_energy_map[std::make_pair(ieta, iphi)] = depth_vec;
            ieta_iphi_row_map[std::make_pair(ieta, iphi)] = row;
            row++;
        }
    }

    /*
    for(auto iter : ieta_iphi_row_map)
    {
        auto key = iter.first;
        auto value = iter.second;
        std::cout << key.first << ", " << key.second << ", " << value << std::endl;
    }
    */

    if(DLPHIN_print_1d_)
    {
        std::string title_string = "reco: ";
        for (int TS = 1; TS <= 8; TS++)
        {
            std::string TS_string = "TS" + std::to_string(TS);
            title_string = title_string + TS_string + " raw charge, " + TS_string + " ped noise, "; 
        }

        title_string = title_string + "raw gain, gain, raw energy, aux energy, mahi energy, flags,";
        title_string = title_string + " id, sub detector, depth, ieta, iphi, DLPHIN energy, DLPHIN_SF";

        std::cout << title_string << std::endl;
    }

    // first loop over all channels for
    // 1. channel by channel prediction by DLPHIN 1d CNN
    // 2. prepare inputs for DLPHIN 2d CNN
    for(auto iter : Dinput_vec)
    {
        auto rec_hit = iter.rec_hit;
        auto hid = rec_hit.id();
        auto rawId = hid.rawId();
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        auto iphi = hid.iphi();

        auto channel_info = iter.channel_info;
        int nSamples = channel_info.nSamples();
        auto gain = channel_info.tsGain(0);

        auto resp_corr = iter.resp_corr;
        float rawgain = gain / resp_corr;

        tensorflow::Tensor ch_input(tensorflow::DT_FLOAT, {1, 8}); //2d tensor of [[TS1, ..., TS8]]
        tensorflow::Tensor ty_input(tensorflow::DT_FLOAT, {1, 2}); //2d tensor of [[depth, ieta]]

        //============= test un-reconstructed channels =========================
        /*
        if(depth == 1 && ieta == -19 && (iphi == 24 || iphi == 23))
        {
            std::cout << "find channel " << rawId << ", depth " << depth << ", ieta " << ieta << ", iphi " << iphi << std::endl;
        }
        */
        //============= test end ===============================================

        int HE_row = -1;
        int HE_col = -1;
        if(subdet == 2)
        {
            HE_row = ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            HE_col = (depth - 1) * 8; 
            ty_input_2d.tensor<float, 2>()(HE_row, 0) = fabs(ieta);
            ma_input_2d.tensor<float, 2>()(HE_row, depth - 1) = 1.0;
        }

        for (int iTS = 0; iTS < nSamples; ++iTS)
        {
            auto charge = channel_info.tsRawCharge(iTS);
            auto ped = channel_info.tsPedestal(iTS);
            //auto rise_time = channel_info.tsRiseTime(iTS);
            float ch_input_TS = (charge - ped) * rawgain;

            ch_input.tensor<float, 2>()(0, iTS) = ch_input_TS;

            if(subdet == 2)
            {
                ch_input_2d.tensor<float, 2>()(HE_row, HE_col + iTS) = ch_input_TS;
            }
            //================ test total uncertainty ==================
            /*
            //ADC granularity
            auto noiseADC = (1./sqrt(12))*channel_info.tsDFcPerADC(iTS);
            //Photostatistics
            auto noisePhoto = 0.;
            if ((charge-ped)>channel_info.tsPedestalWidth(iTS))
            {noisePhoto = sqrt((charge-ped)*channel_info.fcByPE());}
            //Electronic pedestal
            auto pedWidth = channel_info.tsPedestalWidth(iTS);
            std::cout << noiseADC << ", " << noisePhoto << ", " << pedWidth << ", ";
            std::cout << channel_info.fcByPE() << ", " << pedWidth / ped << ", ";
            */
            //================ end test total uncertainty ==================
        }

        ty_input.tensor<float, 2>()(0, 0) = depth;
        ty_input.tensor<float, 2>()(0, 1) = ieta;

        std::vector<tensorflow::Tensor> outputs;        //vector size 1; 2d tensor of [[pred]]
        if(subdet == 1)
        {
            if(depth == 1) {tensorflow::run(session_d1HB, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);}
            else {tensorflow::run(session_dg1HB, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);}
        }

        else if(subdet == 2)
        {
            if(depth == 1) {tensorflow::run(session_d1HE, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);}
            else {tensorflow::run(session_dg1HE, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);}
        }

        //std::cout << "outputs.size() " << outputs.size() << ", tensor.shape().dim_size(0) " << outputs[0].shape().dim_size(0) << ", tensor.shape().dim_size(1) " << outputs[0].shape().dim_size(1) << std::endl;
        auto temp = outputs[0].tensor<float, 2>()(0,0);
        if(DLPHIN_apply_respCorr_) {temp = temp * resp_corr;}
        if(DLPHIN_truncate_)
        {
            temp = temp - 0.25;
            if(temp < 0) {temp = 0.0;}
        }
        //temp = temp * DLPHIN_rand->GetRandom();
        Doutput.push_back(temp);
    }

    std::vector<tensorflow::Tensor> outputs_2d;        //vector size 1; 2d tensor of [[pred]]
    tensorflow::run(session_2dHE, {{"net_charges",ch_input_2d},{"types_input",ty_input_2d},{"mask_input",ma_input_2d}}, {"reshape_1/Reshape"}, &outputs_2d);
    //std::cout << "outputs_2d.size() " << outputs_2d.size() << ", tensor.shape().dim_size(0) " << outputs_2d[0].shape().dim_size(0) << ", tensor.shape().dim_size(1) " << outputs_2d[0].shape().dim_size(1) << ", tensor.shape().dim_size(2) " << outputs_2d[0].shape().dim_size(2) << std::endl;
    /*
    for(int i = 0; i < HE_tot_rows; i++)
    {
        for(int j = 0; j < 56; j++)
        {
            std::cout << ch_input_2d.tensor<float, 2>()(i,j) << ", ";
        }
        for(int j = 0; j < HE_depth_max; j++)
        {
            auto pred = outputs_2d[0].tensor<float, 3>()(i,0,j);
            auto mask = outputs_2d[0].tensor<float, 3>()(i,1,j);
            std::cout << ma_input_2d.tensor<float, 2>()(i,j) << ", " << mask << ", " << mask * pred << ", ";
        }
        std::cout << ty_input_2d.tensor<float, 2>()(i,0) << std::endl;
    }
    */

    // second loop over all channels for
    // 1. save DLPHIN 2D CNN
    // 2. print out results
    for(int i = 0; i < (int)Dinput_vec.size(); i++)
    {
        auto rec_hit = Dinput_vec.at(i).rec_hit;
        auto eraw = rec_hit.eraw();     //m0
        auto eaux = rec_hit.eaux();     //m3 by default
        auto energy = rec_hit.energy(); //mahi
        auto flags = rec_hit.flags();

        auto hid = rec_hit.id();
        auto rawId = hid.rawId();
        auto subdet = hid.subdet();
        auto depth = hid.depth();
        auto ieta = hid.ieta();
        auto iphi = hid.iphi();

        auto channel_info = Dinput_vec.at(i).channel_info;
        int nSamples = channel_info.nSamples();
        auto gain = channel_info.tsGain(0);

        auto resp_corr = Dinput_vec.at(i).resp_corr;
        float rawgain = gain / resp_corr;

        float DLPHIN_SF = 1.0;                          //SF dirived from MAHI / DLPHIN

        std::vector<float> channel_vec_temp(23, 0.0);

        for (int iTS = 0; iTS < nSamples; ++iTS)
        {
            auto charge = channel_info.tsRawCharge(iTS);
            auto ped = channel_info.tsPedestal(iTS);

            if(DLPHIN_print_1d_)std::cout << charge << ", " << ped << ", ";
            if(subdet == 2)
            {
                channel_vec_temp.at(2*iTS) = charge;
                channel_vec_temp.at(2*iTS + 1) = ped;
            }
        }

        if(subdet == 2)
        {
            auto HE_row = ieta_iphi_row_map.at(std::make_pair(ieta, iphi));
            auto pred = outputs_2d[0].tensor<float, 3>()(HE_row,0,depth-1);
            auto mask = outputs_2d[0].tensor<float, 3>()(HE_row,1,depth-1);

            if(mask != 1) {std::cout << "Error! A real channel is masked" << std::endl;}
            if(DLPHIN_apply_respCorr_) {pred = pred * resp_corr;}
            if(DLPHIN_truncate_)
            {
                pred = pred - 0.25;
                if(pred < 0) {pred = 0.0;}
            }
            //pred = pred * DLPHIN_rand->GetRandom();
            Doutput.at(i) = pred;

            channel_vec_temp.at(16) = rawgain;
            channel_vec_temp.at(17) = gain;
            channel_vec_temp.at(18) = eraw;
            channel_vec_temp.at(19) = eaux;
            channel_vec_temp.at(20) = energy;
            channel_vec_temp.at(21) = 1;
            channel_vec_temp.at(22) = pred;
            ieta_iphi_energy_map.at(std::make_pair(ieta, iphi)).at(depth - 1) = channel_vec_temp;
        }

        if(DLPHIN_print_1d_) std::cout << rawgain << ", " << gain << ", " << eraw << ", " << eaux << ", " << energy << ", " << flags << ", " << rawId << ", " << subdet << ", " << depth << ", " << ieta << ", " << iphi << ", " << Doutput.at(i) << ", " << DLPHIN_SF << std::endl;
    }

    if(DLPHIN_print_2d_)
    {
        std::string title_string = "reco: ";
        for (int depth = 1; depth <= HE_depth_max; depth++)
        {
            std::string depth_string = "d" + std::to_string(depth);
            for (int TS = 1; TS <= 8; TS++)
            {
                std::string TS_string = depth_string + " TS" + std::to_string(TS);
                title_string = title_string + TS_string + " raw charge, " + TS_string + " ped noise, "; 
            }
            title_string = title_string + depth_string + " raw gain, " + depth_string + " gain, " + depth_string + " raw energy, " + depth_string + " aux energy, " + depth_string + " mahi energy, " + depth_string + " is real channel, " + depth_string + " DLPHIN energy, ";
        }

        title_string = title_string + "ieta, iphi";
        std::cout << title_string << std::endl;

        for(auto iter : ieta_iphi_energy_map)
        {
            auto key = iter.first;
            auto value = iter.second;
            for(int i = 0; i < (int)value.size(); i++)
            {
                auto channel_vec_temp = value.at(i);
                for(int j = 0; j < (int)channel_vec_temp.size(); j++)
                {std::cout << channel_vec_temp.at(j) << ", ";}
            }
            std::cout << key.first << ", " << key.second << std::endl;
        }
    }
}

void HBHEPhase1Reconstructor::save_dlphin(std::vector<float> Doutput, HBHERecHitCollection* rechits)
{
    for(int i = 0; i < (int)Doutput.size(); i++)
    {
        (*rechits)[i].setEnergy(Doutput.at(i));
    }
}

//define this as a plug-in
DEFINE_FWK_MODULE(HBHEPhase1Reconstructor);
