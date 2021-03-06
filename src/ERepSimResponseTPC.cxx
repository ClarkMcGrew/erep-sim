#include "ERepSimResponseTPC.hxx"
#include "ERepSimOutput.hxx"
#include "ERepSimUnits.hxx"
#include "ERepSimSegmentIdManager.hxx"

#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoNode.h>
#include <TRandom.h>

#include <iostream>

ERepSim::ResponseTPC::ResponseTPC(const char* modelName,
                                  const char* volumeName,
                                  int id)
    : ERepSim::ResponseBase(modelName),
      fVolumeName(volumeName), fTpcId(id) {}

ERepSim::ResponseTPC::~ResponseTPC() {}

bool ERepSim::ResponseTPC::RecurseGeometry() {
    TGeoNode* node = gGeoManager->GetCurrentNode();

    std::string name = node->GetName();

    double local[3] = {0,0,0};
    double master[3];
    gGeoManager->LocalToMaster(local,master);

    // Short circuit the 3DST.
    if (name.find("3DSTBar") != std::string::npos) return false;
    if (name.find("3DSTPlane") != std::string::npos) return false;
    if (name.find("3DSTCube") != std::string::npos) return false;

    if (name.find(fVolumeName) != std::string::npos) {

        const TGeoVolume* vol = node->GetVolume();
        const TGeoBBox* bbox = dynamic_cast<TGeoBBox*>(vol->GetShape());

        fXmin = master[0] - bbox->GetDX();
        fXmax = master[0] + bbox->GetDX();
        fYmin = master[1] - bbox->GetDY();
        fYmax = master[1] + bbox->GetDY();
        fZmin = master[2] - bbox->GetDZ();
        fZmax = master[2] + bbox->GetDZ();
        return true;
    }

    // Handle the daughters
    for (int i=0; i<node->GetNdaughters(); ++i) {
        gGeoManager->CdDown(i);
        RecurseGeometry();
        gGeoManager->CdUp();
    }

    return true;
}

void ERepSim::ResponseTPC::Initialize() {
    std::cout << "ResponseTPC::Initialize" << std::endl;

    //10mm default pad size
    fPadSizeY = 10.0*unit::mm;
    fPadSizeZ = 10.0*unit::mm;

    fStepSize = 1.0*unit::mm;
    fWI = 23.6*unit::eV;
    fGainMM = 1000.0; //MM gain

    fXmin = 0.;
    fXmax = 0.;
    fYmin = 0.;
    fYmax = 0.;
    fZmin = 0.;
    fZmax = 0.;

    gGeoManager->CdTop();
    RecurseGeometry();

    std::cout << fXmin << " < X < " << fXmax << std::endl;
    std::cout << fYmin << " < Y < " << fYmax << std::endl;
    std::cout << fZmin << " < Z < " << fZmax << std::endl;

    fCathodeX = (fXmin + fXmax)/2;
    fNbPadsY = ceil((fYmax - fYmin)/fPadSizeY);
    fNbPadsZ = ceil((fZmax - fZmin)/fPadSizeZ);

    ERepSim::Output::Get().Property[fModelName+".Response.StepSize"]
        = fStepSize;
    ERepSim::Output::Get().Property[fModelName+".Response.WI"] = fWI;
    ERepSim::Output::Get().Property[fModelName+".Response.GainMM"] = fGainMM;

}

void ERepSim::ResponseTPC::Reset() {
    ERepSim::ResponseBase::Reset();
}

void ERepSim::ResponseTPC::Process(const TG4Event* event,
                                   const TG4HitSegmentContainer& segments) {
#ifdef LOUD_AND_PROUD
    std::cout << "ResponseTPC::Process " << segments.size() << " segments"
              << " with " << CountCarriers() << " existing carriers"
              << std::endl;
#endif

    for (std::size_t i = 0; i<segments.size(); ++i) {
        const TG4HitSegment& seg = segments[i];
        ERepSim::SegmentIdManager manager;
        int segId = manager.GetNextSegmentIdentifier();
        AddTrack(event,seg,segId);
    }

#ifdef LOUD_AND_PROUD
    std::cout << "ResponseTPC::Process " << CountCarriers()
              << " carriers generated" << std::endl;
#endif
}

void ERepSim::ResponseTPC::GenerateElectrons(
    const TG4Event* event, const TG4HitSegment& seg, int segId,
    TLorentzVector generationPoint, double segLength) {

    if (generationPoint.Y() < fYmin
        || generationPoint.Y() > fYmax
        || generationPoint.Z() < fZmin
        || generationPoint.Z() > fZmax) {
        //To handle some a few border cases due to rounding errors I think
        return;
    }

    int plane;

    double cathodeThickness = 1.0*unit::mm;
    if (generationPoint.X() < fCathodeX - 0.5*cathodeThickness) {
        plane = 0;
    }
    else if (generationPoint.X() > fCathodeX + 0.5*cathodeThickness) {
        plane = 1;
    }
    else {
        return;
    }

    int pad_i = std::floor((generationPoint.Y() - fYmin)/fPadSizeY);
    int pad_j = std::floor((generationPoint.Z() - fZmin)/fPadSizeZ);

    int sensId = GetSensorId(plane, pad_i, pad_j);

    double totalLength = Length(seg.GetStop(), seg.GetStart());

    double energy = seg.EnergyDeposit*segLength/totalLength;
    double nbElectrons = energy/fWI;

    DriftElectrons(event, seg, segId, sensId, nbElectrons, generationPoint);
}

void ERepSim::ResponseTPC::DriftElectrons(const TG4Event* event,
                                          const TG4HitSegment& seg, int segId,
                                          int sensId,
                                          double nbElectrons,
                                          TLorentzVector generationPoint) {
    // Transversal spread is neglected compared to charge_spreading

    double avg_t = generationPoint.T();
    double anodeDist = (generationPoint.X() < fCathodeX)
        ? generationPoint.X() - fXmin : fXmax - generationPoint.X();

    avg_t += GetDriftTime(anodeDist);
    double sigma_t = GetSigmaDriftTime(anodeDist);

    MMAmplification(event, seg, segId, sensId, nbElectrons,
                    avg_t, sigma_t, generationPoint);
}

void ERepSim::ResponseTPC::MMAmplification(const TG4Event* event,
                                           const TG4HitSegment& seg, int segId,
                                           int sensId, double nbElectrons,
                                           double avg_t, double sigma_t,
                                           TLorentzVector generationPoint) {
    nbElectrons *= fGainMM; //Supposing uniform amplification
    SpreadCharge(event, seg, segId, sensId, nbElectrons,
                 avg_t, sigma_t, generationPoint);
}

void ERepSim::ResponseTPC::SpreadCharge(const TG4Event* event,
                                        const TG4HitSegment& seg, int segId,
                                        int sensId, double nbElectrons,
                                        double avg_t, double sigma_t,
                                        TLorentzVector generationPoint) {
    //Very very basic charge spreading for now, not taking into account the
    //generationPoint but only applying hits on neighbouring pads with
    //relative energy coefficients coming out of nowhere

    int plane, pad_y, pad_z;
    GetPadInfo(sensId, plane, pad_y, pad_z);

    // For now, very basic charge spreading of 1 pad in 4 directions and
    // instantaneous charge spreading, to be improved
    std::vector<std::pair<int, int>> coord_change;
    coord_change.push_back(std::make_pair(0, 0));
#ifdef APPLY_SPREAD
    coord_change.push_back(std::make_pair(0, 1));
    coord_change.push_back(std::make_pair(0, -1));
    coord_change.push_back(std::make_pair(1, 0));
    coord_change.push_back(std::make_pair(-1, 0));
#endif

    for (std::vector<std::pair<int, int>>::iterator xy = coord_change.begin();
         xy != coord_change.end(); ++xy) {
        if (pad_y + xy->first < 0
           || pad_y + xy->first >= fNbPadsY
           || pad_z + xy->second < 0
           || pad_z + xy->second > fNbPadsZ) {
            continue;
        }
        int curSensId = GetSensorId(plane, pad_y + xy->first,
                                    pad_z + xy->second);

        //Random coeff values for charge spreading intensity, to be improved
        double spreadCoeff = (xy->first == 0 && xy->second == 0) ? 0.5 : 0.125;

        AddHit(event, seg, segId, curSensId,
               nbElectrons*spreadCoeff,
               avg_t, sigma_t, generationPoint);
    }
}

void ERepSim::ResponseTPC::AddHit(const TG4Event* event,
                                  const TG4HitSegment& seg, int segId,
                                  int sensId, double nbElectrons,
                                  double avg_t, double sigma_t,
                                  TLorentzVector generationPoint) {

    int plane, pad_y, pad_z;
    GetPadInfo(sensId, plane, pad_y, pad_z);

    double finalPos[4];
    finalPos[0] = (plane == 0) ? fXmin : fXmax;
    finalPos[1] = (0.5 + pad_y)*fPadSizeY + fYmin;
    finalPos[2] = (0.5 + pad_z)*fPadSizeZ + fZmin;
    finalPos[3] = avg_t;
    std::shared_ptr<ERepSim::Carrier> carrier(
        new ERepSim::Carrier(event,&seg,sensId,segId));
    carrier->SetInitialPosition(generationPoint.X(),
                                generationPoint.Y(),
                                generationPoint.Z(),
                                generationPoint.T());
    carrier->SetFinalPosition(finalPos[0],
                              finalPos[1],
                              finalPos[2],
                              finalPos[3]);
    carrier->SetPositionSigma(0, 0, 0, sigma_t);
    carrier->SetCharge(nbElectrons);
    (*fCarriers)[sensId].push_back(carrier);
}

void ERepSim::ResponseTPC::AddTrack(const TG4Event* event,
                                    const TG4HitSegment& seg, int segId) {
    const TLorentzVector& start = seg.GetStart();
    const TLorentzVector& stop = seg.GetStop();

    double totalLength = Length(stop,start);
    double targetStep = 1.0*unit::mm;
    int totalSteps = totalLength/targetStep;
    double stepLength = totalLength/totalSteps;
    if (totalSteps < 2) stepLength = totalLength;
    for (double dL = stepLength/2.0; dL < totalLength; dL += stepLength) {
        double t = dL/totalLength;
        TLorentzVector generationPoint = start + (stop-start)*t;
        double spreadSteps = 10.0;
        double spreadSigma = 3.0*unit::mm;
        for (int i = 0; i < spreadSteps; ++i) {
            TLorentzVector spreadPoint(generationPoint);
            spreadPoint[1] += spreadSigma*gRandom->Gaus();
            spreadPoint[2] += spreadSigma*gRandom->Gaus();
            GenerateElectrons(event, seg, segId,
                              spreadPoint, stepLength/spreadSteps);
        }
    }
}

int ERepSim::ResponseTPC::GetSensorId(int plane, int pad_y, int pad_z) {
    int id = fTpcId;
    id = plane + (id << 3);
    id = pad_y + (id << 12);
    id = pad_z + (id << 12);
    return id;
}

void ERepSim::ResponseTPC::GetPadInfo(int sensId,
                                      int& plane,
                                      int& pad_y,
                                      int& pad_z) {
    pad_z = sensId & ((1 << 12) - 1);
    pad_y = (sensId >> 12) & ((1 << 12) - 1);
    plane = (sensId >> 24) & ((1 << 3) - 1);
}

double ERepSim::ResponseTPC::GetDriftTime(double dist) {
    const double vdrift = 0.078*unit::mm/unit::ns; //mm/ns
    return dist/vdrift;
}

double ERepSim::ResponseTPC::GetSigmaDriftTime(double dist) {
    const double vdrift = 0.078*unit::mm/unit::ns; // mm/ns
    const double sigmaL = 0.29*unit::mm/std::sqrt(unit::cm); // mm/sqrt(cm)
    return std::sqrt(dist)*sigmaL/vdrift;
}

double ERepSim::ResponseTPC::GetLongitudinalSpread(double dist) {
    const double sigmaL = 0.237; //mm/sqrt(cm)
    return std::sqrt(dist)*sigmaL;
}

double ERepSim::ResponseTPC::Length(const TLorentzVector& A,
                                    const TLorentzVector& B) {
    return (A.Vect()-B.Vect()).Mag();
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
