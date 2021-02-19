#ifndef ERepSimECal_hxx_seen
#define ERepSimECal_hxx_seen

#include "ERepSimDetectorBase.hxx"
#include "ERepSimDigiHit.hxx"

#include "ECal_Cell.hxx"

#include <TTree.h>

#include <vector>

namespace ERepSim {
    class DetectorECal;
}

class ERepSim::DetectorECal: public ERepSim::DetectorBase {
public:
    DetectorECal(TTree* tDigitTree);
    virtual ~DetectorECal();

    virtual void Initialize();

    virtual void Accumulate(int entry, const TG4Event* event);

    virtual void Process(int entry);

    virtual void Reset();

    void SetIntegrationWindow(double w) {fIntegrationWindow = w;}

private:
    // The input tree and the branch variables prepared by the sand-stt code.
    TTree* fDigitTree;
    std::vector<cell>* fCells;

    // All of the hits that have been found for the event.  This is filled
    // from the sand-stt tree when it is read by Accumulate(), and then
    // processed to fix the very simplistic STT electronics models during
    // Process().
    ERepSim::DigiHit::Map fHits;

    // The integration window in the ECal.  This is used when combining the
    // input events from SAND-STT.
    double fIntegrationWindow;

};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
