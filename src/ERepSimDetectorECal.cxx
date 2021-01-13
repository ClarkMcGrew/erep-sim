#include "ERepSimDetectorECal.hxx"
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

    int generatedHits = 0;
    std::cout << "DetectorECal::Process " << fDigitTree->cell_
              << " cells"
              << std::endl;

    for (int i = 0; i< fDigitTree->cell_; ++i) {
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
        for (double t : fDigitTree->cell_pe_time1[i]) {
            tAvg += t;
            ttAvg += t*t;
        }
        tAvg /= fDigitTree->cell_pe_time1[i].size();
        ttAvg /= fDigitTree->cell_pe_time1[i].size();
        double tWidth = ttAvg - tAvg*tAvg;
        if (tWidth < 0.0) tWidth = 0.0;
        tWidth = std::sqrt(tWidth);

        ERepSim::DigiHit cell1(cell_id1);
        cell1.SetPosition(fDigitTree->cell_x[i]-fDigitTree->cell_l[i]/2.0,
                         fDigitTree->cell_y[i],
                         fDigitTree->cell_z[i]);
        cell1.GetCharges().push_back(fDigitTree->cell_adc1[i]);
        cell1.GetTimes().push_back(fDigitTree->cell_tdc1[i]);
        cell1.GetTimeWidths().push_back(tWidth);
        PackDigiHit(cell1);
        ++generatedHits;

        // Do sensor 2
        int cell_id2 = ERepSim::Def::Detector::kECal << 27;
        cell_id2 += fDigitTree->cell_mod[i] << 21;
        cell_id2 += fDigitTree->cell_lay[i] << 17;
        cell_id2 += fDigitTree->cell_cel[i] << 9;
        cell_id2 += 1;
        tAvg = 0.0;
        ttAvg = 0.0;
        for (double t : fDigitTree->cell_pe_time2[i]) {
            tAvg += t;
            ttAvg += t*t;
        }
        tAvg /= fDigitTree->cell_pe_time2[i].size();
        ttAvg /= fDigitTree->cell_pe_time2[i].size();
        tWidth = ttAvg - tAvg*tAvg;
        if (tWidth < 0.0) tWidth = 0.0;
        tWidth = std::sqrt(tWidth);

        ERepSim::DigiHit cell2(cell_id2);
        cell2.SetPosition(fDigitTree->cell_x[i]+fDigitTree->cell_l[i]/2.0,
                         fDigitTree->cell_y[i],
                         fDigitTree->cell_z[i]);
        cell2.GetCharges().push_back(fDigitTree->cell_adc2[i]);
        cell2.GetTimes().push_back(fDigitTree->cell_tdc2[i]);
        cell2.GetTimeWidths().push_back(tWidth);
        PackDigiHit(cell2);
        ++generatedHits;
    }

    std::cout << "DetectorECal::Process " << generatedHits
              << " hits generated" << std::endl;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
