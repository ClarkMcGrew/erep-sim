message("ERepSim Library")

# Set the default verbosity of the compilation...
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO)
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# Define the source and include files that should be used for the io
# part of ERepSim.
set(source
  ERepSimOutput.cxx
  ERepSimCarrier.cxx ERepSimImpulse.cxx ERepSimDigiHit.cxx
  ERepSimResponseBase.cxx  ERepSimResponse3DST.cxx
  ERepSimSensorBase.cxx ERepSimSensorIdeal.cxx
  ERepSimDAQBase.cxx ERepSimDAQMultiHit.cxx
  ERepSimDetectorBase.cxx ERepSimDetector3DST.cxx
  ERepSimDetectorTPC.cxx ERepSimResponseTPC.cxx
  )

set(includes
  ERepSimOutput.hxx ERepSimInput.hxx
  ERepSimCarrier.hxx ERepSimImpulse.hxx ERepSimDigiHit.hxx
  ERepSimResponseBase.hxx ERepSimResponse3DST.hxx
  ERepSimSensorBase.hxx  ERepSimSensorIdeal.cxx
  ERepSimDAQBase.hxx ERepSimDAQMultiHit.hxx
  ERepSimDetectorBase.hxx ERepSimDetector3DST.hxx
  ERepSimDetectorTPC.hxx ERepSimResponseTPC.hxx
  ERepSimTPCdef.hxx
  )

# Make sure the current directories are available for the later
# compilation.  This is required to make ROOT_GENERATE_DICTIONARY
# work.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build the library.
add_library(erepsim SHARED ${source})

target_include_directories(erepsim PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include/ERepSim>")

target_link_libraries(erepsim PUBLIC EDepSim::edepsim_io ${ROOT_LIBRARIES})

# Install the io library for ERepSim
install(TARGETS erepsim
  EXPORT ERepSimTargets
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/ERepSim)

# Install the header files.
install(FILES ${includes} DESTINATION include/ERepSim)
