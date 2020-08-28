#include "ERepSimSensorBase.hxx"

ERepSim::SensorBase::SensorBase(const char *modelname)
    : fModelName(modelname) {
    fImpulses.reset(new ERepSim::Impulse::Map);
}

ERepSim::SensorBase::~SensorBase() { }

void ERepSim::SensorBase::Reset() {
    fImpulses.reset(new ERepSim::Impulse::Map);
}
