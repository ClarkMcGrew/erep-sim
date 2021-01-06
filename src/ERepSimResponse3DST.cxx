#include "ERepSimResponse3DST.hxx"
#include "ERepSimOutput.hxx"

#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TRandom.h>

#include <iostream>

ERepSim::Response3DST::Response3DST()
    : ERepSim::ResponseBase("3DST") {}

ERepSim::Response3DST::~Response3DST() {}

bool ERepSim::Response3DST::RecurseGeometry() {
    TGeoNode* node = gGeoManager->GetCurrentNode();

    std::string name = node->GetName();

    double local[3] = {0,0,0};
    double master[3];
    gGeoManager->LocalToMaster(local,master);

    if (name.find("3DSTPlane_PV") != std::string::npos) {
        ++fPlanes;
        fPlaneMin = std::min(fPlaneMin,master[2]);
        fPlaneMax = std::max(fPlaneMin,master[2]);
    }
    else if (name.find("3DSTBar_PV") != std::string::npos) {
        ++fBars;
        fBarMin = std::min(fBarMin,master[1]);
        fBarMax = std::max(fBarMin,master[1]);
    }
    else if (name.find("volcube_PV") != std::string::npos) {
        ++fCubes;
        fCubeMin = std::min(fCubeMin,master[0]);
        fCubeMax = std::max(fCubeMin,master[0]);
    }

    // Sort circuit.  Only look at the daughers for the "_0" plane and bar.
    if (name.find("3DSTPlane_PV") != std::string::npos
        && name.find("3DSTPlane_PV_0") == std::string::npos) return false;
    if (name.find("3DSTBar_PV") != std::string::npos
        && name.find("3DSTBar_PV_0") == std::string::npos) return false;

    // Handle the daughters
    for (int i=0; i<node->GetNdaughters(); ++i) {
        gGeoManager->CdDown(i);
        RecurseGeometry();
        gGeoManager->CdUp();
    }

    return true;
}

void ERepSim::Response3DST::Initialize() {
    std::cout << "Response3DST::Initialize" << std::endl;
    // This is the number of photons per MeV generated in each fiber before
    // attenuation.  The value is set based on the CERN beam test prototype
    // which measured ~65 pe per fiber per 10 mm cube for a MIP depositing
    // ~0.18 MeV/mm without mirroring (the 2.0 is for half the photons going
    // in each direction in the fiber).  This value includes the photon
    // detection efficiency (PDE), and assumes that the PDE is not applied in
    // the sensor simulation.
    fPhotPerMeV = (3.0*65.0/10.0/0.18) * 2.0;
    fLightVelocity = 200.0; // Assume the index of refraction is 1.5
    // The leakage is based on the CERN beamtest.  This is applied to the
    // energy, and not the number of photons, so it's 1/2 the published value
    // for each fiber which is normalized by measured light.
    fXLeakage = 0.0097;      // Based on the CERN beam test
    fYLeakage = 0.0097;      // Based on the CERN beam test
    fZLeakage = 0.0097;      // Based on the CERN beam test
    fCubes = 0;
    fCubeMin = 1E+10;
    fCubeMax = -1E+10;
    fBars = 0;
    fBarMin = 1E+10;
    fBarMax = -1E+10;
    fPlanes = 0;
    fPlaneMin = 1E+10;
    fPlaneMax = -1E+10;
    gGeoManager->CdTop();
    RecurseGeometry();
    fXPitch = (fCubeMax-fCubeMin)/(fCubes-1.0);
    fYPitch = (fBarMax-fBarMin)/(fBars-1.0);
    fZPitch = (fPlaneMax-fPlaneMin)/(fPlanes-1.0);

    std::cout << "Response for a 3DST that has "
              << fCubes << " x " << fBars << " x " << fPlanes
              << " cubes" << std::endl;

    ERepSim::Output::Get().Property["3DST.Response.PhotPerMeV"] = fPhotPerMeV;
    ERepSim::Output::Get().Property["3DST.Response.Velocity"] = fLightVelocity;

    ERepSim::Output::Get().Property["3DST.Response.XLeakage"] = fXLeakage;
    ERepSim::Output::Get().Property["3DST.Response.YLeakage"] = fYLeakage;
    ERepSim::Output::Get().Property["3DST.Response.ZLeakage"] = fZLeakage;

    // Describe the detector for posterity...
    ERepSim::Output::Get().Property["3DST.Response.Cubes"] = fCubes;
    ERepSim::Output::Get().Property["3DST.Response.CubeMin"] = fCubeMin;
    ERepSim::Output::Get().Property["3DST.Response.CubeMax"] = fCubeMax;

    ERepSim::Output::Get().Property["3DST.Response.Bars"] = fBars;
    ERepSim::Output::Get().Property["3DST.Response.BarMin"] = fBarMin;
    ERepSim::Output::Get().Property["3DST.Response.BarMax"] = fBarMax;

    ERepSim::Output::Get().Property["3DST.Response.Planes"] = fPlanes;
    ERepSim::Output::Get().Property["3DST.Response.PlaneMin"] = fPlaneMin;
    ERepSim::Output::Get().Property["3DST.Response.PlaneMax"] = fPlaneMax;

    // Set the attenuation parameters.  The sensor distance is the distance
    // from the center of the "first" cube by the sensor.  This comes from
    // measurements of 1mm Kuraray fibers done at URochester in ~2010.
    ERepSim::Output::Get().Property["3DST.Response.Atten.Ratio12"] = 0.75;
    ERepSim::Output::Get().Property["3DST.Response.Atten.Tau1"] = 4600.0;
    ERepSim::Output::Get().Property["3DST.Response.Atten.Tau2"] = 33.0;
    ERepSim::Output::Get().Property["3DST.Response.Atten.SensorDist"] = 20.0;
    ERepSim::Output::Get().Property["3DST.Response.Atten.Reflect"] = 0.0;
    ERepSim::Output::Get().Property["3DST.Response.Atten.MirrorDist"] = 10.0;

    ERepSim::Output::Get().Property["3DST.Response.Scint.Ratio12"] = 0.50;
    ERepSim::Output::Get().Property["3DST.Response.Scint.Life1"] = 6.0;
    ERepSim::Output::Get().Property["3DST.Response.Scint.Life2"] = 11.0;

}

void ERepSim::Response3DST::Reset() {
    ERepSim::ResponseBase::Reset();
}

void ERepSim::Response3DST::Process(const TG4HitSegmentContainer& segments) {
    std::cout << "Response3DST::Process " << segments.size() << " segments"
              << std::endl;
    TLorentzVector avg;
    for (std::size_t i = 0; i < segments.size(); ++i) {
        const TG4HitSegment& segment = segments[i];
        int segId = GetNextSegmentIdentifier();
        avg = 0.5*segment.GetStart() + 0.5*segment.GetStop();
        double deposit = segment.GetSecondaryDeposit();
        if (deposit < 1E-6) deposit = segment.GetEnergyDeposit();
        AddDeposit(segId, &segment, avg, deposit);
    }
    std::cout << "Response3DST::Process " << CountCarriers()
              << " carriers generated" << std::endl;
}

void ERepSim::Response3DST::AddDeposit(int segId, const TG4HitSegment* seg,
                                       TLorentzVector pos,
                                       double deposit) {
    // Find the position of the cube
    gGeoManager->FindNode(pos.X(),pos.Y(),pos.Z());
    double local[3] = {0,0,0};
    double master[3];
    gGeoManager->LocalToMaster(local,master);
    TLorentzVector cube(master[0],master[1],master[2],pos.T());

    double localDeposit = 1.0 - 2.0*fXLeakage - 2.0*fYLeakage - 2.0*fZLeakage;
    AddCubeDeposit(segId,seg,cube,localDeposit*deposit);

    TLorentzVector neighbor = cube + TLorentzVector(fXPitch,0,0,0);
    AddCubeDeposit(segId,seg,neighbor,fXLeakage*deposit);
    neighbor = cube - TLorentzVector(fXPitch,0,0,0);
    AddCubeDeposit(segId,seg,neighbor,fXLeakage*deposit);

    neighbor = cube + TLorentzVector(0,fYPitch,0,0);
    AddCubeDeposit(segId,seg,neighbor,fYLeakage*deposit);
    neighbor = cube - TLorentzVector(0,fYPitch,0,0);
    AddCubeDeposit(segId,seg,neighbor,fYLeakage*deposit);

    neighbor = cube + TLorentzVector(0,0,0,fZPitch);
    AddCubeDeposit(segId,seg,neighbor,fZLeakage*deposit);
    neighbor = cube - TLorentzVector(0,0,0,fZPitch);
    AddCubeDeposit(segId,seg,neighbor,fZLeakage*deposit);
}

void ERepSim::Response3DST::AddCubeDeposit(int segId, const TG4HitSegment* seg,
                                           TLorentzVector cube,
                                           double deposit) {
    int c = GetCube(cube);
    int b = GetBar(cube);
    int p = GetPlane(cube);

    double sensorDist = ERepSim::Output::Get().Property[
        "3DST.Response.Atten.SensorDist"];
    double mirrorDist = ERepSim::Output::Get().Property[
        "3DST.Response.Atten.MirrorDist"];

    /// Handle the YZ fiber
    TLorentzVector fiberX(cube);
    fiberX.SetX(fCubeMin-sensorDist);
    double dX = (fiberX-cube).Vect().Mag();
    fiberX.SetT(fiberX.T() + dX/fLightVelocity);
    int sensX = GetSensorId(-1,b,p);
    double lenX = fCubeMax-fCubeMin + sensorDist + mirrorDist;
    AddFiberDeposit(sensX, segId,seg, fiberX, cube, lenX, deposit);

    /// Handle the XZ fiber
    TLorentzVector fiberY(cube);
    fiberY.SetY(fBarMin-sensorDist);
    double dY = (fiberY-cube).Vect().Mag();
    fiberY.SetT(fiberY.T() + dY/fLightVelocity);
    int sensY = GetSensorId(c,-1,p);
    double lenY = fBarMax-fBarMin + sensorDist + mirrorDist;
    AddFiberDeposit(sensY, segId,seg, fiberY, cube, lenY, deposit);

    /// Handle the XY Fiber
    TLorentzVector fiberZ(cube);
    fiberZ.SetZ(fPlaneMin-sensorDist);
    double dZ = (fiberZ-cube).Vect().Mag();
    fiberZ.SetT(fiberZ.T() + dZ/fLightVelocity);
    int sensZ = GetSensorId(c,b,-1);
    double lenZ = fPlaneMax-fPlaneMin + sensorDist + mirrorDist;
    AddFiberDeposit(sensZ, segId,seg, fiberZ, cube, lenZ, deposit);
}

void ERepSim::Response3DST::AddFiberDeposit(int sensId, int segId,
                                            const TG4HitSegment* seg,
                                            TLorentzVector fiber,
                                            TLorentzVector cube,
                                            double fiberLength,
                                            double deposit) {
    double generatedPhotons = deposit*fPhotPerMeV/3.0;
    double dist = (cube.Vect()-fiber.Vect()).Mag();
    double survival = PhotonSurvivalProb(dist);
    // Add half the photons going directly toward the sensor
    double expected = 0.5*generatedPhotons*survival;
    int photons = gRandom->Poisson(expected);
    AddFiberPhotons(sensId,segId,seg,fiber,cube,photons);

    // Add half the photons that started going toward the mirror.
    double reflectivity =
        ERepSim::Output::Get().Property["3DST.Response.Atten.Reflect"];
    double longDist = 2.0*fiberLength - dist;
    double extraDist = longDist - dist;
    survival = reflectivity*PhotonSurvivalProb(longDist);
    expected =  0.5*generatedPhotons*survival;
    photons = gRandom->Poisson(expected);
    fiber.SetT(fiber.T() + extraDist/fLightVelocity);
    AddFiberPhotons(sensId,segId,seg,fiber,cube,photons);

}

void ERepSim::Response3DST::AddFiberPhotons(int sensId, int segId,
                                            const TG4HitSegment* seg,
                                            TLorentzVector fiber,
                                            TLorentzVector cube,
                                            int photons) {
    if (photons<1) return;
    for (int i=0; i<photons; ++i) {
        double dT = GetDecayTime();
        std::shared_ptr<ERepSim::Carrier> carrier(
            new ERepSim::Carrier(sensId,segId,seg));
        carrier->SetInitialPosition(cube.X(),cube.Y(),cube.Z(),cube.T()+dT);
        carrier->SetFinalPosition(fiber.X(),fiber.Y(),fiber.Z(),fiber.T()+dT);
        carrier->SetCharge(1.0);
        (*fCarriers)[sensId].push_back(carrier);
    }
}

double ERepSim::Response3DST::PhotonSurvivalProb(double x) {
    if (x<0.0) return 0.0;
    double f = ERepSim::Output::Get().Property["3DST.Response.Atten.Ratio12"];
    double t1 = ERepSim::Output::Get().Property["3DST.Response.Atten.Tau1"];
    double t2 = ERepSim::Output::Get().Property["3DST.Response.Atten.Tau2"];
    double p0 = f * std::exp(-x/t1) + (1.0-f)*std::exp(-x/t2);
    return p0;
}

double ERepSim::Response3DST::GetDecayTime() {
    double d12 = ERepSim::Output::Get().Property["3DST.Response.Scint.Ratio12"];
    if (gRandom->Uniform() < d12) {
        double d1 = ERepSim::Output::Get().Property[
            "3DST.Response.Scint.Life1"];
        return gRandom->Exp(d1);
    }
    double d2 = ERepSim::Output::Get().Property["3DST.Response.Scint.Life2"];
    return gRandom->Exp(d2);
}


int ERepSim::Response3DST::GetCube(TLorentzVector cube) {
    double r = (cube.X()-fCubeMin)/fXPitch;
    return r + 0.5;
}

int ERepSim::Response3DST::GetBar(TLorentzVector cube) {
    double r = (cube.Y()-fBarMin)/fYPitch;
    return r + 0.5;
}

int ERepSim::Response3DST::GetPlane(TLorentzVector cube) {
    double r = (cube.Z()-fPlaneMin)/fZPitch;
    return r + 0.5;
}

int ERepSim::Response3DST::GetSensorId(int c, int b, int p) {
    int cnt = 0;
    if (c<0) {
        c = 511+c;
        ++cnt;
    }
    if (b<0) {
        b = 511+b;
        ++cnt;
    }
    if (p<0) {
        p = 511+p;
        ++cnt;
    }
    // if (cnt != 1) throw std::runtime_error("Not a sensor");
    int id = 13;
    id = c + (id << 9);
    id = b + (id << 9);
    id = p + (id << 9);
    return id;
}

// Local Variables:
// mode:c++
// c-basic-offset:4
// compile-command:"$(git rev-parse --show-toplevel)/build/erep-build.sh force"
// End:
