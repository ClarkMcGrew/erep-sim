#include "ERepSimSegmentIdManager.hxx"

int ERepSim::SegmentIdManager::fSegmentIdentifier = 0;

ERepSim::SegmentIdManager::SegmentIdManager() {}
ERepSim::SegmentIdManager::~SegmentIdManager() {}

int ERepSim::SegmentIdManager::GetNextSegmentIdentifier() {
    if (fSegmentIdentifier < 0) fSegmentIdentifier = 0;
    ++fSegmentIdentifier;
    if (fSegmentIdentifier > 1000000) fSegmentIdentifier = 1;
    return fSegmentIdentifier;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
