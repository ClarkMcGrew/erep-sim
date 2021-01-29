#include "ERepSimOutput.hxx"

#include <iostream>

ERepSim::Output* ERepSim::Output::ThisOutput = NULL;

ERepSim::Output& ERepSim::Output::Get(void) {
    if (!ThisOutput) {
        ThisOutput = new ERepSim::Output;
    }
    return *ThisOutput;
}

ERepSim::Output::Output() {}
ERepSim::Output::Output(const Output&) {}
ERepSim::Output& ERepSim::Output::operator=(const Output&) {return *ThisOutput;}

void ERepSim::Output::CreateTrees() {
    PropertyTree = new TTree("ERepSimProperties","ERepSim Properties");
    PropertyTree->Branch("Property",&Property);
    PropertyTree->Branch("PropertyString",&PropertyString);

    DataTree = new TTree("ERepSimEvents","Energy Response");
    DataTree->Branch("RunId",&RunId,"RunId/I");
    DataTree->Branch("EventId",&EventId,"EventId/I");
    DataTree->Branch("HitSensorId",&HitSensorId);
    DataTree->Branch("HitX",&HitX);
    DataTree->Branch("HitY",&HitY);
    DataTree->Branch("HitZ",&HitZ);
    DataTree->Branch("HitTime",&HitTime);
    DataTree->Branch("HitTimeWidth",&HitTimeWidth);
    DataTree->Branch("HitCharge",&HitCharge);
    DataTree->Branch("HitSegmentBegin",&HitSegmentBegin);
    DataTree->Branch("HitSegmentEnd",&HitSegmentEnd);

    DataTree->Branch("SegmentIds",&SegmentIds);
    DataTree->Branch("SegmentTrackId",&SegmentTrackId);
    DataTree->Branch("SegmentPDG",&SegmentPDG);
    DataTree->Branch("SegmentEnergy",&SegmentEnergy);
    DataTree->Branch("SegmentX1",&SegmentX1);
    DataTree->Branch("SegmentY1",&SegmentY1);
    DataTree->Branch("SegmentZ1",&SegmentZ1);
    DataTree->Branch("SegmentX2",&SegmentX2);
    DataTree->Branch("SegmentY2",&SegmentY2);
    DataTree->Branch("SegmentZ2",&SegmentZ2);
    DataTree->Branch("SegmentT",&SegmentT);

    DataTree->Branch("TrajectoryId",&TrajectoryId);
    DataTree->Branch("TrajectoryParent",&TrajectoryParent);
    DataTree->Branch("TrajectoryPDG",&TrajectoryPDG);
    DataTree->Branch("TrajectoryX",&TrajectoryX);
    DataTree->Branch("TrajectoryY",&TrajectoryY);
    DataTree->Branch("TrajectoryZ",&TrajectoryZ);
    DataTree->Branch("TrajectoryT",&TrajectoryT);
    DataTree->Branch("TrajectoryPx",&TrajectoryPx);
    DataTree->Branch("TrajectoryPy",&TrajectoryPy);
    DataTree->Branch("TrajectoryPz",&TrajectoryPz);
    DataTree->Branch("TrajectoryPe",&TrajectoryPe);

}

void ERepSim::Output::Reset(int runId, int eventId) {
    RunId = runId;
    EventId = eventId;
    TrajectoryIdOffset = 0;

    HitSensorId.clear();
    HitX.clear();
    HitY.clear();
    HitZ.clear();
    HitTime.clear();
    HitTimeWidth.clear();
    HitCharge.clear();
    HitSegmentBegin.clear();
    HitSegmentEnd.clear();

    SegmentIds.clear();
    SegmentTrackId.clear();
    SegmentPDG.clear();
    SegmentEnergy.clear();
    SegmentX1.clear();
    SegmentY1.clear();
    SegmentZ1.clear();
    SegmentX2.clear();
    SegmentY2.clear();
    SegmentZ2.clear();
    SegmentT.clear();

    TrajectoryId.clear();
    TrajectoryParent.clear();
    TrajectoryPDG.clear();
    TrajectoryX.clear();
    TrajectoryY.clear();
    TrajectoryZ.clear();
    TrajectoryT.clear();
    TrajectoryPx.clear();
    TrajectoryPy.clear();
    TrajectoryPz.clear();
    TrajectoryPe.clear();
}

void ERepSim::Output::Update(TG4Event* event) {
    if (!event) return;

    TrajectoryIdOffset = TrajectoryId.size();
    std::cout << "ERepSim::Output::Update: Trajectory offset is "
              << TrajectoryIdOffset
              << std::endl;

    for (TG4Trajectory traj : event->Trajectories) {
        int trackId = traj.GetTrackId();
        if (trackId >= 0) trackId += TrajectoryIdOffset;
        TrajectoryId.push_back(trackId);

        int parentId = traj.GetParentId();
        if (parentId >= 0) parentId += TrajectoryIdOffset;
        TrajectoryParent.push_back(parentId);

        TrajectoryPDG.push_back(traj.GetPDGCode());

        TrajectoryX.push_back(traj.Points.front().GetPosition().X());
        TrajectoryY.push_back(traj.Points.front().GetPosition().Y());
        TrajectoryZ.push_back(traj.Points.front().GetPosition().Z());
        TrajectoryT.push_back(traj.Points.front().GetPosition().T());

        TrajectoryPx.push_back(traj.GetInitialMomentum().X());
        TrajectoryPy.push_back(traj.GetInitialMomentum().Y());
        TrajectoryPz.push_back(traj.GetInitialMomentum().Z());
        TrajectoryPe.push_back(traj.GetInitialMomentum().E());
    }

}

void ERepSim::Output::Fill() {
    DataTree->Fill();
}

void ERepSim::Output::Write() {
    DataTree->Write();
    PropertyTree->Fill();
    PropertyTree->Write();
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
