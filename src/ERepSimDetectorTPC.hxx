#ifndef ERepSimTPC_hxx_seen
#define ERepSimTPC_hxx_seen

#include "ERepSimDetectorBase.hxx"
#include "ERepSimTPCdef.hxx"

namespace ERepSim {
    class DetectorTPC;
    class ResponseBase;
    class SensorBase;
    class DAQBase;
}


class ERepSim::DetectorTPC: public ERepSim::DetectorBase {
public:
    DetectorTPC(ERepSim::TPC_id tpc_id);
    virtual ~DetectorTPC();

    virtual void Initialize();

    virtual void Process(TG4Event* event);

    virtual void Reset();

private:
    ERepSim::TPC_id fTpcId;
    const char* fHitContainer;
    std::shared_ptr<ERepSim::ResponseBase> fResponse;
    std::shared_ptr<ERepSim::SensorBase> fSensor;
    std::shared_ptr<ERepSim::DAQBase> fDAQ;
};

#endif
