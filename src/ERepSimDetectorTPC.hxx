#ifndef ERepSimTPC_hxx_seen
#define ERepSimTPC_hxx_seen

#include "ERepSimDetectorBase.hxx"

namespace ERepSim {
    class DetectorTPC;
    class ResponseBase;
    class SensorBase;
    class DAQBase;
}


class ERepSim::DetectorTPC: public ERepSim::DetectorBase {
public:
    DetectorTPC(const char* modelName,
                const char* hitContainer,
                const char* volumeName,
                int tpc_id);
    virtual ~DetectorTPC();

    virtual void Initialize();

    virtual void Process(TG4Event* event);

    virtual void Reset();

private:
    std::string fHitContainer;
    std::string fVolumeName;
    int fTpcId;
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
