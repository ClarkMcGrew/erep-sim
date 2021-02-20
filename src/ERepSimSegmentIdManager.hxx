#ifndef ERepSimSegmentIdManager_hxx_seen
#define ERepSimSegmentIdManager_hxx_seen

namespace ERepSim {
    class SegmentIdManager;
}

class ERepSim::SegmentIdManager {
public:
    SegmentIdManager();
    ~SegmentIdManager();

    int GetNextSegmentIdentifier();

    int GetCurrentSegmentIdentifier() {
        return fSegmentIdentifier;
    }

private:
    static int fSegmentIdentifier;
};
#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
