#ifndef ERepSimOutput_hxx_seen
#define ERepSimOutput_hxx_seen

#include <TG4Event.h>

#include <TTree.h>
#include <TVector3.h>
#include <TRef.h>

#include <vector>
#include <map>

namespace ERepSim {
    class Output;
}

/// A singleton defining the output tree.
class ERepSim::Output {
private:
    Output();
    Output(const Output&);
    Output& operator=(const Output&);
    static Output *ThisOutput;

public:
    static Output& Get(void);

    TTree* DataTree;
    int RunId;
    int EventId;
    std::vector<int> HitSensorId;
    std::vector<float> HitX;
    std::vector<float> HitY;
    std::vector<float> HitZ;
    std::vector<float> HitTime;
    std::vector<float> HitTimeWidth;
    std::vector<float> HitCharge;
    std::vector<int> HitSegmentBegin;
    std::vector<int> HitSegmentEnd;
    std::vector<int> HitContribBegin;
    std::vector<int> HitContribEnd;

    // The identifier of the hit segment.  This is also the index of the hit
    // segment in the edep-sim TG4HitSegment in the TG4HitSegmentContainer.
    std::vector<int> SegmentIds;

    // The trajectory id of the main particle for this segment (i.e. the first
    // particle that contributed to the segment).
    std::vector<int> SegmentTrackId;

    // The parameters of the segment.
    std::vector<int> SegmentPDG;
    std::vector<float> SegmentEnergy;
    std::vector<float> SegmentX1;
    std::vector<float> SegmentY1;
    std::vector<float> SegmentZ1;
    std::vector<float> SegmentX2;
    std::vector<float> SegmentY2;
    std::vector<float> SegmentZ2;
    std::vector<float> SegmentT;

    // The trajectory id for this contributor
    std::vector<int> ContribIds;

    // A convenient vector of the PDG values.  The value is duplicated from
    // TrajectoryPDG
    std::vector<int> ContribPDG;

    // A convenient vector of momenta.. The value is equal to the magnitude of
    // the TrajectoryP[xyze] vectors.
    std::vector<float> ContribMomentum;

    // The identifier for the trajectory from geant.  The value should be
    // equal to the index.
    std::vector<int> TrajectoryId;

    // The identifier of the parent particle.  The parent will be negative
    // when this is a primary particle (check, it might be zero).
    std::vector<int> TrajectoryParent;

    // The particle information.
    std::vector<int> TrajectoryPDG;
    std::vector<float> TrajectoryX;
    std::vector<float> TrajectoryY;
    std::vector<float> TrajectoryZ;
    std::vector<float> TrajectoryT;
    std::vector<float> TrajectoryPx;
    std::vector<float> TrajectoryPy;
    std::vector<float> TrajectoryPz;
    std::vector<float> TrajectoryPe;

    TTree* PropertyTree;
    std::map<std::string,double> Property;
    std::map<std::string,std::string> PropertyString;

    void CreateTrees();
    void Reset(const TG4Event* event=NULL);
    void Fill();
    void Write();

};

#endif
