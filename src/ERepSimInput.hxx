#ifndef ERepSimExport_hxx_seen
#define ERepSimExport_hxx_seen

#include <TFile.h>
#include <TTree.h>
#include <TRef.h>

#include <vector>
#include <map>

namespace ERepSim {
    class Input;
}

/// A singleton setup to read the ERepSim tree that is suitable to be copied
/// into other projects.  The format of the ERepSim tree is part of the
/// ERepSim API, so I'll try to keep it backward compatible.
class ERepSim::Input {
private:
    Input()
        : DataTree(NULL), RunId(0), EventId(0),
          HitSensorId(NULL),
          HitX(NULL), HitY(NULL), HitZ(NULL),
          HitTime(NULL), HitTimeWidth(NULL), HitCharge(NULL),
          HitSegmentBegin(NULL), HitSegmentEnd(NULL),
          HitContribBegin(NULL), HitContribEnd(NULL),
          SegmentIds(NULL), SegmentPDG(NULL), SegmentEnergy(NULL),
          SegmentX1(NULL), SegmentY1(NULL), SegmentZ1(NULL),
          SegmentX2(NULL), SegmentY2(NULL), SegmentZ2(NULL),
          SegmentT(NULL),
          ContribIds(NULL), ContribPDG(NULL), ContribMomentum(NULL),
          PropertyTree(NULL), Property(NULL), PropertyString(NULL) {}
    Input(const Input&) {}
    Input& operator=(const Input&) {return *ThisInput;}
    static Input *ThisInput;

public:
    static Input& Get(void)  {
        if (!ThisInput) {
            ThisInput = new ERepSim::Input;
        }
        return *ThisInput;
    }

    TTree* DataTree;
    int RunId;
    int EventId;
    std::vector<int> *HitSensorId;
    std::vector<float> *HitX;
    std::vector<float> *HitY;
    std::vector<float> *HitZ;
    std::vector<float> *HitTime;
    std::vector<float> *HitTimeWidth;
    std::vector<float> *HitCharge;
    std::vector<int> *HitSegmentBegin;
    std::vector<int> *HitSegmentEnd;
    std::vector<int> *HitContribBegin;
    std::vector<int> *HitContribEnd;

    std::vector<int> *SegmentIds;
    std::vector<int>   *SegmentPDG;
    std::vector<float> *SegmentEnergy;
    std::vector<float> *SegmentX1;
    std::vector<float> *SegmentY1;
    std::vector<float> *SegmentZ1;
    std::vector<float> *SegmentX2;
    std::vector<float> *SegmentY2;
    std::vector<float> *SegmentZ2;
    std::vector<float> *SegmentT;

    std::vector<int> *ContribIds;
    std::vector<int> *ContribPDG;
    std::vector<float> *ContribMomentum;

    TTree* PropertyTree;
    std::map<std::string,double> *Property;
    std::map<std::string,std::string> *PropertyString;

    void Attach(TFile* inFile) {
        PropertyTree = dynamic_cast<TTree*>(inFile->Get("ERepSimProperties"));
        if (PropertyTree) {
            PropertyTree->SetBranchAddress("Property",&Property);
            PropertyTree->SetBranchAddress("PropertyString",&PropertyString);
            PropertyTree->GetEntry(0);
        }

        DataTree = dynamic_cast<TTree*>(inFile->Get("ERepSimEvents"));
        if (DataTree) {
            DataTree->SetBranchAddress("RunId",&RunId);
            DataTree->SetBranchAddress("EventId",&EventId);
            DataTree->SetBranchAddress("HitSensorId",&HitSensorId);
            DataTree->SetBranchAddress("HitX",&HitX);
            DataTree->SetBranchAddress("HitY",&HitY);
            DataTree->SetBranchAddress("HitZ",&HitZ);
            DataTree->SetBranchAddress("HitTime",&HitTime);
            DataTree->SetBranchAddress("HitTimeWidth",&HitTimeWidth);
            DataTree->SetBranchAddress("HitCharge",&HitCharge);
            DataTree->SetBranchAddress("HitSegmentBegin",&HitSegmentBegin);
            DataTree->SetBranchAddress("HitSegmentEnd",&HitSegmentEnd);
            DataTree->SetBranchAddress("HitContribBegin",&HitContribBegin);
            DataTree->SetBranchAddress("HitContribEnd",&HitContribEnd);

            DataTree->SetBranchAddress("SegmentIds",&SegmentIds);
            DataTree->SetBranchAddress("SegmentEnergy",&SegmentEnergy);
            DataTree->SetBranchAddress("SegmentPDG",&SegmentPDG);
            DataTree->SetBranchAddress("SegmentX1",&SegmentX1);
            DataTree->SetBranchAddress("SegmentY1",&SegmentY1);
            DataTree->SetBranchAddress("SegmentZ1",&SegmentZ1);
            DataTree->SetBranchAddress("SegmentX2",&SegmentX2);
            DataTree->SetBranchAddress("SegmentY2",&SegmentY2);
            DataTree->SetBranchAddress("SegmentZ2",&SegmentZ2);
            DataTree->SetBranchAddress("SegmentT",&SegmentT);

            DataTree->SetBranchAddress("ContribIds",&ContribIds);
            DataTree->SetBranchAddress("ContribPDG",&ContribPDG);
            DataTree->SetBranchAddress("ContribMomentum",&ContribMomentum);
        }
    }

    void GetEntry(int i) {
        DataTree->GetEntry(i);
        PropertyTree->GetEntry(0);
    }
};

ERepSim::Input* ERepSim::Input::ThisInput = NULL;
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
