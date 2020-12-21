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

    //Computes the steps of size fStepSize between two points
    std::vector<std::pair<TLorentzVector, double>> ComputeSteps(TLorentzVector& startPoint, TLorentzVector& stopPoint);

    //Generates a certain amount of electrons at a given position of a segment
    void GenerateElectrons(int segId, const TG4HitSegment& seg, TLorentzVector generationPoint, double segLength);
    //Drifts the electrons towards anode (MM)
    void DriftElectrons(int segId, const TG4HitSegment& seg, int sensId, double nbElectrons, TLorentzVector generationPoint);
    //Computes the MM amplification
    void MMAmplification(int segId, const TG4HitSegment& seg, int sensId, double nbElectrons,
                        double avg_t, double sigma_t, TLorentzVector generationPoint);
    //Applies charge spreading due to resistive pads
    void SpreadCharge(int segId, const TG4HitSegment& seg,
                    int sensId, double nbElectrons,
                    double avg_t, double sigma_t,
                    TLorentzVector generationPoint);
    //Adding a hit in a given pad
    void AddHit(int segId, const TG4HitSegment& seg,
            int sensId, double nbElectrons,
            double avg_t, double sigma_t,
            TLorentzVector generationPoint);

    /// Turn a plane, pad_y and pad_z indexes into a sensor id (or a cube id).
    int GetSensorId(int plane, int pad_y, int pad_z);

    void GetPadInfo(int sensId, int &plane, int& pad_y, int& pad_z);

    //Returns drift time in ns
    double GetDriftTime(double dist);

    //Returns longitudinal spread time in ns
    double GetSigmaDriftTime(double dist);


    //Returns longitudinal spread in mm
    double GetLongitudinalSpread(double dist);
    //Returns 3d length between two points
    double Length(const TLorentzVector& A, const TLorentzVector& B);

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

    //Step size for electron generation along tracks
    double fStepSize;
    //Ionisation coefficient of gas
    double fWI;
    //MM gain
    double fGainMM;
};

#endif
