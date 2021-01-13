#ifndef ERepSimECal_hxx_seen
#define ERepSimECal_hxx_seen

#include "ERepSimDetectorBase.hxx"

#include <TTree.h>

namespace ERepSim {
    class DetectorECal;
}
class tDigit;

class ERepSim::DetectorECal: public ERepSim::DetectorBase {
public:
    DetectorECal(TTree* tDigitTree);
    virtual ~DetectorECal();

    virtual void Initialize();

    virtual void Process(int entry, TG4Event* event);

    virtual void Reset();

private:
    tDigit* fDigitTree;
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
