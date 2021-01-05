#ifndef ERepSimDAQBase_hxx_seen
#define ERepSimDAQBase_hxx_seen

#include "ERepSimDigiHit.hxx"

namespace ERepSim {
    class DAQBase;
}

/// An abstract base class for classes that takes signals from the sensors and
/// then digitizes/re-calibrates the result.
class ERepSim::DAQBase {
public:

    DAQBase(const char *modelName);
    virtual ~DAQBase();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize() = 0;

    /// Process a list of segments.
    virtual void Process(const ERepSim::Impulse::Map& impulses) = 0;

    /// Reset the simulation between events.  This will probably need to be
    /// overloaded by the derived classes, but the base class Reset should
    /// always be called (i.e. "ERepSim::DAQBase::Reset()") by the
    /// derived class.
    virtual void Reset();

    std::shared_ptr<ERepSim::DigiHit::Map> GetDigiHits() const {
        return fDigiHits;
    }

    const std::string& GetModelName() const {return fModelName;}

protected:

    /// What to say.  It's the name of the response model.
    std::string fModelName;

    /// The map of impulses.  The derived implementation classes should access
    /// this directly.
    std::shared_ptr<ERepSim::DigiHit::Map> fDigiHits;
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
