#ifndef ERepSimDAQMultiHit_hxx_seen
#define ERepSimDAQMulti_hxx_seen

#include "ERepSimDAQBase.hxx"

namespace ERepSim {
    class DAQMultiHit;
}

/// Simulate multi-hit TDC/ADC electronics, DAQ and Calibration.
class ERepSim::DAQMultiHit : public ERepSim::DAQBase {
public:

    DAQMultiHit();
    virtual ~DAQMultiHit();

    /// Initialize the model.  This is called before the first event is
    /// processed.
    virtual void Initialize();

    /// Process a list of segments.
    virtual void Process(const ERepSim::Impulse::Map& impulses);

    /// Reset the simulation between events.  This will probably need to be
    /// overloaded by the derived classes, but the base class Reset should
    /// always be called (i.e. "ERepSim::DAQBase::Reset()") by the
    /// derived class.
    virtual void Reset();

    /// Set the threshold for a hit.
    void SetThreshold(double t) {fThreshold = t;}

    /// Set the time zero.  This is the time the trigger occured.
    void SetTimeZero(double t) {fTimeZero = t;}

    /// Set the hit integration window.  This is the minimum time window for a
    /// hit.
    void SetIntegrationWindow(double t) {fIntegrationWindow = t;}

    /// Set the time calibration in TDC counts per ns
    void SetTimeCalibration(double t) {fDigitsPerNanosecond = t;}

    /// Set the charge calibration in ADC counts per unit charge.
    void SetChargeCalibration(double q) {fDigitsPerCharge = q;}

private:
    // Add impulses to the fDigiHits container.
    void DigitizeImpulses(int id, const ERepSim::Impulse::Container& impulses);

    void DigitizeHit(std::shared_ptr<ERepSim::DigiHit> hit, double t, double q);

    // The charge threshold for the hit.
    double fThreshold;

    // The time of the trigger.
    double fTimeZero;

    // the integration window for the ADC.
    double fIntegrationWindow;

    // The calibration constant for time.
    double fDigitsPerNanosecond;

    // The calibration constant for charge.
    double fDigitsPerCharge;
};
#endif
