#ifndef ERepSimResponse3DST_hxx_seen
#define ERepSimResponse3DST_hxx_seen

#include "ERepSimResponseBase.hxx"
#include <TG4HitSegment.h>

namespace ERepSim {
    class Response3DST;
}

class ERepSim::Response3DST: public ERepSim::ResponseBase {
public:
    Response3DST();
    virtual ~Response3DST();

    virtual void Initialize();

    virtual void Process(const TG4HitSegmentContainer& segments);

    virtual void Reset();

private:
    /// Forage information out of the geometry.
    bool RecurseGeometry();

    /// Add the energy deposition that occured at a particular average point.
    /// This deals with the crosstalk.
    void AddDeposit(int segId, const TG4HitSegment* seg,
                    TLorentzVector pos, double deposit);

    /// Add the energy dposition that occurred to a cube.  This adds it to the
    /// particular cube, and doesn't do cross talk (used by AddDeposition)
    void AddCubeDeposit(int segId, const TG4HitSegment* seg,
                        TLorentzVector cube, double deposit);

    /// Add the energy deposition that occurred to a fiber.
    void AddFiberDeposit(int sensId, int segId, const TG4HitSegment* seg,
                         TLorentzVector fiber, TLorentzVector cube,
                         double fiberLength, double deposit);

    /// Add a photons to one fiber.
    void AddFiberPhotons(int sensId, int segId, const TG4HitSegment* seg,
                         TLorentzVector fiber, TLorentzVector cube,
                         int photons);

    /// Calculate the photon survival based on the distance the light travels
    /// in the fiber.  It uses a two component model for attenuation (long and
    /// short).
    double PhotonSurvivalProb(double x);

    /// Genarate a random lifetime for the metastable state making a photon to
    /// live.
    double GetDecayTime();

    /// Get the index of the cube, bar or plane.
    int GetCube(TLorentzVector cube);
    int GetBar(TLorentzVector cube);
    int GetPlane(TLorentzVector cube);

    /// Turn a cube, bar and plane index into a sensor id (or a cube id)..  If
    /// one of them is a sensor, then pass a negative number.  Use -1 for a
    /// sensor on the low index end, and -2 for a sensor on the high index
    /// end.
    int GetSensorId(int c, int b, int p);

    /// The number of photons generated in the cube per MeV.  This includes
    /// all of the multiplicative factors, including the sensor geometric and
    /// quantum efficiency.  One third of the photons go down each fiber.
    double fPhotPerMeV;

    /// The light velocity in mm/ns
    double fLightVelocity;

    /// The energy leakage in each dimension.
    double fXLeakage;
    double fYLeakage;
    double fZLeakage;

    /// The number of cubes in a bar.
    int fCubes;

    /// The min/max X positions for the cubes
    double fCubeMin;
    double fCubeMax;

    /// The total number of bars of cubes in the detector.
    int fBars;

    /// The min/max Y positions for the bars
    double fBarMin;
    double fBarMax;

    /// The total number of planes of cubes in the detector.  The planes are
    /// ordered along the Z axis.
    int fPlanes;

    /// The min/max Z positions for the planes
    double fPlaneMin;
    double fPlaneMax;

    /// The pitch between cubes in each dimension.
    double fXPitch;
    double fYPitch;
    double fZPitch;
};

#endif
