#include "ERepSimDAQBase.hxx"
#include "ERepSimDigiHit.hxx"

ERepSim::DAQBase::DAQBase(const char *modelname)
    : fModelName(modelname) {
    fDigiHits.reset(new ERepSim::DigiHit::Map);
}

ERepSim::DAQBase::~DAQBase() { }

void ERepSim::DAQBase::Reset() {
    fDigiHits.reset(new ERepSim::DigiHit::Map);
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
