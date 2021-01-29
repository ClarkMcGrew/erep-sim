#include "ERepSimDetectorTPC.hxx"
#include "ERepSimResponseTPC.hxx"
#include "ERepSimSensorIdeal.hxx"
#include "ERepSimDAQMultiHit.hxx"
#include "ERepSimUnits.hxx"

#include "ERepSimOutput.hxx"

#include <TG4Event.h>

#include <TGeoManager.h>

#include <iostream>

ERepSim::DetectorTPC::DetectorTPC(const char* modelName,
                                  const char* hitContainer,
                                  const char* volumeName,
                                  int tpc_id)
    : DetectorBase(modelName),
      fHitContainer(hitContainer),
      fVolumeName(volumeName),
      fTpcId(tpc_id) {
    std::cout << "DetectorTPC::Construct"
              << " " << fModelName
              << " with segments " << fHitContainer << std::endl;
}

ERepSim::DetectorTPC::~DetectorTPC() {
    std::cout << "DetectorTPC::Deconstruct" << std::endl;
}

void ERepSim::DetectorTPC::Initialize() {
    std::cout << "DetectorTPC::Initialize" << std::endl;

    ERepSim::Output::Get().Property[fModelName+".SensorMask"]
        = (int) 0x78000000;
    ERepSim::Output::Get().Property[fModelName+".SensorType"]
        = (fTpcId << 27);
    ERepSim::Output::Get().PropertyString[fModelName+".TG4HitContainer"]
        = fHitContainer;
    ERepSim::Output::Get().PropertyString[fModelName+".VolumeName"]
        = fVolumeName;

    fResponse.reset(new ERepSim::ResponseTPC(fModelName.c_str(),
                                             fVolumeName.c_str(),
                                             fTpcId));
    fResponse->Initialize();

    fSensor.reset(new ERepSim::SensorIdeal);
    fSensor->Initialize();

    ERepSim::Output::Get().Property[fModelName+".DAQ.TimeZero"] = -100.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.IntegrationWindow"]
        = 100.0*unit::ns;
    ERepSim::Output::Get().Property[fModelName+".DAQ.Threshold"]
        = 500.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerNanosecond"] = 2.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerCharge"] = 5.0;

    std::shared_ptr<ERepSim::DAQMultiHit> multi(new ERepSim::DAQMultiHit);
    multi->SetTimeZero(
        ERepSim::Output::Get().Property[fModelName+".DAQ.TimeZero"]);
    multi->UseAverageTime();
    multi->SetThreshold(
        ERepSim::Output::Get().Property[fModelName+".DAQ.Threshold"]);
    multi->UseSlidingWindow();
    multi->SetIntegrationWindow(
        ERepSim::Output::Get().Property[fModelName+".DAQ.IntegrationWindow"]);
    multi->SetDeadTime(0.0);
    multi->SetTimeCalibration(
        ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerNanosecond"]);
    multi->SetChargeCalibration(
        ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerCharge"]);
    fDAQ = multi;
    fDAQ->Initialize();

}

void ERepSim::DetectorTPC::Reset() {
    std::cout << "DetectorTPC::Reset" << std::endl;
    DetectorBase::Reset();
    fResponse->Reset();
    fSensor->Reset();
    fDAQ->Reset();
}

void ERepSim::DetectorTPC::Accumulate(int entry, const TG4Event* event) {
    const TG4HitSegmentDetectors& segments = event->SegmentDetectors;
    TG4HitSegmentDetectors::const_iterator detector
        = segments.find(fHitContainer);
    if (detector == segments.end()) return;
    std::cout << "DetectorTPC::Accumulate " << detector->second.size()
              << " segments"
              << std::endl;
    fResponse->Process(event,detector->second);
    std::cout << "DetectorTPC::Accumulate done"
              << std::endl;
}

void ERepSim::DetectorTPC::Process(int entry) {
    std::cout << "DetectorTPC::Process"
              << std::endl;
    if (fResponse->GetCarriers()) {
        fSensor->Process(*(fResponse->GetCarriers()));
    }
    if (fSensor->GetImpulses()) {
        fDAQ->Process(*(fSensor->GetImpulses()));
    }
    int generatedHits = 0;
    if (fDAQ->GetDigiHits()) {
        for (ERepSim::DigiHit::Map::iterator s
                 = fDAQ->GetDigiHits()->begin();
             s != fDAQ->GetDigiHits()->end(); ++s) {
            for (ERepSim::DigiHit::Container::iterator h
                     = s->second.begin();
                 h != s->second.end(); ++h) {
                PackDigiHit(*(*h));
                ++generatedHits;
            }
        }
    }
    std::cout << "DetectorTPC::Process " << generatedHits
              << " hits generated" << std::endl;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
