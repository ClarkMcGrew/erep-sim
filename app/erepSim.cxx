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
#include <TPRegexp.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <memory>
#include <unistd.h>

/// The command is:
///
/// erep-sim -o output.root -n <count> -M <file>:<mean> -N <file>:<number>
void usage() {
    std::cout << "Usage: erep-sim -o output.root [options] [input.root]"
              << std::endl;
    std::cout << std::endl;
    std::cout << "   -o [output-file]   Set the output file" << std::endl;
    std::cout << "   -r [run-number]    Set the run number" << std::endl;
    std::cout << "   -n [event-count]   Events to generate" << std::endl;
    std::cout << "   -M [file]:{s}[mean]{,skip}{:volume}" << std::endl;
    std::cout << "          Draw a 'mean' number of events from the file."
              << std::endl;
    std::cout << "          If prefixed with 's', the use events sequentially,"
              << std::endl;
    std::cout << "          otherwise events are drawn randomly.  If drawn"
              << std::endl;
    std::cout << "          sequentially, the number of interactions to skip"
              << std::endl;
    std::cout << "          can be set.  Optionally, a regexp for volumes to"
              << std::endl;
    std::cout << "          be excluded can be set."
              << std::endl;
    std::cout << "   -N [file]:[number]{,skip}{:volume}" << std::endl;
    std::cout << "          Draw a fixed number of interactions from the file."
              << std::endl;
    std::cout << "          The number of interactions to skip can be set."
              << std::endl;
    std::cout << "          Optionally, a regexp for volumes to be excluded"
              << std::endl;
    std::cout << "          can be set."
              << std::endl;
    std::cout << std::endl;
    std::cout << "  Providing a separate input file is equivalent to"
              << "  '-N input.root:1'."
              << std::endl;
}

// Which files to get edep-sim events frm.
struct EventTree {
    EventTree() : eventCount(0), sequential(true),
                  firstEntry(0), lastEntry(0), treeEntry(0) {}
    std::string fileName;   // The file name.
    std::string excludeVolume; // A regular expression for a volume to exclude
    double      eventCount; // Events to draw from the file
    bool        sequential; // Use events sequentially
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
            EventTree et;
            std::string file(optarg);
            std::string count(file.substr(file.find(':')+1));
            file = file.substr(0,file.find(':'));
            et.fileName = file;
            et.sequential = false; // set the default.
            if (count[0] == 'r' || count[0] == 'R') {
                et.sequential = false;
                count = count.substr(1);
            }
            if (count[0] == 's' || count[0] == 'S') {
                et.sequential = true;
                count = count.substr(1);
            }
            {
                // positive says random (this is the mean).
                std::istringstream tmp(count);
                double eCount = 1.0;
                tmp >> eCount;
                et.eventCount = eCount;
            }
            et.treeEntry = 0;
            {
                std::size_t skipIndex = count.find(',');
                if (skipIndex != std::string::npos) {
                    count = count.substr(skipIndex+1);
                    std::istringstream tmp(count);
                    int sCount = 0;
                    tmp >> sCount;
                    et.treeEntry = sCount;
                }
            }
            // Check if there is an excluded volume expression
            et.excludeVolume = "";
            {
                std::size_t exclusionIndex = count.find(':');
                if (exclusionIndex != std::string::npos) {
                    et.excludeVolume = count.substr(exclusionIndex+1);
                }
            }
            inputFiles.push_back(et);
            break;
        }
        case 'n': {
            std::istringstream tmp(optarg);
            tmp >> maxEvents;
            break;
        }
        case 'N': {
            EventTree et;
            std::string file(optarg);
            std::string count(file.substr(file.find(':')+1));
            file = file.substr(0,file.find(':'));
            et.fileName = file;
            et.sequential = true;
            {
                std::istringstream tmp(count);
                // Negative says fixed (this is the value).
                int eCount = 1;
                tmp >> eCount;
                et.eventCount = -eCount;
            }
            et.treeEntry = 0;
            {
                std::size_t skipIndex = count.find(',');
                if (skipIndex != std::string::npos) {
                    count = count.substr(skipIndex+1);
                    std::istringstream tmp(count);
                    int sCount = 0;
                    tmp >> sCount;
                    et.treeEntry = sCount;
                }
            }
            et.excludeVolume = "";
            {
                std::size_t exclusionIndex = count.find(':');
                if (exclusionIndex != std::string::npos) {
                    et.excludeVolume = count.substr(exclusionIndex+1);
                }
            }
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
            usage();
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
            std::cout << " Count: " << -et.eventCount << " interactions";
        }
        else {
            std::cout << " Mean: " << et.eventCount << " interactions";
        }
        if (et.sequential) {
            std::cout << " sequential from entry "
                      << et.treeEntry;
        }
        else {
            std::cout << " randomized entry order";
        }
        std::cout << std::endl;
        et.firstEntry = totalEntries;
        et.treeEntry = et.firstEntry + et.treeEntry;
        edepsimChain->Add(et.fileName.c_str());
        totalEntries = edepsimChain->GetEntries();
        et.lastEntry = totalEntries;
        std::cout << "    -- ";
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
        if (!et.excludeVolume.empty()) {
            std::cout << "excluding volume: " << et.excludeVolume;
        }
        std::cout << std::endl;
    }

    static TG4Event* edepsimEvent = NULL;
    edepsimChain->SetBranchAddress("Event", &edepsimEvent);

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
        bool eventOK = true;
        for (EventTree& et : inputFiles) {
            selectedEntries.clear();
            possibleEntries.clear();
            int events = -1;
            // Check if we are using a fixed number of entries.
            if (et.eventCount < 0) {
                events = -et.eventCount;
                std::cout << "ERepSim:: file: " << et.fileName;
                std::cout << std::endl;
                std::cout << "     --" << " Use: " << events;
                if (et.sequential) {
                    std::cout << " drawn sequentially starting at"
                              << " entry " << et.treeEntry - et.firstEntry;
                }
                else {
                    std::cout << " drawn randomly";
                }
                std::cout << std::endl;
            }
            else {
                events = gRandom->Poisson(et.eventCount);
                std::cout << "ERepSim:: file: " << et.fileName;
                std::cout << std::endl;
                std::cout << "     --" << " Mean: " << et.eventCount
                          << " Generate: " << events;
                if (et.sequential) {
                    std::cout << " drawn sequentially starting at"
                              << " entry " << et.treeEntry - et.firstEntry;
                }
                else {
                    std::cout << " drawn randomly";
                }
                std::cout << std::endl;
            }
            if (!et.excludeVolume.empty()) {
                std::cout << "     -- Excluding volume "
                          << et.excludeVolume
                          << std::endl;
            }
            if (et.sequential) {
                for (int i = 0; i < events; ++i) {
                    selectedEntries.push_back(et.treeEntry++);
                }
            }
            else {
                // Randomize the order
                possibleEntries.clear();
                randomEntryVector(possibleEntries,events,
                                  et.lastEntry-et.firstEntry);
                for (int entry : possibleEntries) {
                    et.treeEntry = entry+et.firstEntry;
                    selectedEntries.push_back(et.treeEntry);
                }
            }

            // Apply a sanity check to see if the event should be generated.
            if (et.treeEntry > et.lastEntry) {
                std::cout << "ERepSim:: Ran out of events in "
                          << et.fileName
                          << std::endl;
                eventOK = false;
                break;
            }

            // The selected entries should already be sorted, but check anyway.
            std::sort(selectedEntries.begin(), selectedEntries.end());

            // Accumulate the input events for this file
            int index = -1;
            for (const int entry : selectedEntries) {
                ++index;
                ///////////////////////////////////////
                // Get the next entry to be added to the event.
                if (edepsimChain->GetEntry(entry) < 1) {
                    std::cout << "Error getting entry " << entry
                              << std::endl;
                    throw std::runtime_error("Error getting an input event");
                }
#ifdef LOUD_AND_PROUD
                std::cout << "ERepSim:: Generate Entry: " << entry
                          << " " << index
                          << " Input event: "
                          << edepsimEvent->RunId << "/" << edepsimEvent->EventId
                          << std::endl;
#endif

                ///////////////////////////////////////
                // Check if the entry should be excluded.
                bool excludeEntry = false;

                // Find the volume containing the first primary particle.
                if (!et.excludeVolume.empty()) {
                    std::string volumeName;
                    for (TG4Trajectory& traj : edepsimEvent->Trajectories) {
                        if (traj.GetParentId() >= 0) continue;
                        if (traj.Points.empty()) continue;
                        TVector3 pnt = traj.Points.front().GetPosition().Vect();
                        TGeoNode* node
                            = gGeoManager->FindNode(pnt.X(),pnt.Y(),pnt.Z());
                        if (!node) continue;
                        volumeName = gGeoManager->GetPath();
                        break;
                    }
                    // See if the volume should be excluded.
                    std::unique_ptr<TPRegexp> exclusion(
                        new TPRegexp(et.excludeVolume.c_str()));
                    if (exclusion->Match(volumeName)>0) {
                        std::cout << "EXCLUDING " << volumeName
                                  << std::endl;
                        excludeEntry = true;
                    }
                }

                if (excludeEntry) continue;

                ///////////////////////////////////////
                // Add the entry to the erepSim event.
                ERepSim::Output::Get().Update(edepsimEvent);
                for (std::shared_ptr<ERepSim::DetectorBase>& detector
                         : gDetectors) {
                    detector->Accumulate(entry, edepsimEvent);
                }
            }
        }

        if (!eventOK) break;

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
