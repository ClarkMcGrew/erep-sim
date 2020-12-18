#include "ERepSimResponseTPC.hxx"
#include "ERepSimOutput.hxx"

#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoNode.h>
#include <TRandom.h>

#include <iostream>

ERepSim::ResponseTPC::ResponseTPC(ERepSim::TPC_id id)
    : ERepSim::ResponseBase("TPC"), fTpcId(id) {}

ERepSim::ResponseTPC::~ResponseTPC() {}

bool ERepSim::ResponseTPC::RecurseGeometry() {
    TGeoNode* node = gGeoManager->GetCurrentNode();

    std::string name = node->GetName();

    double local[3] = {0,0,0};
    double master[3];
    gGeoManager->LocalToMaster(local,master);

    if(name.find("vol3DST_PV_0") != std::string::npos){ //Skipping all the 3DST cubes
        return false;
    }

    const char* volTpc = ERepSim::TPCdef::volNames[fTpcId];
    std::cout << "TPC id : " << fTpcId << " name: " << volTpc << std::endl;

    if(name.find(volTpc) != std::string::npos){

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
    fPadSizeY = 10;
    fPadSizeZ = 10;

    fXmin = 0.;
    fXmax = 0.;
    fYmin = 0.;
    fYmax = 0.;
    fZmin = 0.;
    fZmax = 0.;

    gGeoManager->CdTop();
    RecurseGeometry();

    fCathodeX = (fXmin + fXmax)/2;
    fNbPadsY = ceil((fYmax - fYmin)/fPadSizeY);
    fNbPadsZ = ceil((fZmax - fZmin)/fPadSizeZ);

    // ERepSim::Output::Get().Property["TPC.Response.PhotPerMeV"] = fPhotPerMeV;
    // ERepSim::Output::Get().Property["TPC.Response.Velocity"] = fLightVelocity;

    // ERepSim::Output::Get().Property["TPC.Response.XLeakage"] = fXLeakage;
    // ERepSim::Output::Get().Property["TPC.Response.YLeakage"] = fYLeakage;
    // ERepSim::Output::Get().Property["TPC.Response.ZLeakage"] = fZLeakage;

    // // Describe the detector for posterity...
    // ERepSim::Output::Get().Property["TPC.Response.Cubes"] = fCubes;
    // ERepSim::Output::Get().Property["TPC.Response.CubeMin"] = fCubeMin;
    // ERepSim::Output::Get().Property["TPC.Response.CubeMax"] = fCubeMax;

    // ERepSim::Output::Get().Property["TPC.Response.Bars"] = fBars;
    // ERepSim::Output::Get().Property["TPC.Response.BarMin"] = fBarMin;
    // ERepSim::Output::Get().Property["TPC.Response.BarMax"] = fBarMax;

    // ERepSim::Output::Get().Property["TPC.Response.Planes"] = fPlanes;
    // ERepSim::Output::Get().Property["TPC.Response.PlaneMin"] = fPlaneMin;
    // ERepSim::Output::Get().Property["TPC.Response.PlaneMax"] = fPlaneMax;

    // Set the attenuation parameters.  The sensor distance is the distance
    // from the center of the "first" cube by the sensor.  This comes from
    // measurements of 1mm Kuraray fibers done at URochester in ~2010.
    // ERepSim::Output::Get().Property["TPC.Response.Atten.Ratio12"] = 0.75;
    // ERepSim::Output::Get().Property["TPC.Response.Atten.Tau1"] = 4600.0;
    // ERepSim::Output::Get().Property["TPC.Response.Atten.Tau2"] = 33.0;
    // ERepSim::Output::Get().Property["TPC.Response.Atten.SensorDist"] = 20.0;
    // ERepSim::Output::Get().Property["TPC.Response.Atten.Reflect"] = 0.0;
    // ERepSim::Output::Get().Property["TPC.Response.Atten.MirrorDist"] = 10.0;

    // ERepSim::Output::Get().Property["TPC.Response.Scint.Ratio12"] = 0.50;
    // ERepSim::Output::Get().Property["TPC.Response.Scint.Life1"] = 6.0;
    // ERepSim::Output::Get().Property["TPC.Response.Scint.Life2"] = 11.0;

}

void ERepSim::ResponseTPC::Reset() {
    ERepSim::ResponseBase::Reset();
}

void ERepSim::ResponseTPC::Process(const TG4HitSegmentContainer& segments) {
    std::cout << "ResponseTPC::Process " << segments.size() << " segments"
              << std::endl;

    for (std::size_t segId = 0; segId<segments.size(); ++segId) {
        const TG4HitSegment& seg = segments[segId];
        AddTrack(segId, seg);
    }

    // TLorentzVector avg;
    // for (std::size_t segId = 0; segId<segments.size(); ++segId) {
    //     const TG4HitSegment& segment = segments[segId];
    //     avg = 0.5*segment.GetStart() + 0.5*segment.GetStop();
    //     double deposit = segment.GetSecondaryDeposit();
    //     if (deposit < 1E-6) deposit = segment.GetEnergyDeposit();
    //     AddDeposit(segId,&segment,avg,deposit);
    // }

    std::cout << "ResponseTPC::Process " << CountCarriers()
              << " carriers generated" << std::endl;
}

void ERepSim::ResponseTPC::AddTrack(int segId, const TG4HitSegment& seg){
    const TLorentzVector& start = seg.GetStart();
    const TLorentzVector& stop = seg.GetStop();

    //Check if track crosses the cathode
    if((start.X() - fCathodeX)*(stop.X() - fCathodeX) < 0){
        //Split the track over the intersection point
        double t = (fCathodeX - start.X())/(stop.X() - start.X());
        TLorentzVector intersection = start + (stop - start)*t;
        intersection.SetX(fCathodeX);

        TG4HitSegment *seg1 = static_cast<TG4HitSegment*>(seg.Clone());
        TG4HitSegment *seg2 = static_cast<TG4HitSegment*>(seg.Clone());

        seg1->Stop = intersection;
        seg1->EnergyDeposit *= t;
        seg1->SecondaryDeposit *= t;
        seg1->TrackLength *= t;

        seg2->Start = intersection;
        seg2->EnergyDeposit *= (1 - t);
        seg2->SecondaryDeposit *= (1 - t);
        seg2->TrackLength *= (1 - t);

        AddTrack(segId, *seg1);
        AddTrack(segId, *seg2);

        return;
    }

    int plane;

    if(std::min(start.X(), stop.X()) < fCathodeX)
        plane = 0;
    else
        plane = 1;

    const std::vector<TLorentzVector> crosses = ComputeCrosses(start, stop);
    double total_length = sqrtf(powf(start.Y() - stop.Y(), 2) + powf(start.Z() - stop.Z(), 2));

    for(uint i = 0; i < crosses.size() - 1; i++){
        if(crosses[i] == crosses[i+1])//Dealing with borders
            continue;
        TLorentzVector middle_point = (crosses[i] + crosses[i + 1])*0.5;

        int pad_i = floor((middle_point.Y() - fYmin)/fPadSizeY);
        int pad_j = floor((middle_point.Z() - fZmin)/fPadSizeZ);

        double length_on_pad = sqrtf(powf(crosses[i].Y() - crosses[i + 1].Y(), 2) + powf(crosses[i].Z() - crosses[i + 1].Z(), 2));

        double energy_on_pad = seg.EnergyDeposit*length_on_pad/total_length;

        double avg_t = middle_point.T();
        double dist_to_anode = (plane == 0) ? middle_point.X() - fXmin : fXmax - middle_point.X();

        avg_t += GetDriftTime(dist_to_anode);
        double sigma_t = GetSigmaDriftTime(dist_to_anode);

        int sensId = GetSensorId(plane, pad_i, pad_j);

        // Transversal spread is neglected compared to charge_spreading

        AddHit(sensId, segId, seg, energy_on_pad, avg_t, sigma_t, middle_point);
    }
}

const std::vector<TLorentzVector> ERepSim::ResponseTPC::ComputeCrosses(const TLorentzVector A, const TLorentzVector B){
    std::vector<TLorentzVector> crosses;
    crosses.push_back(A);

    const TLorentzVector vdir = B - A;

    if(A.Y() != B.Y()){
        double y0 = std::min(A.Y(), B.Y());
        double y1 = std::max(A.Y(), B.Y());

        double yI0 = ceil((y0 - fYmin)/fPadSizeY)*fPadSizeY + fYmin;
        double yI1 = floor((y1 - fYmin)/fPadSizeY)*fPadSizeY + fYmin;

        for(double yI = yI0; yI < yI1 + fPadSizeY/2; yI += fPadSizeY){
            double tI = (yI - A.Y())/(B.Y() - A.Y());
            TLorentzVector pI = A + tI*vdir;
            crosses.push_back(pI);
        }
    }

    if(A.Z() != B.Z()){
        double z0 = std::min(A.Z(), B.Z());
        double z1 = std::max(A.Z(), B.Z());

        double zI0 = ceil((z0 - fZmin)/fPadSizeZ)*fPadSizeZ + fZmin;
        double zI1 = floor((z1 - fZmin)/fPadSizeZ)*fPadSizeZ + fZmin;

        for(double zI = zI0; zI < zI1 + fPadSizeZ/2; zI += fPadSizeZ){
            double tI = (zI - A.Z())/(B.Z() - A.Z());
            TLorentzVector pI = A + tI*vdir;
            crosses.push_back(pI);
        }
    }

    crosses.push_back(B);

    sort(crosses.begin(), crosses.end(),
        [A](const TLorentzVector &a, const TLorentzVector &b) -> bool{
            double dista = pow(A.Y() - a.Y(), 2) + pow(A.Z() - a.Z(), 2);
            double distb = pow(A.Y() - b.Y(), 2) + pow(A.Z() - b.Z(), 2);

            return dista > distb;
        });

    return crosses;
}

void ERepSim::ResponseTPC::AddHit(int sensId, int segId,
                                    const TG4HitSegment& seg,
                                    double energy,
                                    double avg_t,
                                    double sigma_t,
                                    TLorentzVector pos){

    const float MMgain = 1000;
    const float WI = 23.6; //eV
    const float eVtofC = 6250;

    int plane, pad_y, pad_z;
    GetPadInfo(sensId, plane, pad_y, pad_z);

    assert(pad_y >= 0);
    assert(pad_y <= fNbPadsY);
    assert(pad_z >= 0);
    assert(pad_z <= fNbPadsZ);

    if(pad_y == fNbPadsY || pad_z == fNbPadsZ)
        return;

    //For now, very basic charge spreading of 1 pad in 4 directions + instantaneous charge spreading, to be improved
    std::vector<std::pair<int, int>> coord_change;
    coord_change.push_back(std::make_pair(0, 0));
    coord_change.push_back(std::make_pair(0, 1));
    coord_change.push_back(std::make_pair(0, -1));
    coord_change.push_back(std::make_pair(1, 0));
    coord_change.push_back(std::make_pair(-1, 0));

    for(std::pair<int, int> xy : coord_change){
        if(pad_y + xy.first < 0 || pad_y + xy.first >= fNbPadsY || pad_z + xy.second < 0 || pad_z + xy.second > fNbPadsZ){
            continue;
        }
        int cur_pad_id = GetSensorId(plane, pad_y + xy.first, pad_z + xy.second);
        double energy_coeff = (xy.first == 0 && xy.second == 0) ? 0.5 : 0.125; //Random coeef values for charge spreading intensity, to be improved


        double finalPos[4];
        finalPos[0] = (plane == 0) ? fXmin : fXmax;
        finalPos[1] = (0.5 + pad_y)*fPadSizeY;
        finalPos[2] = (0.5 + pad_z)*fPadSizeZ;
        finalPos[3] = avg_t;
        std::shared_ptr<ERepSim::Carrier> carrier(
            new ERepSim::Carrier(cur_pad_id,segId,&seg));
        carrier->SetInitialPosition(pos.X(),pos.Y(),pos.Z(),pos.T());
        carrier->SetFinalPosition(finalPos[0], finalPos[1], finalPos[2], finalPos[3]);
        carrier->SetPositionSigma(0, 0, 0, sigma_t);
        carrier->SetCharge(energy_coeff*energy*MMgain*1E6/WI/eVtofC);
        (*fCarriers)[cur_pad_id].push_back(carrier);
    }
}

int ERepSim::ResponseTPC::GetSensorId(int plane, int pad_y, int pad_z) {
    int id = 25 + fTpcId;
    id = plane + (id << 3);
    id = pad_y + (id << 12);
    id = pad_z + (id << 12);
    return id;
}

void ERepSim::ResponseTPC::GetPadInfo(int sensId, int &plane, int& pad_y, int& pad_z){
    pad_z = sensId & ((1 << 12) - 1);
    pad_y = (sensId >> 12) & ((1 << 12) - 1);
    plane = (sensId >> 24) & ((1 << 3) - 1);
}

double ERepSim::ResponseTPC::GetDriftTime(double dist){
    const double vdrift = 0.078; //mm/ns
    return dist/vdrift;
}

double ERepSim::ResponseTPC::GetSigmaDriftTime(double dist){
    const double vdrift = 0.078; //mm/ns
    const double sigmaL = 0.29; //mm/sqrt(cm)
    return std::sqrt(dist/10)*sigmaL/vdrift;
}

double ERepSim::ResponseTPC::GetLongitudinalSpread(double dist){
    const double sigmaL = 0.237; //mm/sqrt(cm)
    return std::sqrt(dist)*sigmaL;
}