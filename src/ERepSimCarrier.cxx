#include <ERepSimCarrier.hxx>
#include <ERepSimOutput.hxx>

#include <TG4Event.h>
#include <TG4Trajectory.h>

#include <exception>
#include <iostream>

ERepSim::Carrier::Carrier(const TG4Event* event,
                          const TG4HitSegment* hitSegment,
                          int sensor, int segment)
    : fSensorId(sensor), fSegmentId(segment), fCharge(1.0) {
    if (!hitSegment && fSegmentId != 0) {
        std::cout << "Segment Id: " << fSegmentId << std::endl;
        throw std::runtime_error(
            "A NULL TG4HitSegment with a nonzero segment id");
    }
    if (hitSegment && fSegmentId == 0) {
        std::cout << "Segment Id: " << fSegmentId << std::endl;
        throw std::runtime_error(
            "A non-NULL TG4HitSegment with a zero segment id");
    }
    if (hitSegment) {
        fHitSegment = *hitSegment;
        if (fHitSegment.PrimaryId >=0) {
            fHitSegment.PrimaryId += ERepSim::Output::Get().TrajectoryIdOffset;
        }
        for (std::size_t i = 0; i < fHitSegment.Contrib.size(); ++i) {
            if (fHitSegment.Contrib[i] >= 0) {
                fHitSegment.Contrib[i]
                    += ERepSim::Output::Get().TrajectoryIdOffset;
            }
        }
    }
}

ERepSim::Carrier::~Carrier() {}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
