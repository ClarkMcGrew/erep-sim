#include "ERepSimDetectorTPC.hxx"
#include "ERepSimResponseTPC.hxx"
#include "ERepSimSensorIdeal.hxx"
#include "ERepSimDAQMultiHit.hxx"

#include "ERepSimOutput.hxx"

#include <TG4Event.h>

#include <TGeoManager.h>

#include <iostream>

ERepSim::DetectorTPC::DetectorTPC(TPC_id tpc_id)
    : DetectorBase("TPC"), fTpcId(tpc_id) {
    fHitContainer = ERepSim::TPCdef::hitContainers[fTpcId];
    std::cout << "DetectorTPC::Construct" << std::endl;
}

ERepSim::DetectorTPC::~DetectorTPC() {
    std::cout << "DetectorTPC::Deconstruct" << std::endl;
}

void ERepSim::DetectorTPC::Initialize() {
    std::cout << "DetectorTPC::Initialize" << std::endl;

    ERepSim::Output::Get().Property["TPC.SensorMask"] = (int) 0x78000000;
    ERepSim::Output::Get().Property["TPC.SensorType"] = ((25 + fTpcId) << 27);
    ERepSim::Output::Get().PropertyString["TPC.TG4HitContainer"] = "volCube";

    fResponse.reset(new ERepSim::ResponseTPC(fTpcId));
    fResponse->Initialize();

    fSensor.reset(new ERepSim::SensorIdeal);
    fSensor->Initialize();


    ERepSim::Output::Get().Property["TPC.DAQ.TimeZero"] = -100.0;
    // ERepSim::Output::Get().Property["TPC.DAQ.IntegrationWindow"] = 200.0; //ns
    ERepSim::Output::Get().Property["TPC.DAQ.IntegrationWindow"] = 50.0; //ns
    ERepSim::Output::Get().Property["TPC.DAQ.DigitPerNanosecond"] = 2.0;
    ERepSim::Output::Get().Property["TPC.DAQ.DigitPerCharge"] = 5.0;

    std::shared_ptr<ERepSim::DAQMultiHit> multi(new ERepSim::DAQMultiHit);
    multi->SetTimeZero(
        ERepSim::Output::Get().Property["TPC.DAQ.TimeZero"]);
    multi->UseAverageTime();
    multi->SetIntegrationWindow(
        ERepSim::Output::Get().Property["TPC.DAQ.IntegrationWindow"]);
    multi->SetTimeCalibration(
        ERepSim::Output::Get().Property["TPC.DAQ.DigitPerNanosecond"]);
    multi->SetChargeCalibration(
        ERepSim::Output::Get().Property["TPC.DAQ.DigitPerCharge"]);
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

void ERepSim::DetectorTPC::Process(TG4Event* event) {
    fCurrentEvent = event;
    TG4HitSegmentDetectors& segments = event->SegmentDetectors;
    std::cout << "DetectorTPC::Process " << segments[fHitContainer].size()
              << " segments"
              << std::endl;
    fResponse->Process(segments[fHitContainer]);
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
