#include "ERepSimDetectorBase.hxx"
#include "ERepSimDigiHit.hxx"
#include "ERepSimOutput.hxx"

#include <TG4Event.h>
#include <TG4HitSegment.h>
#include <TG4Trajectory.h>

#include <iostream>
#include <set>

ERepSim::DetectorBase::DetectorBase(const char *modelname)
    : fModelName(modelname) {}

ERepSim::DetectorBase::~DetectorBase() { }

void ERepSim::DetectorBase::Reset() {
    std::cout << "DetectorBase::Reset" << std::endl;
    fCurrentEvent = NULL;
}

void ERepSim::DetectorBase::PackDigiHit(const ERepSim::DigiHit& hit) {
    ERepSim::Output::Get().HitSensorId.push_back(
        hit.GetSensorId());
    ERepSim::Output::Get().HitX.push_back(hit.GetPosition().X());
    ERepSim::Output::Get().HitY.push_back(hit.GetPosition().Y());
    ERepSim::Output::Get().HitZ.push_back(hit.GetPosition().Z());
    ERepSim::Output::Get().HitTime.push_back(hit.GetTime());
    ERepSim::Output::Get().HitTimeWidth.push_back(
        hit.GetTimeWidth());
    ERepSim::Output::Get().HitCharge.push_back(hit.GetCharge());
    ERepSim::Output::Get().HitSegmentBegin.push_back(
        ERepSim::Output::Get().SegmentIds.size());
    ERepSim::Output::Get().HitContribBegin.push_back(
        ERepSim::Output::Get().ContribIds.size());
    PackImpulses(hit.GetImpulses());
    ERepSim::Output::Get().HitSegmentEnd.push_back(
        ERepSim::Output::Get().SegmentIds.size());
    ERepSim::Output::Get().HitContribEnd.push_back(
        ERepSim::Output::Get().ContribIds.size());
}

void ERepSim::DetectorBase::PackImpulses(
    const ERepSim::Impulse::Container& impulses) {
    if (!fCurrentEvent) {
        throw std::runtime_error("DetectorBase::fCurrentEvent not set");
    }
    std::set<std::pair<int,const TG4HitSegment*>> segmentIds;
    for (ERepSim::Impulse::Container::const_iterator i = impulses.begin();
         i != impulses.end(); ++i) {
        const ERepSim::Carrier::Container& carriers = (*i)->GetCarriers();
        for (ERepSim::Carrier::Container::const_iterator c = carriers.begin();
             c != carriers.end(); ++c) {
            segmentIds.insert(std::make_pair((*c)->GetSegmentId(),
                                             (*c)->GetSegment()));
        }
    }
    std::set<int> contrib;
    for (std::set<std::pair<int,const TG4HitSegment*>>::iterator s
             = segmentIds.begin();
         s != segmentIds.end(); ++s) {
        double ener = -1.0;
        TLorentzVector sPos;
        if (s->second) {
            ener = s->second->GetEnergyDeposit();
            sPos = 0.5*(s->second->GetStart() + s->second->GetStop());
        }
        ERepSim::Output::Get().SegmentIds.push_back(s->first);
        ERepSim::Output::Get().SegmentEnergy.push_back(ener);
        ERepSim::Output::Get().SegmentX.push_back(sPos.X());
        ERepSim::Output::Get().SegmentY.push_back(sPos.Y());
        ERepSim::Output::Get().SegmentZ.push_back(sPos.Z());
        ERepSim::Output::Get().SegmentT.push_back(sPos.T());
        for (std::vector<int>::const_iterator t = s->second->Contrib.begin();
             t != s->second->Contrib.end(); ++t) {
            contrib.insert(*t);
        }
    }
    for(std::set<int>::iterator t = contrib.begin(); t != contrib.end(); ++t) {
        ERepSim::Output::Get().ContribIds.push_back(*t);
        ERepSim::Output::Get().ContribPDG.push_back(
            fCurrentEvent->Trajectories[*t].GetPDGCode());
        ERepSim::Output::Get().ContribMomentum.push_back(
            fCurrentEvent->Trajectories[*t].GetInitialMomentum().P());
    }
}
