#ifndef ERepSimTPCdef_hxx_seen
#define ERepSimTPCdef_hxx_seen

namespace ERepSim {
    class TPCdef;

    enum TPC_id{
        DOWNSTREAM = 0,
        TOP,
        BOTTOM,
    };
}

class ERepSim::TPCdef{
public:
    inline static const char* names[3] = {"TPC_DOWNSTREAM", "TPC_TOP", "TPC_BOTTOM"};
    inline static const char* hitContainers[3] = {"voltpc", "voltpcTop", "voltpcBot"};
    inline static const char* volNames[3] = {"volTpc_PV_0", "volTpcTop_PV_0", "volTpcBot_PV_0"};
    // inline static const float boundaries[3][3][2] = {
    //                                             {//DOWNSTREAM
    //                                                 {-1640, 1640},//XLIM
    //                                                 {-1500, 1500},//YLIM
    //                                                 {5510, 6280}//ZLIM
    //                                             },
    //                                             {//TOP
    //                                                 {-1640, 1640},//XLIM
    //                                                 {1180, 1750},//YLIM
    //                                                 {4093.5, 5570},//ZLIM

    //                                             },
    //                                             {//BOTTOM
    //                                                 {-1640, 1640},//XLIM
    //                                                 {-1750, -1180},//YLIM
    //                                                 {4093.5, 5510},//ZLIM
    //                                             }
    //                                         };
};

#endif