#include <TG4Event.h>
#include "ERepSimOutput.hxx"
#include "ERepSimDetectorBase.hxx"
#include "ERepSimDetectorECal.hxx"
#include "ERepSimDetector3DST.hxx"
#include "ERepSimDetectorTPC.hxx"
#include "ERepSimDefs.hxx"

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TGeoManager.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <memory>
#include <unistd.h>

int main(int argc, char **argv) {
    std::string outputName;
    int maxEvents = 1E+8; // Maximum to process.
    int combineEntries = 1; // Number of input entries per output event.
    int runId = 0;

    while (true) {
        int c = getopt(argc,argv,"c:n:o:r:");
        if (c<0) break;
        switch (c) {
        case 'c': {
            std::istringstream tmp(optarg);
            tmp >> combineEntries;
            break;
        }
        case 'n': {
            std::istringstream tmp(optarg);
            tmp >> maxEvents;
            break;
        }
        case 'o': {
            outputName = optarg;
            break;
        }
        case 'r': {
            std::istringstream tmp(optarg);
            tmp >> runId;
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
    if (combineEntries < 1) combineEntries = 1;

    if (argc <= optind) {
        throw std::runtime_error("Missing input file");
    }
    std::vector<std::string> inputNames;
    while (optind < argc) {
        inputNames.push_back(argv[optind++]);
        std::cout << "Input Name " << inputNames.back() << std::endl;
    }


    if (outputName.empty()) {
        std::cout << "Set the output file name using the '-o' option"
                  << std::endl;
        throw std::runtime_error("Must have an output file");
    }
    std::cout << "Output Name " << outputName << std::endl;

    // Attach to the input tree.
    std::unique_ptr<TFile> inputFile(
        new TFile(inputNames.front().c_str(),"old"));
    if (!inputFile->IsOpen()) throw std::runtime_error("Input file not open");

    /// Build the input chain.
    std::unique_ptr<TChain> edepsimChain(new TChain("EDepSimEvents"));
    for (std::size_t i = 0; i<inputNames.size(); ++i) {
        std::cout << "Add EDepSim file " << inputNames[i] << std::endl;
        edepsimChain->Add(inputNames[i].c_str());
    }

    static TG4Event* edepsimEvent = NULL;
    edepsimChain->SetBranchAddress("Event",&edepsimEvent);
    int totalEntries = edepsimChain->GetEntries();

    TGeoManager* geom
        = dynamic_cast<TGeoManager*>(inputFile->Get("EDepSimGeometry"));
    if (!geom) {
        std::runtime_error("No EDepSimGeometry");
    }

    // Check to see if the ECal tree in in the input file.
    TChain* ecalDigitChain = NULL;
    try {
        TTree* ecalDigitTree = dynamic_cast<TTree*>(inputFile->Get("tDigit"));
        if (!ecalDigitTree) {
            std::runtime_error("File does not have ECal hits");
        }
        /// Build the input chain.
        ecalDigitChain = new TChain("tDigit");
        for (std::size_t i = 0; i<inputNames.size(); ++i) {
            std::cout << "Add ECal file " << inputNames[i] << std::endl;
            ecalDigitChain->Add(inputNames[i].c_str());
        }
        std::cout << "File has the ECal electronics simulation"
                  << std::endl;
    }
    catch (...) {
        std::cout << "File does not have the ECal electronics simulation"
                  << std::endl;
    }

    std::vector<std::shared_ptr<ERepSim::DetectorBase>> detectors;

#define PROCESS_ECAL
#ifdef PROCESS_ECAL
    if (ecalDigitChain) {
        std::cout << "ERepSim:: Create ECal." << std::endl;
        detectors.push_back(
            std::shared_ptr<ERepSim::DetectorECal>(
                new ERepSim::DetectorECal(ecalDigitChain)));
    }
    else {
        std::cout << "ERepSim:: Invalid ECal digit chain." << std::endl;
    }
#endif

#define PROCESS_3DST
#ifdef PROCESS_3DST
    detectors.push_back(
        std::shared_ptr<ERepSim::Detector3DST>(
            new ERepSim::Detector3DST));
#endif

#define PROCESS_TPC
#ifdef PROCESS_TPC
    detectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("TPC",
                                     "voltpc",
                                     "volTpc_PV_0",
                                     ERepSim::Def::Detector::kTPC)));
    detectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("TopTPC",
                                     "voltpcTop",
                                     "volTpcTop_PV_0",
                                     ERepSim::Def::Detector::kTopTPC)));
    detectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("BotTPC",
                                     "voltpcBot",
                                     "volTpcBot_PV_0",
                                     ERepSim::Def::Detector::kBotTPC)));
#endif

    std::unique_ptr<TFile> outputFile(new TFile(outputName.c_str(),"recreate"));
    ERepSim::Output::Get().CreateTrees();

    // Save the geometry into the output file
    gGeoManager->Write();

    // Initialize all of the detectors.
    for (auto detector: detectors) {detector->Initialize();}

    // Build the output events.
    int entry = 0;
    for (int erepEvent = 0;
         erepEvent < maxEvents && entry < totalEntries;
         ++erepEvent) {
        std::cout << "ERepSim::  Event " << erepEvent
                  << " starting from entry " << entry
                  << std::endl;
        // Reset the detectors for the next event.
        ERepSim::Output::Get().Reset(runId,erepEvent);
        for (auto detector: detectors) {detector->Reset();}

        // Add input events to build the full ERepSim event.
        for (int subEntry = 0; subEntry < combineEntries; ++subEntry) {
            if (edepsimChain->GetEntry(entry++) < 1) {
                std::cout << "Ran out of input events"
                          << std::endl;
                break;
            }
            std::cout << "ERepSim:: Handle input event "
                      << edepsimEvent->RunId << "/" << edepsimEvent->EventId
                      << std::endl;
            ERepSim::Output::Get().Update(edepsimEvent);
            for (auto detector: detectors) {
                detector->Accumulate(entry, edepsimEvent);
            }
        }

        // Apply the sensors and DAQ
        for (auto detector: detectors) {
            detector->Process(erepEvent);
        }

        ERepSim::Output::Get().Fill();
    }

    ERepSim::Output::Get().Write();
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
