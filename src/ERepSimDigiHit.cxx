#include "ERepSimDigiHit.hxx"

ERepSim::DigiHit::DigiHit(int sensor)
    : fSensorId(sensor) {
    fDirectSegments.clear();
}

ERepSim::DigiHit::DigiHit(int sensor,
                          std::shared_ptr<ERepSim::Impulse> impulse)
    : fSensorId(sensor) {
    fDirectSegments.clear();
    AddImpulse(impulse);
}

ERepSim::DigiHit::~DigiHit() {}

void ERepSim::DigiHit::AddDirectHitSegment(int segId,
                                           const TG4HitSegment& seg) {
    if (!fImpulses.empty()) {
        // Cannot have impulses and direct segments: This might be overly
        // strict, but is here at least for the initial debugging.
        throw std::runtime_error("Impulses in hit when adding a segment");
    }
    fDirectSegments[segId] = seg;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
