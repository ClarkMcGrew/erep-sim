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
