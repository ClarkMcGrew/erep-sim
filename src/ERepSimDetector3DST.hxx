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

    virtual void Process(TG4Event* event);

    virtual void Reset();

private:

    std::shared_ptr<ERepSim::ResponseBase> fResponse;
    std::shared_ptr<ERepSim::SensorBase> fSensor;
    std::shared_ptr<ERepSim::DAQBase> fDAQ;
};

#endif
