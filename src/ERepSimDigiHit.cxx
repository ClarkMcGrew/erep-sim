#include "ERepSimDigiHit.hxx"

ERepSim::DigiHit::DigiHit(int sensor)
    : fSensorId(sensor) {
}

ERepSim::DigiHit::DigiHit(int sensor,
                          std::shared_ptr<ERepSim::Impulse> impulse)
    : fSensorId(sensor) {
    AddImpulse(impulse);
}

ERepSim::DigiHit::~DigiHit() {}
