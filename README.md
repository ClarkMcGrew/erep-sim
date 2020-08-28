# erep-sim

A library to support simulating the energy response generated using
edep-sim.  While this is initially simulationg the 3DST scintillator
cube detector, it is using the same techiques (and almost the same
class structure) as the T2K elecSim.  It is well suited to simulated
any detector (that is likely to be used) in the DUNE ND.

## Requirements

This needs to find ROOT, edep-sim and their dependencies in the cmake
search path.

## Implementation Status 

This is a work in progress:

* The 3DST detector response model (as of 8.2020)

..* The response model for the 3DST is relatively complete.  It
  includes the important effects like energy to photons, light
  crossing between cubes, photon arrival times, attenuation, and such.

..* The first sensor model is a place holder.  It directly translates
  the input classes to the output classes.  For the 3DST, this is
  "almost" sufficient since the photon detection efficiency can be
  approximated by the MeV to carrier normalization, but it doesn't
  include the sensor jitter (timing or charge), or non-linear effects.

..* The first DAQ model is a place holder that implements a simple
  multi-hit ADC/TDC.  This is a starting point for a scintillator
  detector, but not for a TPC.  It is hard-coded with a 50ns
  integration window, and a 0.5ns digitization time.  The threshold is
  at 1 pe.

# Design

The simulation is divided into three basic steps, and base classes are
provided for each.  The steps are the response, sensing, and DAQ
processes.  These steps are then combined using detector classes which
handle each sub-detector being simulated.

## Response Simulation

The first step simulates the detector response, which translates the
energy deposition into a "Carrier".  This class accepts a container of
edep-sim hit segments and translates them into carriers.  The carrier
represents a photon, ionization electron (or cloud of electrons) that
can be transported to the sensor.  The response classes are
responsible for simulating the effects of transporting the carrier to
the sensor.

## Sensor Simulation

The second step simulates the sensor.  This simulates the effect of a
photo-sensor, micromegas, or other sensor on the carriers produced by
the detector response.  The output of the sensor step is are impulses
of current (or a vector of current vs time) that is sent to the DAQ
step.  The impulses are represented by the time, total charge, and
time width of the current produced by the sensor.

## Electronics, DAQ, and Calibration

The third step simulates the "DAQ" which handles the effects of
digitization of the current coming from the sensors, and then
"calibrates" the digitized values into hits.

## Overall Detector Class

The detector class is called by the main loop and is responsible using
the response, sensor and DAQ classes to simulate the response of a
particular detector. It takes EDepSim hit segments and uses the other
classes to build a "calibrated" hit that is saved in the output tree.

# Output

The output of the simulation is a very simple tree that is suitable to
be used as a "friend" of the edep-sim tree.  All of the information is
saved in vectors (of int or float).  There is a second "Property" tree
that is created with a map from strings to float and string values.

The trees can be easily managed by hand, but you can also copy the
ERepSimInput.hxx include file into your own project.  This has some
skeleton code to attach to the tree and declare the branches.
