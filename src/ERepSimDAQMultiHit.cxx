#include "ERepSimDAQMultiHit.hxx"
#include "ERepSimOutput.hxx"

#include <iostream>
#include <cmath>

ERepSim::DAQMultiHit::DAQMultiHit()
    : ERepSim::DAQBase("MultiHit"),
      fTimeZero(0), fIntegrationWindow(1E+10),
      fDigitsPerNanosecond(10.0), fDigitsPerCharge(10.0) {}

ERepSim::DAQMultiHit::~DAQMultiHit() {}

void ERepSim::DAQMultiHit::Initialize() {
    std::cout << "DAQMultiHit::Initialize" << std::endl;
}

void ERepSim::DAQMultiHit::Reset() {
    ERepSim::DAQBase::Reset();
}

void ERepSim::DAQMultiHit::Process(const ERepSim::Impulse::Map& impulses) {
    std::cout << "DAQMultiHit::Process" << std::endl;
    for (ERepSim::Impulse::Map::const_iterator imp = impulses.begin();
         imp != impulses.end(); ++imp) {
        DigitizeImpulses(imp->first, imp->second);
    }
}

void ERepSim::DAQMultiHit::DigitizeImpulses(
    int id, const ERepSim::Impulse::Container& impulses) {
    if (impulses.empty()) return;
    std::shared_ptr<ERepSim::DigiHit> digiHit(new ERepSim::DigiHit(id));
    digiHit->SetPosition(impulses.front()->GetPosition());
    double hitT = impulses.front()->GetTime();
    double lastT = impulses.front()->GetTime();
    double hitQ = 0.0;
    for (ERepSim::Impulse::Container::const_iterator i = impulses.begin();
         i != impulses.end(); ++i) {
        if (id != (*i)->GetSensorId()) {
            throw std::runtime_error("DAQMultiHit::DigitizeImpulses: bad id");
        }
        if ((*i)->GetTime() > lastT + 20.0) {
            // Digitize, calibrate and move to the next hit.
            DigitizeHit(digiHit,hitT,hitQ);
            (*fDigiHits)[id].push_back(digiHit);
            // Setup for the next hit.
            digiHit.reset(new ERepSim::DigiHit(id));
            digiHit->SetPosition((*i)->GetPosition());
            hitT = (*i)->GetTime();
            hitQ = 0;
        }
        digiHit->AddImpulse(*i);
        lastT = (*i)->GetTime();
        hitQ += (*i)->GetCharge();
    }
}

void ERepSim::DAQMultiHit::DigitizeHit(std::shared_ptr<ERepSim::DigiHit> hit,
                                       double t, double q) {

    int iTime = std::round((t-fTimeZero) * fDigitsPerNanosecond);
    int iCharge = std::round(q * fDigitsPerCharge);
    double cTime = iTime / fDigitsPerNanosecond;
    double cCharge = iCharge / fDigitsPerCharge;
    hit->GetDigiTimes().push_back(iTime);
    hit->GetDigiCharges().push_back(iCharge);
    hit->GetTimes().push_back(cTime);
    hit->GetTimeWidths().push_back(1.0/fDigitsPerNanosecond);
    hit->GetCharges().push_back(cCharge);
}
