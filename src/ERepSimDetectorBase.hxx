#ifndef ERepSimDetectorBase_hxx_seen
#define ERepSimDetectorBase_hxx_seen

#include "ERepSimDigiHit.hxx"

namespace ERepSim {
    class DetectorBase;
}
class TG4Event;
class TG4HitSegment;

/// An abstract base class for classes that simulate a full detector response,
/// digitization and calibration chain.  This converts energy deposition into
/// basic detector response (like scintillation photons, or free ionization
/// electrons), digitizes the hits, and then produces calibrated hits.  This
/// directly fills the ERepSim::Output tree.
class ERepSim::DetectorBase {
public:

    /// The derived constructor should not do anything except set default
    /// values for fields.  The constructor should assume that the model will
    /// not be used.  If the model is used, the Initialize method will be
    /// called.
    DetectorBase(const char *modelName);
    virtual ~DetectorBase();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize() = 0;

    /// Accumulate a list of segments.  This can be called multiple times per
    /// ERepSim "event".
    virtual void Accumulate(int entry, const TG4Event* event) = 0;

    /// Process all of the accumulated information and save the output.
    virtual void Process(int entry) = 0;

    /// Reset the simulation between erep-sim events.  This will probably need
    /// to be overloaded by the derived classes, but the base class Reset
    /// should always be called (i.e. "ERepSim::DetectorBase::Reset()") by the
    /// derived class.
    virtual void Reset();

    const std::string& GetModelName() const {return fModelName;}

protected:

    /// What to say.  It's the name of the response model.
    std::string fModelName;

    /// Pack the output!
    void PackDigiHit(const ERepSim::DigiHit& hit);
    void PackDirectSegments(const ERepSim::DigiHit& hit);
    void PackImpulses(const ERepSim::Impulse::Container& impulses);
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
