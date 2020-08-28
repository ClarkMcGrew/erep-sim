#ifndef ERepSimSensorIdeal_hxx_seen
#define ERepSimSensorIdeal_hxx_seen

#include "ERepSimSensorBase.hxx"

namespace ERepSim {
    class SensorIdeal;
}

/// An ideal sensor that directly translates incoming Carriers into out-going
/// impulses.  If this is used as a "interim" sensor, be sure to include the
/// PDE into the conversion factor for the energy to the number of carriers.
class ERepSim::SensorIdeal: public ERepSim::SensorBase {
public:

    SensorIdeal();
    virtual ~SensorIdeal();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize();

    /// Process a list of segments.
    virtual void Process(const ERepSim::Carrier::Map& carriers);

    /// Reset the simulation between events.  This will probably need to be
    /// overloaded by the derived classes, but the base class Reset should
    /// always be called (i.e. "ERepSim::SensorIdeal::Reset()") by the
    /// derived class.
    virtual void Reset();

private:
    // Add impulses to the fImpulses container.
    void AddImpulses(int id, const ERepSim::Carrier::Container& carriers);

};
#endif
