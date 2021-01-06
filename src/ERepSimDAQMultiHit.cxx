#include "ERepSimDAQMultiHit.hxx"
#include "ERepSimOutput.hxx"

#include <iostream>
#include <cmath>

ERepSim::DAQMultiHit::DAQMultiHit()
    : ERepSim::DAQBase("MultiHit"),
      fThreshold(2.5), fTimeZero(0), fUseThresholdTime(true),
      fIntegrationWindow(50.0), fDeadTime(10.0),
      fDigitsPerNanosecond(10.0), fDigitsPerCharge(10.0) {}

ERepSim::DAQMultiHit::~DAQMultiHit() {}

void ERepSim::DAQMultiHit::Initialize() {
    std::cout << "DAQMultiHit::Initialize" << std::endl;
}

void ERepSim::DAQMultiHit::Reset() {
    ERepSim::DAQBase::Reset();
}

void ERepSim::DAQMultiHit::Process(const ERepSim::Impulse::Map& impulses) {
    std::cout << "DAQMultiHit::Process " << impulses.size() << " sensors hit"
              << std::endl;
    for (ERepSim::Impulse::Map::const_iterator imp = impulses.begin();
         imp != impulses.end(); ++imp) {
        DigitizeImpulses(imp->first, imp->second);
    }
    std::cout << "DAQMultiHit::Process"
              << " " << GetDigiHits()->size()
              << " hits generated"
              << std::endl;
}

void ERepSim::DAQMultiHit::DigitizeImpulses(
    int id, const ERepSim::Impulse::Container& impulses) {
    if (impulses.empty()) {
        std::cout << "No impulses on sensor " << id << std::endl;
        return;
    }
    // Set up the first (possibly only) hit
    std::shared_ptr<ERepSim::DigiHit> digiHit(new ERepSim::DigiHit(id));
    digiHit->SetPosition(impulses.front()->GetPosition());
    double hitT = impulses.front()->GetTime();
    double hitQ = 0.0;
    for (ERepSim::Impulse::Container::const_iterator i = impulses.begin();
         i != impulses.end(); ++i) {
        if (id != (*i)->GetSensorId()) {
            throw std::runtime_error("DAQMultiHit::DigitizeImpulses: bad id");
        }
        if ((*i)->GetTime() > hitT + fIntegrationWindow + fDeadTime) {
            // Digitize, calibrate and move to the next hit.
            if (hitQ > fThreshold) {
                DigitizeHit(digiHit);
                (*fDigiHits)[id].push_back(digiHit);
            }
            // Setup for the next hit.
            digiHit.reset(new ERepSim::DigiHit(id));
            digiHit->SetPosition((*i)->GetPosition());
            hitT = (*i)->GetTime();
            hitQ = 0;
        }
        if ((*i)->GetTime() > hitT + fIntegrationWindow) continue;
        digiHit->AddImpulse(*i);
        hitQ += (*i)->GetCharge();
    }
    if (hitQ < fThreshold) return;
    DigitizeHit(digiHit);
    (*fDigiHits)[id].push_back(digiHit);
}

void ERepSim::DAQMultiHit::DigitizeHit(std::shared_ptr<ERepSim::DigiHit> hit) {
    const ERepSim::Impulse::Container& impulses = hit->GetImpulses();
    double qHit = 0.0;
    double tHit = 1E+20;
    if (fUseThresholdTime) {
        // Find the threshold crossing...
        double q = 0.0;
        for (std::size_t i = 0; i< impulses.size(); ++i) {
            q += impulses[i]->GetCharge();
            if (q > fThreshold) {
                tHit = impulses[i]->GetTime();
                break;
            }
        }
        // Get the total charge.
        for (std::size_t i = 0; i< impulses.size(); ++i) {
            qHit += impulses[i]->GetCharge();
        }
    }
    else {
        qHit = 0.0;
        tHit = 0.0;
        for (std::size_t i = 0; i< impulses.size(); ++i) {
            qHit += impulses[i]->GetCharge();
            tHit += impulses[i]->GetCharge()*impulses[i]->GetTime();
        }
        if (qHit > fThreshold) {
            tHit /= qHit;
        }
        else {
            tHit = 1E+20;
        }
    }

    int iTime = std::round((tHit-fTimeZero) * fDigitsPerNanosecond);
    int iCharge = std::round(qHit * fDigitsPerCharge);
    double cTime = iTime / fDigitsPerNanosecond;
    double cCharge = iCharge / fDigitsPerCharge;

    hit->GetDigiTimes().push_back(iTime);
    hit->GetDigiCharges().push_back(iCharge);
    hit->GetTimes().push_back(cTime);
    hit->GetTimeWidths().push_back(1.0/fDigitsPerNanosecond);
    hit->GetCharges().push_back(cCharge);
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
