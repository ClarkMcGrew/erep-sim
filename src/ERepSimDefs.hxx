#ifndef ERepSimDefs_hxx_seen
#define ERepSimDefs_hxx_seen

namespace ERepSim {
    namespace Def {

        // The subdetector identifiers.  These have to fit in 5 bits (1-31).
        // The zero value is reserved to make sure that the sub-detector field
        // is always non-zero.
        namespace Detector {
            enum Detectors {
                kReserved = 0,
                kECal = 12,
                k3DST = 13,
                kTPC = 25,      // The downstream TPC
                kTopTPC = 26,
                kBotTPC = 27,
                kSTT = 31,
            };
        }
    }
}


#endif

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
