#include <ERepSimImpulse.hxx>

ERepSim::Impulse::Impulse(int s, std::shared_ptr<ERepSim::Carrier> c)
    : fSensorId(s), fTime(0), fWidth(0), fCharge(0) {
    fCarriers.push_back(c);
}

ERepSim::Impulse::Impulse(int s)
    : fSensorId(s), fTime(0), fWidth(0), fCharge(0) {}

ERepSim::Impulse::~Impulse() {}
