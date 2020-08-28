#include <TG4Event.h>
#include "ERepSimOutput.hxx"

#include <TFile.h>

#include <iostream>
#include <exception>
#include <memory>

int main(int argc, char **argv) {
    std::cout << "erepReadWriteTest: Hello World" << std::endl;

    if (argc < 2) {
        throw std::runtime_error("Missing input file");
    }
    std::string inputName(argv[1]);
    std::cout << "Input Name " << inputName << std::endl;

    if (argc < 3) {
        throw std::runtime_error("Missing output file");
    }
    std::string outputName(argv[2]);
    std::cout << "Output Name " << outputName << std::endl;

    std::unique_ptr<TFile> inputFile(new TFile(inputName.c_str(),"old"));
    if (!inputFile->IsOpen()) {
        throw std::runtime_error("File not open");
    }
    std::cout << "Input File " << inputFile->GetName() << std::endl;

    TTree* edepsimTree = dynamic_cast<TTree*>(inputFile->Get("EDepSimEvents"));
    if (!edepsimTree) {
        std::runtime_error("No EDepSimEvents tree");
    }

    static TG4Event* edepsimEvent = NULL;
    edepsimTree->SetBranchAddress("Event",&edepsimEvent);
    int totalEntries = edepsimTree->GetEntries();

    std::unique_ptr<TFile> outputFile(new TFile(outputName.c_str(),"recreate"));

    ERepSim::Output::Get().CreateTrees();

    ERepSim::Output::Get().Property["3DST.Atten1"] = 1.0;
    ERepSim::Output::Get().Property["3DST.Atten1.Fraction"] = 1.0;
    ERepSim::Output::Get().Property["3DST.Atten2"] = 1.0;

    for (int entry = 0; entry < totalEntries; ++entry) {
        ERepSim::Output::Get().Reset();
        edepsimTree->GetEntry(entry);
        ERepSim::Output::Get().RunId = edepsimEvent->RunId;
        ERepSim::Output::Get().EventId = edepsimEvent->EventId;
        for (int i=0; i<10; ++i) {
            ERepSim::Output::Get().HitSensorId.push_back(i+1);
            ERepSim::Output::Get().HitX.push_back(i+1);
            ERepSim::Output::Get().HitY.push_back(i+1);
            ERepSim::Output::Get().HitZ.push_back(i+1);
            ERepSim::Output::Get().HitTime.push_back(i+1);
            ERepSim::Output::Get().HitTimeWidth.push_back(1.0);
            ERepSim::Output::Get().HitCharge.push_back(i+1);
            ERepSim::Output::Get().HitSegmentBegin.push_back(
                ERepSim::Output::Get().SegmentIds.size());
            for (int j = 0; j<i; ++j) {
                ERepSim::Output::Get().SegmentIds.push_back(j+1);
            }
            ERepSim::Output::Get().HitSegmentEnd.push_back(
                ERepSim::Output::Get().SegmentIds.size());
        }
        std::cout << "event " << ERepSim::Output::Get().RunId
                  << " " << ERepSim::Output::Get().EventId<< std::endl;
        ERepSim::Output::Get().Fill();
    }

    ERepSim::Output::Get().Write();
}
