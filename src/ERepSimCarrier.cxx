#include <ERepSimCarrier.hxx>

#include <exception>

ERepSim::Carrier::Carrier(int sensor, int segment,
                          const TG4HitSegment* hitSegment)
    : fSensorId(sensor),
      fSegmentId(segment), fHitSegment(hitSegment),
      fCharge(1.0) {
    if (!fHitSegment && fSegmentId != 0) {
        throw std::runtime_error(
            "A NULL TG4HitSegment with a nonzero segment id");
    }
    if (fHitSegment && fSegmentId == 0) {
        throw std::runtime_error(
            "A non-NULL TG4HitSegment with a zero segment id");
    }
}

ERepSim::Carrier::~Carrier() {}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
