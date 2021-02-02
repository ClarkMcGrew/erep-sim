#include "ERepSimDetectorBase.hxx"
#include "ERepSimDigiHit.hxx"
#include "ERepSimOutput.hxx"

#include <TG4Event.h>
#include <TG4HitSegment.h>
#include <TG4Trajectory.h>

#include <iostream>
#include <set>
#include <map>

ERepSim::DetectorBase::DetectorBase(const char *modelname)
    : fModelName(modelname) {}

ERepSim::DetectorBase::~DetectorBase() { }

void ERepSim::DetectorBase::Reset() {
    // std::cout << "DetectorBase::Reset" << std::endl;
}

void ERepSim::DetectorBase::PackDigiHit(const ERepSim::DigiHit& hit) {
    ERepSim::Output::Get().HitSensorId.push_back(
        hit.GetSensorId());
    ERepSim::Output::Get().HitX.push_back(hit.GetPosition().X());
    ERepSim::Output::Get().HitY.push_back(hit.GetPosition().Y());
    ERepSim::Output::Get().HitZ.push_back(hit.GetPosition().Z());
    ERepSim::Output::Get().HitTime.push_back(hit.GetTime());
    ERepSim::Output::Get().HitTimeWidth.push_back(hit.GetTimeWidth());
    ERepSim::Output::Get().HitCharge.push_back(hit.GetCharge());
    ERepSim::Output::Get().HitSegmentBegin.push_back(
        ERepSim::Output::Get().SegmentIds.size());
    PackImpulses(hit.GetImpulses());
    ERepSim::Output::Get().HitSegmentEnd.push_back(
        ERepSim::Output::Get().SegmentIds.size());
}

void ERepSim::DetectorBase::PackImpulses(
    const ERepSim::Impulse::Container& impulses) {
    std::map<int,TG4HitSegment> segmentIds;
    for (ERepSim::Impulse::Container::const_iterator i = impulses.begin();
         i != impulses.end(); ++i) {
        const ERepSim::Carrier::Container& carriers = (*i)->GetCarriers();
        for (ERepSim::Carrier::Container::const_iterator c = carriers.begin();
             c != carriers.end(); ++c) {
            segmentIds[(*c)->GetSegmentId()] = (*c)->GetSegment();
        }
    }
    for (std::map<int,TG4HitSegment>::iterator s
             = segmentIds.begin();
         s != segmentIds.end(); ++s) {
        double ener = -1.0;
        TLorentzVector sPos;
        TLorentzVector sPos1;
        TLorentzVector sPos2;
        int pdg = 0;
        int trackId = -1;
        const TG4HitSegment g4Seg = s->second;
        ener = g4Seg.GetEnergyDeposit();
        sPos1 = g4Seg.GetStart();
        sPos2 = g4Seg.GetStop();
        sPos = 0.5*(sPos1 + sPos2);
        if (!g4Seg.Contrib.empty()) {
            trackId = g4Seg.Contrib.front();
            pdg = ERepSim::Output::Get().TrajectoryPDG[trackId];
        }
        ERepSim::Output::Get().SegmentIds.push_back(s->first);
        ERepSim::Output::Get().SegmentTrackId.push_back(trackId);
        ERepSim::Output::Get().SegmentEnergy.push_back(ener);
        ERepSim::Output::Get().SegmentPDG.push_back(pdg);
        ERepSim::Output::Get().SegmentX1.push_back(sPos1.X());
        ERepSim::Output::Get().SegmentY1.push_back(sPos1.Y());
        ERepSim::Output::Get().SegmentZ1.push_back(sPos1.Z());
        ERepSim::Output::Get().SegmentX2.push_back(sPos2.X());
        ERepSim::Output::Get().SegmentY2.push_back(sPos2.Y());
        ERepSim::Output::Get().SegmentZ2.push_back(sPos2.Z());
        ERepSim::Output::Get().SegmentT.push_back(sPos.T());
    }
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
