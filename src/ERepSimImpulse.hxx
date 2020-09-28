#ifndef ERepSimImpulse_hxx_seen
#define ERepSimImpulse_hxx_seen

#include "ERepSimCarrier.hxx"

namespace ERepSim {
    class Impulse;
}

// This describes an impulse generated by a carrier (usually some electric
// current) in a sensor that is being sent to the digitizing electronics.
// It's OK to think of the impulse as a Gaussian pulse of current.  A sensor
// may send overlapping impulses (i.e. impulses that occur at the same time).
class ERepSim::Impulse {
public:
    /// The container of impulses.
    typedef std::vector<std::shared_ptr<ERepSim::Impulse>> Container;

    /// The type of a map from the sensor identifier to the impulses
    /// associated with that sensor.
    typedef std::map<int,ERepSim::Impulse::Container> Map;

    explicit Impulse(int sensorId);
    Impulse(int sensorId, std::shared_ptr<ERepSim::Carrier> carrier);
    virtual ~Impulse();

    int GetSensorId() const {return fSensorId;}

    /// A pointer to the carriers that generated this impulse. It is empty if
    /// the impulse was generated by sensor noise (or something similar).  It
    /// will usually contain one carrier.
    const ERepSim::Carrier::Container& GetCarriers() const {return fCarriers;}

    /// Add a carrier to the impulse.
    void AddCarrier(std::shared_ptr<ERepSim::Carrier> c) {
        fCarriers.push_back(c);
    }

    /// The position of the sensor.  This is usually a copy of the Carrier
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

    /// The time of the impulse.  The details will depend on the sensor model,
    /// but is usually either the peak time (for a Gaussian model), or the
    /// start time of a peaking time model.
    double GetTime() {return fTime;}
    void SetTime(double t) {fTime = t;}

    /// A parameter describing the width.  The details will depend on the
    /// sensor model, but it is usually either the Gaussian width, or the
    /// peaking time of the impulse.
    double GetWidth() {return fWidth;}
    void SetWidth(double w) {fWidth = w;}

    /// The integrated charge of the pulse.
    double GetCharge() {return fCharge;}
    void SetCharge(double q) {fCharge = q;}

private:

    /// The sensor that generated this impulse
    int fSensorId;

    /// The carrier that caused the sensor to generate this impulse.  This
    /// will be NULL if the impulse is due to noise or some other non-particle
    /// related thing.
    ERepSim::Carrier::Container fCarriers;

    /// The time of the impulse.  The details will depend on the sensor model,
    /// but is usually either the peak time (for a Gaussian model), or the
    /// start time of a peaking time model.
    double fTime;

    /// A parameter describing the width.  The details will depend on the
    /// sensor model, but it is usually either the Gaussian width, or the
    /// peaking time of the impulse.  Zero is a reasonable value if the
    /// impulse is a "delta-function".
    double fWidth;

    /// The integrated charge of the pulse.
    double fCharge;

    /// The position of the sensor.  This is usually copy of the carrier final
    /// position.
    TVector3 fPosition;
};

#endif