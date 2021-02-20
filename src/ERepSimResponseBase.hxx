#ifndef ERepSimResponseBase_hxx_seen
#define ERepSimResponseBase_hxx_seen

#include "ERepSimCarrier.hxx"

#include <TG4HitSegment.h>

namespace ERepSim {
    class ResponseBase;
}

/// An abstract base class for classes that convert energy deposition into
/// basic detector response (like scintillation photons, or free ionization
/// electrons).
class ERepSim::ResponseBase {
public:

    ResponseBase(const char *modelName);
    virtual ~ResponseBase();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize() = 0;

    /// Process a list of segments.
    virtual void Process(const TG4Event* event,
                         const TG4HitSegmentContainer& segments) = 0;

    /// Reset the simulation between events.  This will probably need to be
    /// overloaded by the derived classes, but the base class Reset should
    /// always be called (i.e. "ERepSim::ResponseBase::Reset()") by the
    /// derived class.
    virtual void Reset();

    std::shared_ptr<ERepSim::Carrier::Map> GetCarriers() const {
        return fCarriers;
    }

    int CountCarriers() const;

    const std::string& GetModelName() const {return fModelName;}

protected:

    /// What to say.  It's the name of the response model.
    std::string fModelName;

    /// The map of carriers.  The derived implementation classes should access
    /// this directly.
    std::shared_ptr<ERepSim::Carrier::Map> fCarriers;

};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
