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

    std::vector<int> SegmentIds;
    std::vector<float> SegmentEnergy;
    std::vector<float> SegmentPDG;
    std::vector<float> SegmentX1;
    std::vector<float> SegmentY1;
    std::vector<float> SegmentZ1;
    std::vector<float> SegmentX2;
    std::vector<float> SegmentY2;
    std::vector<float> SegmentZ2;
    std::vector<float> SegmentT;

    std::vector<int> ContribIds;
    std::vector<int> ContribPDG;
    std::vector<float> ContribMomentum;

    std::vector<int> TrajectoryId;
    std::vector<int> TrajectoryParent;
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
