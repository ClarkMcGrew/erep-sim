#include <ERepSimCarrier.hxx>

ERepSim::Carrier::Carrier(int sensor, int segment,
                          const TG4HitSegment* hitSegment)
    : fSensorId(sensor),
      fSegmentId(segment), fHitSegment(hitSegment),
      fCharge(1.0) {}

ERepSim::Carrier::~Carrier() {}
