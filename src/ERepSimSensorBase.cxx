#include "ERepSimSensorBase.hxx"

ERepSim::SensorBase::SensorBase(const char *modelname)
    : fModelName(modelname) {
    fImpulses.reset(new ERepSim::Impulse::Map);
}

ERepSim::SensorBase::~SensorBase() { }

void ERepSim::SensorBase::Reset() {
    fImpulses.reset(new ERepSim::Impulse::Map);
}

int ERepSim::SensorBase::CountImpulses() const {
    int counter = 0;
    for (ERepSim::Impulse::Map::iterator c = fImpulses->begin();
         c != fImpulses->end(); ++c) {
        for (ERepSim::Impulse::Container::iterator p = c->second.begin();
             p != c->second.end(); ++p) {
            ++counter;
        }
    }
    return counter;
}
