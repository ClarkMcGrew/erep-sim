#ifndef ERepSim3DST_hxx_seen
#define ERepSim3DST_hxx_seen

#include "ERepSimDetectorBase.hxx"

namespace ERepSim {
    class Detector3DST;
    class ResponseBase;
    class SensorBase;
    class DAQBase;
}

class ERepSim::Detector3DST: public ERepSim::DetectorBase {
public:
    Detector3DST();
    virtual ~Detector3DST();

    virtual void Initialize();

    virtual void Accumulate(int entry, const TG4Event* event);

    virtual void Process(int entry);

    virtual void Reset();

private:

    std::shared_ptr<ERepSim::ResponseBase> fResponse;
    std::shared_ptr<ERepSim::SensorBase> fSensor;
    std::shared_ptr<ERepSim::DAQBase> fDAQ;
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
