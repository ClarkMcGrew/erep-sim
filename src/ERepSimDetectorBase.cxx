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
        TLorentzVector sPos1;
        TLorentzVector sPos2;
        int pdg = 0;
        if (s->second) {
            ener = s->second->GetEnergyDeposit();
            sPos1 = s->second->GetStart();
            sPos2 = s->second->GetStop();
            sPos = 0.5*(sPos1 + sPos2);
            if (!s->second->Contrib.empty()) {
                int indx = s->second->Contrib.front();
                pdg = fCurrentEvent->Trajectories[indx].GetPDGCode();
            }
        }
        ERepSim::Output::Get().SegmentIds.push_back(s->first);
        ERepSim::Output::Get().SegmentEnergy.push_back(ener);
        ERepSim::Output::Get().SegmentPDG.push_back(pdg);
        ERepSim::Output::Get().SegmentX1.push_back(sPos1.X());
        ERepSim::Output::Get().SegmentY1.push_back(sPos1.Y());
        ERepSim::Output::Get().SegmentZ1.push_back(sPos1.Z());
        ERepSim::Output::Get().SegmentX2.push_back(sPos2.X());
        ERepSim::Output::Get().SegmentY2.push_back(sPos2.Y());
        ERepSim::Output::Get().SegmentZ2.push_back(sPos2.Z());
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
