#ifndef ERepSimDigiHit_hxx_seen
#define ERepSimDigiHit_hxx_seen

#include "ERepSimImpulse.hxx"
#include "TG4HitSegment.h"

namespace ERepSim {
    class DigiHit;
}

// This describes both a digitized hit and the calibrated result that goes
// with it.  The values are held in a vector so that this can represent an
// FADC.  For a simple digitizer, the vectors will have single entries.
class ERepSim::DigiHit {
public:
    /// The container of DigiHit shared pointers.
    typedef std::vector<std::shared_ptr<ERepSim::DigiHit>> Container;

    /// The type of a map from the sensor identifier to the hits
    /// associated with that sensor.
    typedef std::map<int,ERepSim::DigiHit::Container> Map;

    explicit DigiHit(int sensorId);
    DigiHit(int sensorId, std::shared_ptr<ERepSim::Impulse> impulse);
    virtual ~DigiHit();

    int GetSensorId() const {return fSensorId;}

    /// A vector of the impulses that generated this hit.  It is empty if
    /// the hit comes from an external simulation (like the KLOE ECal).
    const ERepSim::Impulse::Container& GetImpulses() const {return fImpulses;}

    /// Add an impulse to the hit.
    void AddImpulse(std::shared_ptr<ERepSim::Impulse> c) {
        if (!fDirectSegments.empty()) {
            // Cannot have impulses and direct segments: This might be overly
            // strict, but is here at least for the initial debugging.
            throw std::runtime_error("Segments in hit when adding an impulse");
        }
        fImpulses.push_back(c);
    }

    /// The position of the sensor.  This is usually a copy of the Impulse
    /// final position.
    const TVector3& GetPosition() const {return fPosition;}

    /// Set the position of the sensor.
    void SetPosition(double x, double y, double z) {
        fPosition.SetX(x);
        fPosition.SetY(y);
        fPosition.SetZ(z);
    }

    /// Set the position of the sensor.
    void SetPosition(TVector3 pos) {
        fPosition = pos;
    }

    /// Get the vector of digitized times.
    std::vector<int>& GetDigiTimes() {return fDigiTimes;}

    // Get the vector of digitized charges.
    std::vector<int>& GetDigiCharges() {return fDigiCharges;}

    // Get the vector of calibrated times.
    std::vector<double>& GetTimes() {return fCaliTimes;}

    // Get the vector of calibrated time Width values.
    std::vector<double>& GetTimeWidths() {return fCaliTimeWidths;}

    // Get the vector of calibrated charges.
    std::vector<double>& GetCharges() {return fCaliCharges;}

    /// Return the first digitized time.  Most hits only have a single time,
    /// so this is "The Digitized Time (tm)".
    int GetDigiTime() const {
        if (fDigiTimes.size() < 1) return -1;
        return fDigiTimes.front();
    }

    /// Return the first calibrated time Width.  Most hits only have a single
    /// time Width, so this is "The Width (tm)".
    int GetDigiCharge() const {
        if (fDigiCharges.size() < 1) return -1;
        return fDigiCharges.front();
    }

    /// Return the first calibrated time.  Most hits only have a single time,
    /// so this is "The Time (tm)".
    double GetTime() const {
        if (fCaliTimes.size() < 1) return -1E+10;
        return fCaliTimes.front();
    }

    /// Return the first calibrated time Width.  Most hits only have a single
    /// time Width, so this is "The Width (tm)".
    double GetTimeWidth() const {
        if (fCaliTimeWidths.size() < 1) return -1.0;
        return fCaliTimeWidths.front();
    }

    /// Return the first calibrated charge.  Most hits only have a single
    /// charge, so this is "The Charge (tm)".
    double GetCharge() const {
        if (fCaliCharges.size() < 1) return -1;
        return fCaliCharges.front();
    }

    /// Add a direct hit segment to this hit.  This will prevent adding the
    /// impulses to the output.  It's intended to allow overlaying of hits,
    /// not just the energy deposit.
    void AddDirectHitSegment(int segId, const TG4HitSegment& seg);

    /// This should only be used in ERepSimDetectorBase.  It could be handled
    /// using "friend".
    const std::map<int, TG4HitSegment>& GetDirectSegments() const {
        return fDirectSegments;
    }

private:

    /// The sensor that generated this impulse
    int fSensorId;

    /// The position of the sensor.  This is usually copy of the carrier final
    /// position.
    TVector3 fPosition;

    /// The carrier that caused the sensor to generate this impulse.  This
    /// will be NULL if the impulse is due to noise or some other non-particle
    /// related thing.
    ERepSim::Impulse::Container fImpulses;

    /// The digitized time(s) for the DigiHit.
    std::vector<int> fDigiTimes;

    /// The calibrated time(s) for the DigiHit
    std::vector<double> fCaliTimes;

    /// The "Width" for the calibrated time(s).  This might not be used in all
    /// DigiHit objects.
    std::vector<double> fCaliTimeWidths;

    /// The digitzed charge(s).
    std::vector<int> fDigiCharges;

    /// The calibrated charge(s).
    std::vector<double> fCaliCharges;

    /// Any segments that got directly attached to this hit.  When there are
    /// direct segments, then the impulses should not be processed separately.
    /// The key is the segment id (so we can keep the segments unique).
    std::map<int, TG4HitSegment> fDirectSegments;
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
