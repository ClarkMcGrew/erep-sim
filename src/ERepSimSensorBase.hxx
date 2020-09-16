#ifndef ERepSimSensorBase_hxx_seen
#define ERepSimSensorBase_hxx_seen

#include "ERepSimCarrier.hxx"
#include "ERepSimImpulse.hxx"

namespace ERepSim {
    class SensorBase;
}

/// An abstract base class for classes that convert basic detector response
/// (like scintillation photons, or free ionization electrons) into
/// (electrical) impulses produced by a sensor.
class ERepSim::SensorBase {
public:

    SensorBase(const char *modelName);
    virtual ~SensorBase();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize() = 0;

    /// Process a container of carriers.
    virtual void Process(const ERepSim::Carrier::Map& carriers) = 0;

    /// Reset the simulation between events.  This will probably need to be
    /// overloaded by the derived classes, but the base class Reset should
    /// always be called (i.e. "ERepSim::SensorBase::Reset()") by the
    /// derived class.
    virtual void Reset();

    std::shared_ptr<ERepSim::Impulse::Map> GetImpulses() const {
        return fImpulses;
    }

    const std::string& GetModelName() const {return fModelName;}

    int CountImpulses() const;

protected:

    /// What to say.  It's the name of the response model.
    std::string fModelName;

    /// The map of impulses.  The derived implementation classes should access
    /// this directly.
    std::shared_ptr<ERepSim::Impulse::Map> fImpulses;
};
#endif
