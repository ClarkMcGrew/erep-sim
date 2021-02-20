#include "ERepSimResponseBase.hxx"

ERepSim::ResponseBase::ResponseBase(const char *modelname)
    : fModelName(modelname) {}

ERepSim::ResponseBase::~ResponseBase() { }

void ERepSim::ResponseBase::Reset() {
    if (fCarriers) fCarriers->clear();
    fCarriers.reset(new ERepSim::Carrier::Map);
}

int ERepSim::ResponseBase::CountCarriers() const {
    int counter = 0;
    for (ERepSim::Carrier::Map::iterator c = fCarriers->begin();
         c != fCarriers->end(); ++c) {
        for (ERepSim::Carrier::Container::iterator p = c->second.begin();
             p != c->second.end(); ++p) {
            ++counter;
        }
    }
    return counter;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
