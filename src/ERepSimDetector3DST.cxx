#include "ERepSimDetector3DST.hxx"
#include "ERepSimResponse3DST.hxx"
#include "ERepSimSensorIdeal.hxx"
#include "ERepSimDAQMultiHit.hxx"
#include "ERepSimUnits.hxx"

#include "ERepSimOutput.hxx"

#include <TG4Event.h>

#include <TGeoManager.h>

#include <iostream>

ERepSim::Detector3DST::Detector3DST()
    : DetectorBase("3DST") {
    std::cout << "Detector3DST::Construct" << std::endl;
}

ERepSim::Detector3DST::~Detector3DST() {
    std::cout << "Detector3DST::Deconstruct" << std::endl;
}

void ERepSim::Detector3DST::Initialize() {
    std::cout << "Detector3DST::Initialize" << std::endl;

    ERepSim::Output::Get().Property[fModelName+".SensorMask"]
        = (int) 0x78000000;
    ERepSim::Output::Get().Property[fModelName+".SensorType"] = (13 << 27);
    ERepSim::Output::Get().PropertyString[fModelName+".TG4HitContainer"]
        = "volCube";

    fResponse.reset(new ERepSim::Response3DST);
    fResponse->Initialize();

    fSensor.reset(new ERepSim::SensorIdeal);
    fSensor->Initialize();

    ERepSim::Output::Get().Property[fModelName+".DAQ.TimeZero"] = -100.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.IntegrationWindow"]
        = 50.0*unit::ns;
    ERepSim::Output::Get().Property[fModelName+".DAQ.Threshold"]
        = 3.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerNanosecond"] = 2.0;
    ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerCharge"] = 5.0;

    std::shared_ptr<ERepSim::DAQMultiHit> multi(new ERepSim::DAQMultiHit);
    multi->SetTimeZero(
        ERepSim::Output::Get().Property[fModelName+".DAQ.TimeZero"]);
    multi->UseThresholdTime();
    multi->SetThreshold(
        ERepSim::Output::Get().Property[fModelName+".DAQ.Threshold"]);
    multi->UseFixedWindow();
    multi->SetIntegrationWindow(
        ERepSim::Output::Get().Property[fModelName+".DAQ.IntegrationWindow"]);
    multi->SetDeadTime(50*unit::ns);
    multi->SetTimeCalibration(
        ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerNanosecond"]);
    multi->SetChargeCalibration(
        ERepSim::Output::Get().Property[fModelName+".DAQ.DigitPerCharge"]);
    fDAQ = multi;
    fDAQ->Initialize();

}

void ERepSim::Detector3DST::Reset() {
#ifdef LOUD_AND_PROUD
    std::cout << "Detector3DST::Reset" << std::endl;
#endif
    DetectorBase::Reset();
    fResponse->Reset();
    fSensor->Reset();
    fDAQ->Reset();
}

void ERepSim::Detector3DST::Accumulate(int entry, const TG4Event* event) {
    const TG4HitSegmentDetectors& segments = event->SegmentDetectors;
    TG4HitSegmentDetectors::const_iterator detector = segments.find("volCube");
    if (detector == segments.end()) return;
#ifdef LOUD_AND_PROUD
    std::cout << "Detector3DST::Accumulate " << detector->second.size()
              << " segments"
              << std::endl;
#endif
    fResponse->Process(event,detector->second);
#ifdef LOUD_AND_PROUD
    std::cout << "Detector3DST::Accumulate "
              << std::endl;
#endif
}

void ERepSim::Detector3DST::Process(int entry) {
#ifdef LOUD_AND_PROUD
    std::cout << "Detector3DST::Process "
              << std::endl;
#endif
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
    std::cout << "Detector3DST::Process " << generatedHits
              << " hits generated" << std::endl;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
