#include "ERepSimSensorIdeal.hxx"

#include <iostream>

ERepSim::SensorIdeal::SensorIdeal()
    : ERepSim::SensorBase("Ideal") {}

ERepSim::SensorIdeal::~SensorIdeal() {}

void ERepSim::SensorIdeal::Initialize() {
    std::cout << "SensorIdeal::Initialize" << std::endl;
}

void ERepSim::SensorIdeal::Reset() {
    ERepSim::SensorBase::Reset();
}

namespace {
    struct CompareImpulses {
        bool operator() (std::shared_ptr<ERepSim::Impulse> a,
                         std::shared_ptr<ERepSim::Impulse> b) {
            if (a->GetTime() == b->GetTime()) {
                return a->GetCharge() > b->GetCharge();
            }
            return (a->GetTime() < b->GetTime());
        }
    };
}

void ERepSim::SensorIdeal::Process(const ERepSim::Carrier::Map& carriers) {
#ifdef LOUD_AND_PROUD
    std::cout << "SensorIdeal::Process"
              << " " << carriers.size() << " sensors"
              << std::endl;
#endif
    for (ERepSim::Carrier::Map::const_iterator car = carriers.begin();
         car != carriers.end(); ++car) {
        AddImpulses(car->first, car->second);
    }
    // Make sure the impulses are ordered by time, then decreasing charge.
    for (ERepSim::Impulse::Map::iterator imp = fImpulses->begin();
         imp != fImpulses->end(); ++imp) {
        std::sort(imp->second.begin(), imp->second.end(), CompareImpulses());
    }

#ifdef LOUD_AND_PROUD
    std::cout << "SensorIdeal::Process " << CountImpulses()
              << " impulses generated" << std::endl;
#endif
}

void ERepSim::SensorIdeal::AddImpulses(
    int id, const ERepSim::Carrier::Container& carriers) {
    for (ERepSim::Carrier::Container::const_iterator c = carriers.begin();
         c != carriers.end(); ++c) {
        if (id != (*c)->GetSensorId()) {
            throw std::runtime_error("SensorIdeal::AddImpulses -- id mismatch");
        }
        std::shared_ptr<ERepSim::Impulse> impulse(new ERepSim::Impulse(id));
        impulse->AddCarrier(*c);
        impulse->SetPosition((*c)->GetFinalPosition().Vect());
        impulse->SetTime((*c)->GetFinalPosition().T());
        impulse->SetCharge((*c)->GetCharge());
        (*fImpulses)[id].push_back(impulse);
    }
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
