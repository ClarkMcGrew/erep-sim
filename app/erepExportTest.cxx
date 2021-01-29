#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TVector.h>

#include <iostream>
#include <exception>
#include <memory>

#include "../src/ERepSimInput.hxx"

int main(int argc, char **argv) {
    std::cout << "erepReadWriteTest: Hello World" << std::endl;

    if (argc < 2) {
        throw std::runtime_error("Missing input file");
    }
    std::string inputName(argv[1]);
    std::cout << "Input Name " << inputName << std::endl;

    std::unique_ptr<TFile> inputFile(new TFile(inputName.c_str(),"old"));
    if (!inputFile->IsOpen()) {
        throw std::runtime_error("File not open");
    }
    std::cout << "Input File " << inputFile->GetName() << std::endl;

    ERepSim::Input::Get().Attach(inputFile.get());
    int totalEntries = ERepSim::Input::Get().DataTree->GetEntries();

    for (int entry = 0; entry < totalEntries; ++entry) {
        ERepSim::Input::Get().GetEntry(entry);
        std::cout << "event " << ERepSim::Input::Get().RunId
                  << " " << ERepSim::Input::Get().EventId << std::endl;
        for (auto m : (*ERepSim::Input::Get().Property)) {
            std::cout << m.first << ": " << m.second << std::endl;
        }
        for (auto m : (*ERepSim::Input::Get().PropertyString)) {
            std::cout << m.first << ": " << m.second << std::endl;
        }
        for (std::size_t i=0;
             i < ERepSim::Input::Get().HitSensorId->size();
             ++i) {
            int id = (*ERepSim::Input::Get().HitSensorId)[i];
            int idp = id & 0x000001FF;
            int idb = (id>>9) & 0x000001FF;
            int idc = (id>>18) & 0x000001FF;
            int id0 = (id>>27);
            std::cout << "   "
                      << " " << (*ERepSim::Input::Get().HitSensorId)[i]
                      << " " << id0 << "-" << idc << "-" << idb << "-" << idp
                      << " (" << (*ERepSim::Input::Get().HitX)[i]
                      << "," << (*ERepSim::Input::Get().HitY)[i]
                      << "," << (*ERepSim::Input::Get().HitZ)[i]
                      << ") T " << (*ERepSim::Input::Get().HitTime)[i]
                      << " W " << (*ERepSim::Input::Get().HitTimeWidth)[i]
                      << " Q " << (*ERepSim::Input::Get().HitCharge)[i]
                      << std::endl;
            for (int j = (*ERepSim::Input::Get().HitSegmentBegin)[i];
                 j < (*ERepSim::Input::Get().HitSegmentEnd)[i];
                 ++j) {
                std::cout << "        S -> "
                          << " " << (*ERepSim::Input::Get().SegmentIds)[j]
                          << " " << (*ERepSim::Input::Get().SegmentEnergy)[j]
                          << " " << (*ERepSim::Input::Get().SegmentX1)[j]
                          << " " << (*ERepSim::Input::Get().SegmentY1)[j]
                          << " " << (*ERepSim::Input::Get().SegmentZ1)[j]
                          << " " << (*ERepSim::Input::Get().SegmentX2)[j]
                          << " " << (*ERepSim::Input::Get().SegmentY2)[j]
                          << " " << (*ERepSim::Input::Get().SegmentZ2)[j]
                          << " " << (*ERepSim::Input::Get().SegmentT)[j]
                          << std::endl;
            }
            // for (int j = (*ERepSim::Input::Get().HitContribBegin)[i];
            //      j < (*ERepSim::Input::Get().HitContribEnd)[i];
            //      ++j) {
            //     std::cout << "        C -> "
            //               << " " << (*ERepSim::Input::Get().ContribIds)[j]
            //               << " " << (*ERepSim::Input::Get().ContribPDG)[j]
            //               << " " << (*ERepSim::Input::Get().ContribMomentum)[j]
            //               << std::endl;
            // }
        }
    }

}
