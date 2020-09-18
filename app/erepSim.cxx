#include <TG4Event.h>
#include "ERepSimOutput.hxx"
#include "ERepSimDetectorBase.hxx"
#include "ERepSimDetector3DST.hxx"

#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <memory>
#include <unistd.h>

int main(int argc, char **argv) {
    std::cout << "erepReadWriteTest: Hello World" << std::endl;
    int maxEntries = 1E+8; // Maximum to process.

    while (true) {
        int c = getopt(argc,argv,"n:");
        if (c<0) break;
        switch (c) {
        case 'n': {
            std::istringstream tmp(optarg);
            tmp >> maxEntries;
            break;
        }
        default: {
            std::cout << "Usage: " << std::endl;
            std::cout << "   "
                      << "-n <number>  : Process no more than"
                      << " <number> events."
                      << std::endl;
            exit(1);
        }
        }
    }

    if (argc <= optind) {
        throw std::runtime_error("Missing input file");
    }
    std::string inputName(argv[optind++]);
    std::cout << "Input Name " << inputName << std::endl;

    if (argc <= optind) {
        throw std::runtime_error("Missing output file");
    }
    std::string outputName(argv[optind++]);
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

    TGeoManager* geom
        = dynamic_cast<TGeoManager*>(inputFile->Get("EDepSimGeometry"));
    if (!geom) {
        std::runtime_error("No EDepSimGeometry");
    }

    std::vector<std::shared_ptr<ERepSim::DetectorBase>> detectors;

    detectors.push_back(
        std::shared_ptr<ERepSim::Detector3DST>(
            new ERepSim::Detector3DST));

    std::unique_ptr<TFile> outputFile(new TFile(outputName.c_str(),"recreate"));
    ERepSim::Output::Get().CreateTrees();

#define SAVE_GEOMETRY
#ifdef SAVE_GEOMETRY
    // Save the geometry into the output file
    gGeoManager->Write();
#endif

    for (auto detector: detectors) {detector->Initialize();}

    totalEntries = std::min(totalEntries,maxEntries);
    for (int entry = 0; entry < totalEntries; ++entry) {

        edepsimTree->GetEntry(entry);

        for (auto detector: detectors) {detector->Reset();}

        ERepSim::Output::Get().Reset(edepsimEvent);
        std::cout << "Process event " << ERepSim::Output::Get().RunId
                  << "/" << ERepSim::Output::Get().EventId<< std::endl;

        for (auto detector: detectors) {
            detector->Process(edepsimEvent);
        }

        ERepSim::Output::Get().Fill();
    }

    ERepSim::Output::Get().Write();
}
