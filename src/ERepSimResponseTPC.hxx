#ifndef ERepSimResponseTPC_hxx_seen
#define ERepSimResponseTPC_hxx_seen

#include "ERepSimResponseBase.hxx"
#include "ERepSimTPCdef.hxx"
#include <TG4HitSegment.h>

namespace ERepSim {
    class ResponseTPC;
}



class ERepSim::ResponseTPC: public ERepSim::ResponseBase {
public:
    ResponseTPC(ERepSim::TPC_id id);
    virtual ~ResponseTPC();

    virtual void Initialize();

    virtual void Process(const TG4HitSegmentContainer& segments);

    virtual void Reset();

private:
    /// Forage information out of the geometry.
    bool RecurseGeometry();

    //Iterates over segment
    void AddTrack(int segId, const TG4HitSegment& seg);

    //Computes the intersections with the track 2D projection and pads
    const std::vector<TLorentzVector> ComputeCrosses(const TLorentzVector A, const TLorentzVector B);

    void AddHit(int sensId, int segId, const TG4HitSegment& seg,
                double energy, double avg_t, double sigma_t,
                TLorentzVector pos);

    /// Turn a plane, pad_y and pad_z indexes into a sensor id (or a cube id).
    int GetSensorId(int plane, int pad_y, int pad_z);

    void GetPadInfo(int sensId, int &plane, int& pad_y, int& pad_z);

    //Returns drift time in ns
    double GetDriftTime(double dist);

    //Returns longitudinal spread time in ns
    double GetSigmaDriftTime(double dist);


    //Returns longitudinal spread in mm
    double GetLongitudinalSpread(double dist);

    //Selects which of the 3 tpcs
    ERepSim::TPC_id fTpcId;

    //Pad sizes (hardcoded)
    double fPadSizeY;
    double fPadSizeZ;

    //Dimensions of the tpc
    double fXmin;
    double fXmax;
    double fYmin;
    double fYmax;
    double fZmin;
    double fZmax;
    double fCathodeX;

    //Number of pads
    int fNbPadsY;
    int fNbPadsZ;
};

#endif
