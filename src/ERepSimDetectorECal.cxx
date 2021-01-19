#include "ERepSimDetectorECal.hxx"
#include "ERepSimResponseBase.hxx"
#include "ERepSimDigiHit.hxx"
#include "ERepSimUnits.hxx"
#include "ERepSimDefs.hxx"

#include "ERepSimOutput.hxx"

#define ERepSimECalDigit_cxx
#include "ERepSimECalDigit.hxx"

#include <TG4Event.h>

#include <TGeoManager.h>
#include <TTree.h>

#include <iostream>

ERepSim::DetectorECal::DetectorECal(TTree* tDigitTree)
    : DetectorBase("ECal") {
    fDigitTree = new ERepSimECalDigit(tDigitTree);
}

ERepSim::DetectorECal::~DetectorECal() {
    std::cout << "DetectorECal::Deconstruct" << std::endl;
}

void ERepSim::DetectorECal::Initialize() {
    std::cout << "DetectorECal::Initialize" << std::endl;
}

void ERepSim::DetectorECal::Reset() {
    std::cout << "DetectorECal::Reset" << std::endl;
    DetectorBase::Reset();
}

void ERepSim::DetectorECal::Process(int entry, TG4Event* event) {
    if (!fDigitTree) return;
    fDigitTree->GetEntry(entry);

    fCurrentEvent = event;

    int generatedHits = 0;
    std::cout << "DetectorECal::Process " << fDigitTree->cell_
              << " cells"
              << std::endl;

    for (int i = 0; i<fDigitTree->cell_; ++i) {
        if (fDigitTree->cell_mod[i] > 29) continue;
        // Do sensor 1
        // The id is ddddd mmmmmm
        // ddddd (bits 27-31 range 0-31) -- The sub detector definition (12)
        // mmmmmmm (bits 21-26, range 0-63) -- the module number.
        // llll (bits 17-20, range 0-15) -- The layer number
        // cccccccc (bits 9-16, range 0-255) -- The cell number
        // xxxxxxxx (bits 1-8, reserved)
        // e (bit 0) -- The end of the bar.
        int cell_id1 = ERepSim::Def::Detector::kECal << 27;
        cell_id1 += fDigitTree->cell_mod[i] << 21;
        cell_id1 += fDigitTree->cell_lay[i] << 17;
        cell_id1 += fDigitTree->cell_cel[i] << 9;
        cell_id1 += 0;
        double tAvg = 0.0;
        double ttAvg = 0.0;
        double tWidth = 0.0;
        if (fDigitTree->cell_pe_time1[i].size() > 0) {
            for (double t : fDigitTree->cell_pe_time1[i]) {
                tAvg += t;
                ttAvg += t*t;
            }
            tAvg /= fDigitTree->cell_pe_time1[i].size();
            ttAvg /= fDigitTree->cell_pe_time1[i].size();
            if (tWidth < 0.0) tWidth = 0.0;
            tWidth = std::sqrt(tWidth);
        }
        else {
            tWidth = 2.0*unit::ns;
        }

        ERepSim::DigiHit cell1(cell_id1);
        TVector3 pos1;
        if (fDigitTree->cell_mod[i] < 30) {
            pos1.SetX(fDigitTree->cell_x[i] - fDigitTree->cell_l[i]/2.0);
            pos1.SetY(fDigitTree->cell_y[i]);
            pos1.SetZ(fDigitTree->cell_z[i]);
        }
        else {
            pos1.SetX(fDigitTree->cell_x[i]);
            pos1.SetY(fDigitTree->cell_y[i] - fDigitTree->cell_l[i]/2.0);
            pos1.SetZ(fDigitTree->cell_z[i]);
        }
        cell1.SetPosition(pos1.X(),pos1.Y(),pos1.Z());
        double q1 = fDigitTree->cell_adc1[i]/4.0;
        cell1.GetCharges().push_back(q1);
        double t1 = fDigitTree->cell_tdc1[i];
        cell1.GetTimes().push_back(t1);
        cell1.GetTimeWidths().push_back(tWidth);
        if (q1 > 1.0) {
            PackDigiHit(cell1);
            ++generatedHits;
        }

        // Do sensor 2
        int cell_id2 = ERepSim::Def::Detector::kECal << 27;
        cell_id2 += fDigitTree->cell_mod[i] << 21;
        cell_id2 += fDigitTree->cell_lay[i] << 17;
        cell_id2 += fDigitTree->cell_cel[i] << 9;
        cell_id2 += 1;
        tAvg = 0.0;
        ttAvg = 0.0;
        tWidth = 0.0;
        if (fDigitTree->cell_pe_time2[i].size() > 0) {
            for (double t : fDigitTree->cell_pe_time2[i]) {
                tAvg += t;
                ttAvg += t*t;
            }
            tAvg /= fDigitTree->cell_pe_time2[i].size();
            ttAvg /= fDigitTree->cell_pe_time2[i].size();
            if (tWidth < 0.0) tWidth = 0.0;
            tWidth = std::sqrt(tWidth);
        }
        else {
            tWidth = 2.0*unit::ns;
        }
        ERepSim::DigiHit cell2(cell_id2);
        TVector3 pos2;
        if (fDigitTree->cell_mod[i] < 30) {
            pos2.SetX(fDigitTree->cell_x[i] + fDigitTree->cell_l[i]/2.0);
            pos2.SetY(fDigitTree->cell_y[i]);
            pos2.SetZ(fDigitTree->cell_z[i]);
        }
        else {
            pos2.SetX(fDigitTree->cell_x[i]);
            pos2.SetY(fDigitTree->cell_y[i] + fDigitTree->cell_l[i]/2.0);
            pos2.SetZ(fDigitTree->cell_z[i]);
        }
        cell2.SetPosition(pos2.X(),pos2.Y(),pos2.Z());
        double q2 = fDigitTree->cell_adc2[i]/4.0;
        cell2.GetCharges().push_back(q2);
        double t2 = fDigitTree->cell_tdc2[i];
        cell2.GetTimes().push_back(t2);
        cell2.GetTimeWidths().push_back(tWidth);
        if (q2 > 1.0) {
            PackDigiHit(cell2);
            ++generatedHits;
        }
    }

    // Pack all of the ecal hit segments.
    TG4HitSegmentDetectors& segments = event->SegmentDetectors;
    const TG4HitSegmentContainer&  g4Hits = segments["EMCalSci"];
    for (TG4HitSegmentContainer::const_iterator s = g4Hits.begin();
         s != g4Hits.end(); ++s) {
        TLorentzVector sPos;
        TLorentzVector sPos1;
        TLorentzVector sPos2;
        int pdg = 0;
        int trackId = -1;
        double ener = s->GetEnergyDeposit();
        sPos1 = s->GetStart();
        sPos2 = s->GetStop();
        sPos = 0.5*(sPos1 + sPos2);
        if (!s->Contrib.empty()) {
            int indx = s->Contrib.front();
            pdg = fCurrentEvent->Trajectories[indx].GetPDGCode();
            trackId = fCurrentEvent->Trajectories[indx].GetTrackId();
        }
        ERepSim::Output::Get().SegmentIds.push_back(
            ERepSim::ResponseBase::GetNextSegmentIdentifier());
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


    std::cout << "DetectorECal::Process " << generatedHits
              << " hits generated" << std::endl;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
