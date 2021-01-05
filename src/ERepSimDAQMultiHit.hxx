#ifndef ERepSimDAQMultiHit_hxx_seen
#define ERepSimDAQMultiHit_hxx_seen

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

    /// The hit time is the time that the threshold was crossed.  This is
    /// appropriate for photon counting.
    void UseThresholdTime() {fUseThresholdTime = true;}

    /// The hit time is the average time of the integrated charge.  This is
    /// appropriate for a TPC.
    void UseAverageTime() {fUseThresholdTime = false;}

    /// Set the hit integration window.  This is the minimum time window for a
    /// hit.
    void SetIntegrationWindow(double t) {fIntegrationWindow = t;}

    /// Set the dead time between hits on a channel.  This is the minimum time
    /// window for a hit.
    void SetDeadTime(double t) {fDeadTime = t;}

    /// Set the time calibration in TDC counts per ns
    void SetTimeCalibration(double t) {fDigitsPerNanosecond = t;}

    /// Set the charge calibration in ADC counts per unit charge.
    void SetChargeCalibration(double q) {fDigitsPerCharge = q;}

private:
    // Add impulses to the fDigiHits container.
    void DigitizeImpulses(int id, const ERepSim::Impulse::Container& impulses);

    void DigitizeHit(std::shared_ptr<ERepSim::DigiHit> hit);

    // The charge threshold for the hit.
    double fThreshold;

    // The time of the trigger.
    double fTimeZero;

    // Use the threshold crossing (true), or average (false) time.
    bool fUseThresholdTime;

    // The integration window for the ADC.
    double fIntegrationWindow;

    // The dead time after a hit on an ADC.
    double fDeadTime;

    // The calibration constant for time.
    double fDigitsPerNanosecond;

    // The calibration constant for charge.
    double fDigitsPerCharge;
};
#endif
