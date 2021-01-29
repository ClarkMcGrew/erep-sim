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
#include <TRandom.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <memory>
#include <unistd.h>

/// The command is:
///
/// erep-merge -o output.root -n <count> -M <file>:<mean> -N <file>:<number>
void usage() {
    std::cout << "usage: erep-merge -o output.root [options] [input.root]"
              << std::endl;
    std::cout << std::endl;
    std::cout << "   -o [output-file]   Set the output file" << std::endl;
    std::cout << "   -r [run-number]    Set the run number" << std::endl;
    std::cout << "   -n [event-count]   Events to generate" << std::endl;
    std::cout << "   -M [file]:[mean]   Draw 'mean' events from file"
              << std::endl;
    std::cout << "   -N [file]:[number] Draw exactly 'number' events from file"
              << std::endl;
    std::cout << std::endl;
    std::cout << "  If a separate input file is provided, it is equivalent to"
              << "  adding '-N input.root:1'."
              << std::endl;
}

// Which files to get edep-sim events frm.
struct EventTree {
    EventTree() : eventCount(0), firstEntry(0), lastEntry(0), treeEntry(0) {}
    std::string fileName;   // The file name.
    double      eventCount; // Events to draw from the file
    int         firstEntry; // First entry in the chain.
    int         lastEntry;  // firstEntry + total entries in chain
    int         treeEntry;  // The last entry used.
};

// Fill a vector of random entries to be used in an event.  This has
// protection for when there aren't enough entries in the input file.
void randomEntryVector(std::vector<int>& entries, int choose,
                       int totalEntries) {
    entries.clear();
    ///////////////////////////////////////////////////////
    // Not enough entries, so return them all.
    if (totalEntries < choose) {
        for (int i=0; i<totalEntries; ++i) {
            entries.push_back(i);
        }
        return;
    }
    ///////////////////////////////////////////////////////
    // Only a few entries, so use an elimination method.
    if (totalEntries < 5*choose) {
        // Add all possible entries
        for (int i=0; i<totalEntries; ++i) {
            entries.push_back(i);
        }
        // Eliminate entries until we have few enough.
        while (entries.size() > (std::size_t) choose) {
            int eliminate = entries.size() - choose;
            for (int i = 0; i<eliminate; ++i) {
                int entry = gRandom->Uniform(0,entries.size());
                // Mark an entry to be dropped with the vector size.  This
                // will sort to be the last element in the vector.
                entries[entry] = entries.size();
            }
            std::sort(entries.begin(), entries.end());
            std::vector<int>::iterator e
                = std::unique(entries.begin(),entries.end());
            // Drop an extra entry since the "drop" flag will be the one
            // before e.
            entries.erase(e-1,entries.end());
        }
        return;
    }
    ///////////////////////////////////////////////////////
    // Lots of entries, so add entries until we have enough.
    int remaining = choose;
    while (remaining > 0) {
        for (int i=0; i<remaining; ++i) {
            int entry = gRandom->Uniform(0,totalEntries);
            entries.push_back(entry);
        }
        std::sort(entries.begin(), entries.end());
        std::vector<int>::iterator e
            = std::unique(entries.begin(),entries.end());
        entries.erase(e,entries.end());
        remaining = choose - entries.size();
    }
}

int main(int argc, char **argv) {
    std::string outputName;
    int runId = 0;
    // Maximum number of erep-sim events to generate.
    int maxEvents = 1E+8;
    // All of the inputs.
    std::vector<EventTree> inputFiles;
    // If true, then don't use the ecal/stt kludges.
    int sanity = false;
    // The detectors that will be simulated during this run.
    std::vector<std::shared_ptr<ERepSim::DetectorBase>> gDetectors;


    while (true) {
        int c = getopt(argc,argv,"M:n:N:o:r:S");
        if (c<0) break;
        switch (c) {
        case 'M': {
            std::string file(optarg);
            std::string count(file.substr(file.find(':')+1));
            file = file.substr(0,file.find(':'));
            std::istringstream tmp(count);
            EventTree et;
            et.fileName = file;
            double eCount = 1.0;
            tmp >> eCount;
            et.eventCount = eCount; // positive says random (this is the mean).
            inputFiles.push_back(et);
            break;
        }
        case 'n': {
            std::istringstream tmp(optarg);
            tmp >> maxEvents;
            break;
        }
        case 'N': {
            std::string file(optarg);
            std::string count(file.substr(file.find(':')+1));
            file = file.substr(0,file.find(':'));
            std::istringstream tmp(count);
            EventTree et;
            et.fileName = file;
            int eCount = 1;
            tmp >> eCount;
            et.eventCount = -eCount; // negative says fixed (this is the value).
            inputFiles.push_back(et);
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
        case 'S': {
            // Impose sanity and don't use the ecal kludges.
            sanity = true;
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

    if (outputName.empty()) {
        std::cout << "Set the output file name using the '-o' option"
                  << std::endl;
        throw std::runtime_error("Must have an output file");
    }

    while (optind < argc) {
        EventTree et;
        et.fileName = argv[optind++];
        et.eventCount = -1;
        inputFiles.push_back(et);
    }

    // Attach to the first input file so we can get things like the geometry.
    std::unique_ptr<TFile>
        firstFile(new TFile(inputFiles.front().fileName.c_str(),"old"));
    if (!firstFile->IsOpen()) throw std::runtime_error("Input file not open");

    // Get the geometry.
    if (!dynamic_cast<TGeoManager*>(firstFile->Get("EDepSimGeometry"))) {
        std::runtime_error("No EDepSimGeometry");
    }

    // Look for the ecal kludge
    std::unique_ptr<TChain> ecalKludgeChain;
    if (!sanity && dynamic_cast<TTree*>(firstFile->Get("tDigit"))) {
        ecalKludgeChain.reset(new TChain("tDigit"));
    }

    // Build the monster chain.
    std::unique_ptr<TChain> edepsimChain(new TChain("EDepSimEvents"));
    int totalEntries = 0;
    for (EventTree& et : inputFiles) {
        std::cout << "Input Name: " << et.fileName << std::endl;
        std::cout << "    -- ";
        if (et.eventCount < 0) {
            std::cout << " Count: " << -et.eventCount;
        }
        else {
            std::cout << " Mean: " << et.eventCount;
        }
        et.firstEntry = totalEntries;
        et.treeEntry = et.firstEntry;
        edepsimChain->Add(et.fileName.c_str());
        totalEntries = edepsimChain->GetEntries();
        et.lastEntry = totalEntries;
        std::cout << " Entries: [" << et.firstEntry
                  << " - " << et.lastEntry << ")";
        if (ecalKludgeChain) {
            ecalKludgeChain->Add(et.fileName.c_str());
            int ecalEntries = ecalKludgeChain->GetEntries();
            std::cout << " w/ ECal ";
            if (ecalEntries != totalEntries) {
                throw std::runtime_error("File doesn't support ecal kludge");
            }
        }
        std::cout << std::endl;
    }

    static TG4Event* edepsimEvent = NULL;
    edepsimChain->SetBranchAddress("Event",&edepsimEvent);

    if (ecalKludgeChain) {
        std::cout << "ERepSim:: Create ECal." << std::endl;
        gDetectors.push_back(
            std::shared_ptr<ERepSim::DetectorECal>(
                new ERepSim::DetectorECal(ecalKludgeChain.get())));
    }
    gDetectors.push_back(
        std::shared_ptr<ERepSim::Detector3DST>(
            new ERepSim::Detector3DST));
    gDetectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("TPC",
                                     "voltpc",
                                     "volTpc_PV_0",
                                     ERepSim::Def::Detector::kTPC)));
    gDetectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("TopTPC",
                                     "voltpcTop",
                                     "volTpcTop_PV_0",
                                     ERepSim::Def::Detector::kTopTPC)));
    gDetectors.push_back(
        std::shared_ptr<ERepSim::DetectorTPC>(
            new ERepSim::DetectorTPC("BotTPC",
                                     "voltpcBot",
                                     "volTpcBot_PV_0",
                                     ERepSim::Def::Detector::kBotTPC)));

    std::cout << "Output Name " << outputName << std::endl;
    std::unique_ptr<TFile> outputFile(new TFile(outputName.c_str(),"recreate"));
    ERepSim::Output::Get().CreateTrees();

    // Save the geometry into the output file
    gGeoManager->Write();

    // Initialize all of the detectors.
    for (std::shared_ptr<ERepSim::DetectorBase>& detector : gDetectors) {
        detector->Initialize();
    }

    // Make the events...
    std::vector<int> selectedEntries;
    std::vector<int> possibleEntries;
    for (int eventId = 0; eventId < maxEvents; ++eventId) {
        std::cout << "ERepSim::  Event " << runId << "/" << eventId
                  << std::endl;
        // Reset the detectors for the next event.
        ERepSim::Output::Get().Reset(runId,eventId);
        for (std::shared_ptr<ERepSim::DetectorBase>& detector : gDetectors) {
            detector->Reset();
        }

        // Select the entries to use from each input source.
        selectedEntries.clear();
        for (EventTree& et : inputFiles) {
            // Check if we are using a fixed number of entries.
            if (et.eventCount < 0) {
                std::cout << "ERepSim:: file: " << et.fileName;
                std::cout << std::endl;
                std::cout << "     --" << " Use: " << -et.eventCount
                          << " starting at " << et.treeEntry
                          << std::endl;
                for (int i = 0; i < -et.eventCount; ++i) {
                    selectedEntries.push_back(et.treeEntry++);
                }
                continue;
            }
            int events = gRandom->Poisson(et.eventCount);
            std::cout << "ERepSim:: file: " << et.fileName;
            std::cout << std::endl;
            std::cout << "     --" << " Mean: " << et.eventCount
                      << " Generate: " << events
                      << std::endl;
            possibleEntries.clear();
            randomEntryVector(possibleEntries,events,
                              et.lastEntry-et.firstEntry);
            for (int entry : possibleEntries) {
                selectedEntries.push_back(entry+et.firstEntry);
            }
        }

        // Apply a sanity check to see if the event should be generated.
        bool eventOK = true;
        for (EventTree& et : inputFiles) {
            if (et.treeEntry > et.lastEntry) {
                std::cout << "ERepSim:: Ran out of events in "
                          << et.fileName
                          << std::endl;
                eventOK = false;
            }
        }
        if (!eventOK) break;

        // The selected entries should already be sorted, but check anyway.
        std::sort(selectedEntries.begin(), selectedEntries.end());

        // Accumulate the input events.
        for (int entry : selectedEntries) {
            if (edepsimChain->GetEntry(entry) < 1) {
                std::cout << "Error getting entry " << entry
                          << std::endl;
                throw std::runtime_error("Error getting an input event");
            }
            std::cout << "ERepSim:: Entry: " << entry
                      << " Input event: "
                      << edepsimEvent->RunId << "/" << edepsimEvent->EventId
                      << std::endl;
            ERepSim::Output::Get().Update(edepsimEvent);
            for (std::shared_ptr<ERepSim::DetectorBase>& detector
                     : gDetectors) {
                detector->Accumulate(entry, edepsimEvent);
            }
        }

        // Apply the sensors and DAQ
        for (std::shared_ptr<ERepSim::DetectorBase>& detector
                 : gDetectors) {
            detector->Process(eventId);
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
