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
    int sipmQTSShift_;
    int sipmQNTStoSum_;

    // Parameters for turning status bit setters on/off
    bool setNegativeFlagsQIE8_;
    bool setNegativeFlagsQIE11_;
    bool setNoiseFlagsQIE8_;
    bool setNoiseFlagsQIE11_;
    bool setPulseShapeFlagsQIE8_;
    bool setPulseShapeFlagsQIE11_;

    // Other members
    edm::EDGetTokenT<HBHEDigiCollection> tok_qie8_;
    edm::EDGetTokenT<QIE11DigiCollection> tok_qie11_;
    std::unique_ptr<AbsHBHEPhase1Algo> reco_;
    std::unique_ptr<AbsHcalAlgoData> recoConfig_;
    std::unique_ptr<HcalRecoParams> paramTS_;

    // Struct for DLPHIN
    struct DLPHIN_input {HBHEChannelInfo channel_info; float rawgain; HBHERecHit rec_hit;};
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
                     HBHERecHitCollection* rechits);

    void run_dlphin(std::vector<DLPHIN_input> Dinput_vec, std::vector<float>& Doutput);

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
      sipmQTSShift_(conf.getParameter<int>("sipmQTSShift")),
      sipmQNTStoSum_(conf.getParameter<int>("sipmQNTStoSum")),
      setNegativeFlagsQIE8_(conf.getParameter<bool>("setNegativeFlagsQIE8")),
      setNegativeFlagsQIE11_(conf.getParameter<bool>("setNegativeFlagsQIE11")),
      setNoiseFlagsQIE8_(conf.getParameter<bool>("setNoiseFlagsQIE8")),
      setNoiseFlagsQIE11_(conf.getParameter<bool>("setNoiseFlagsQIE11")),
      setPulseShapeFlagsQIE8_(conf.getParameter<bool>("setPulseShapeFlagsQIE8")),
      setPulseShapeFlagsQIE11_(conf.getParameter<bool>("setPulseShapeFlagsQIE11")),
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
                                          HBHERecHitCollection* rechits)
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
        float rawgain = 0;

        // Go over time slices and fill the samples
        for (int ts = 0; ts < maxTS; ++ts)
        {
            auto s(frame[ts]);
            const uint8_t adc = s.adc();
            const int capid = s.capid();
            //optionally store "effective" pedestal (measured with bias voltage on)
            // = QIE contribution + SiPM contribution (from dark current + crosstalk)
            const double pedestal = saveEffectivePeds ? calib.effpedestal(capid) : calib.pedestal(capid);
            const double pedestalWidth = saveEffectivePeds ? calibWidth.effpedestal(capid) : calibWidth.pedestal(capid);
            const double gain = calib.respcorrgain(capid);
            if(ts == 0){rawgain = calib.rawgain(capid);}
            const double gainWidth = calibWidth.gain(capid);
            //always use QIE-only pedestal for this computation
            const double rawCharge = rcfs.getRawCharge(cs[ts], calib.pedestal(capid));
            const float t = getTDCTimeFromSample(s);
            const float dfc = getDifferentialChargeGain(*channelCoder, *shape, adc,
                                                        capid, channelInfo->hasTimeInfo());
            channelInfo->setSample(ts, adc, dfc, rawCharge,
                                   pedestal, pedestalWidth,
                                   gain, gainWidth, t);
            if (ts == soi)
                soiCapid = capid;
        }

        // Fill the overall channel info items
        const int pulseShapeID = param_ts->pulseShapeID();
        const std::pair<bool,bool> hwerr = findHWErrors(frame, maxTS);
        channelInfo->setChannelInfo(cell, pulseShapeID, maxTS, soi, soiCapid,
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
                DLPHIN_input_vec.push_back((DLPHIN_input){*channelInfo, rawgain, rh});
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

    //clear DLPHIN input/output vectors for each event
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
                                   isData, &channelInfo, infos.get(), out.get());
        if (setNoiseFlagsQIE8_)
            hbheFlagSetterQIE8_->SetFlagsFromRecHits(*out);
    }

    if (processQIE11_)
    {
        if (setNoiseFlagsQIE11_)
            hbheFlagSetterQIE11_->Clear();

        HBHEChannelInfo channelInfo(true,saveEffectivePedestal_);
        processData<QIE11DataFrame>(*heDigis, *conditions, *p, *mycomputer,
                                    isData, &channelInfo, infos.get(), out.get());
        if (setNoiseFlagsQIE11_)
            hbheFlagSetterQIE11_->SetFlagsFromRecHits(*out);
    }

    run_dlphin(DLPHIN_input_vec, DLPHIN_output);

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

    add_param_set(algorithm);
    add_param_set(flagParametersQIE8);
    add_param_set(flagParametersQIE11);
    add_param_set(pulseShapeParametersQIE8);
    add_param_set(pulseShapeParametersQIE11);
    
    descriptions.addDefault(desc);
}

void HBHEPhase1Reconstructor::run_dlphin(std::vector<DLPHIN_input> Dinput_vec, std::vector<float>& Doutput)
{
    tensorflow::GraphDef *graphDef_d1HB = tensorflow::loadGraphDef("DLPHIN_pb/model_d1HB_R2.pb");
    tensorflow::Session *session_d1HB = tensorflow::createSession(graphDef_d1HB);

    tensorflow::GraphDef *graphDef_dg1HB = tensorflow::loadGraphDef("DLPHIN_pb/model_dg1HB_R2.pb");
    tensorflow::Session *session_dg1HB = tensorflow::createSession(graphDef_dg1HB);

    tensorflow::GraphDef *graphDef_d1HE = tensorflow::loadGraphDef("DLPHIN_pb/model_d1HE_R2.pb");
    tensorflow::Session *session_d1HE = tensorflow::createSession(graphDef_d1HE);

    tensorflow::GraphDef *graphDef_dg1HE = tensorflow::loadGraphDef("DLPHIN_pb/model_dg1HE_R2.pb");
    tensorflow::Session *session_dg1HE = tensorflow::createSession(graphDef_dg1HE);

    std::cout << "reco: TS1 raw charge, TS1 ped noise, TS2 raw charge, TS2 ped noise, TS3 raw charge, TS3 ped noise, TS4 raw charge, TS4 ped noise, TS5 raw charge, TS5 ped noise, TS6 raw charge, TS6 ped noise, TS7 raw charge, TS7 ped noise, TS8 raw charge, TS8 ped noise, raw gain, gain, raw energy, aux energy, mahi energy, flags, id, sub detector, depth, ieta, iphi, DLPHIN energy" << std::endl;

    for(auto iter : Dinput_vec)
    {
        auto rec_hit = iter.rec_hit;
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

        auto channel_info = iter.channel_info;
        int nSamples = channel_info.nSamples();
        auto gain = channel_info.tsGain(0);

        auto rawgain = iter.rawgain;

        tensorflow::Tensor ch_input(tensorflow::DT_FLOAT, {1, 8}); // template for charge input
        auto ch_input_tensor = ch_input.tensor<float, 2>(); // place holder for taking in values

        tensorflow::Tensor ty_input(tensorflow::DT_FLOAT, {1, 2}); // template for charge input
        auto ty_input_tensor = ty_input.tensor<float, 2>(); // place holder for taking in values

        for (int iTS = 0; iTS < nSamples; ++iTS)
        {
            auto charge = channel_info.tsRawCharge(iTS);
            auto ped = channel_info.tsPedestal(iTS);
            std::cout << charge << ", " << ped << ", ";
            
            ch_input_tensor(0, iTS) = (charge - ped)*gain;
        }

        // ty_input_tensor(0,0) = hid.subdet();
        ty_input_tensor(0,0) = depth;
        ty_input_tensor(0,1) = ieta;

        std::vector<tensorflow::Tensor> outputs;
        if(subdet == 1)
        {
            if(depth == 1) tensorflow::run(session_d1HB, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);
            else tensorflow::run(session_dg1HB, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);
        }

        else if(subdet == 2)
        {
            if(depth == 1) tensorflow::run(session_d1HE, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);
            else tensorflow::run(session_dg1HE, {{"net_charges",ch_input},{"types_input",ty_input}}, {"dense/Relu"}, &outputs);
        }

        float temp = float(outputs[0].matrix<float>()(0));
        std::cout << rawgain << ", " << gain << ", " << eraw << ", " << eaux << ", " << energy << ", " << flags << ", " << rawId << ", " << subdet << ", " << depth << ", " << ieta << ", " << iphi << ", " << temp << std::endl;

        Doutput.push_back(temp);
    }
    tensorflow::closeSession(session_d1HB);
    delete graphDef_d1HB;
    tensorflow::closeSession(session_dg1HB);
    delete graphDef_dg1HB;
    tensorflow::closeSession(session_d1HE);
    delete graphDef_d1HE;
    tensorflow::closeSession(session_dg1HE);
    delete graphDef_dg1HE;
}

//define this as a plug-in
DEFINE_FWK_MODULE(HBHEPhase1Reconstructor);
