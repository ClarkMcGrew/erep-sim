#include "ERepSimDetectorBase.hxx"
#include "ERepSimDigiHit.hxx"
#include "ERepSimOutput.hxx"

#include <TG4Event.h>
#include <TG4HitSegment.h>
#include <TG4Trajectory.h>

#include <iostream>
#include <set>

namespace {
    std::map<int, const TG4HitSegment*> gDebugIdToHitSegment;
    std::map<const TG4HitSegment*, int> gDebugHitSegmentToId;
}

ERepSim::DetectorBase::DetectorBase(const char *modelname)
    : fModelName(modelname) {}

ERepSim::DetectorBase::~DetectorBase() { }

void ERepSim::DetectorBase::Validate() {
    for (std::map<int, const TG4HitSegment*>::iterator itoh
             = gDebugIdToHitSegment.begin();
         itoh != gDebugIdToHitSegment.end();
         ++itoh) {
        std::map<const TG4HitSegment*, int>::iterator  htoi
            = gDebugHitSegmentToId.find(itoh->second);
        if (htoi == gDebugHitSegmentToId.end()) {
            std::cout << "Hit segment is missing for id: "
                      << itoh->first
                      << std::endl;
            throw std::runtime_error("Missing segment identifiers");
        }
        if (htoi->second != itoh->first || htoi->first != itoh->second) {
            std::cout << "Id is duplicated for hit segment." << std::endl;
            std::cout << "itoh->first " << itoh->first
                      << " htoi->second " << htoi->second
                      << std::endl;
            std::cout << "itoh->second " << (long int) itoh->second
                      << " htoi->first " << (long int) htoi->first
                      << std::endl;
            throw std::runtime_error("Duplicate segment identifiers");
        }
    }

    for (std::map<const TG4HitSegment*, int>::iterator htoi
             = gDebugHitSegmentToId.begin();
         htoi != gDebugHitSegmentToId.end();
         ++htoi) {
        std::map<int, const TG4HitSegment*>::iterator  itoh
            = gDebugIdToHitSegment.find(htoi->second);
        if (itoh == gDebugIdToHitSegment.end()) {
            std::cout << "idenfitier is missing for hit segment: "
                      << htoi->second
                      << std::endl;
            throw std::runtime_error("Missing segment identifiers");
        }
        if (htoi->second != itoh->first || htoi->first != itoh->second) {
            std::cout << "Id is duplicated for hit segment." << std::endl;
            std::cout << "itoh->first: " << itoh->first
                      << " htoi->second: " << htoi->second
                      << std::endl;
            std::cout << "itoh->second: " << (long int) itoh->second
                      << " htoi->first: " << (long int) htoi->first
                      << std::endl;
            throw std::runtime_error("Duplicate segment identifiers");
        }
    }

    if (gDebugIdToHitSegment.size() != gDebugHitSegmentToId.size()) {
        std::cout << "gDebugIdToHitSegment.size(): "
                  << gDebugIdToHitSegment.size()
                  << std::endl;
        std::cout << "gDebugHitSegmentToId.size(): "
                  << gDebugHitSegmentToId.size()
              << std::endl;
        throw std::runtime_error("Missing identifiers or segments");
    }
}

void ERepSim::DetectorBase::Reset() {
    std::cout << "DetectorBase::Reset" << std::endl;
    fCurrentEvent = NULL;
    gDebugIdToHitSegment.clear();
    gDebugHitSegmentToId.clear();
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
            gDebugIdToHitSegment[(*c)->GetSegmentId()] = (*c)->GetSegment();
            gDebugHitSegmentToId[(*c)->GetSegment()] = (*c)->GetSegmentId();
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
        int trackId = -1;
        if (s->second) {
            ener = s->second->GetEnergyDeposit();
            sPos1 = s->second->GetStart();
            sPos2 = s->second->GetStop();
            sPos = 0.5*(sPos1 + sPos2);
            if (!s->second->Contrib.empty()) {
                int indx = s->second->Contrib.front();
                pdg = fCurrentEvent->Trajectories[indx].GetPDGCode();
                trackId = fCurrentEvent->Trajectories[indx].GetTrackId();
            }
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
        for (std::vector<int>::const_iterator t = s->second->Contrib.begin();
             t != s->second->Contrib.end(); ++t) {
            contrib.insert(*t);
        }
    }
    for(std::set<int>::iterator t = contrib.begin(); t != contrib.end(); ++t) {
        ERepSim::Output::Get().ContribIds.push_back(
            fCurrentEvent->Trajectories[*t].GetTrackId());
        ERepSim::Output::Get().ContribPDG.push_back(
            fCurrentEvent->Trajectories[*t].GetPDGCode());
        ERepSim::Output::Get().ContribMomentum.push_back(
            fCurrentEvent->Trajectories[*t].GetInitialMomentum().P());
    }
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
