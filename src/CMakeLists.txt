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
  ERepSimDetectorECal.cxx ECal_Cell.cxx STT_Digit.cxx
  )

set(includes
  ERepSimOutput.hxx ERepSimInput.hxx
  ERepSimCarrier.hxx ERepSimImpulse.hxx ERepSimDigiHit.hxx
  ERepSimResponseBase.hxx ERepSimResponse3DST.hxx
  ERepSimSensorBase.hxx  ERepSimSensorIdeal.cxx
  ERepSimDAQBase.hxx ERepSimDAQMultiHit.hxx
  ERepSimDetectorBase.hxx ERepSimDetector3DST.hxx
  ERepSimDetectorTPC.hxx ERepSimResponseTPC.hxx
  ERepSimDetectorECal.hxx ECal_Cell.hxx STT_Digit.hxx
  ERepSimDefs.hxx
  )

# Make sure the current directories are available for the later
# compilation.  This is required to make ROOT_GENERATE_DICTIONARY
# work.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__ECal_io
  ECal_Cell.hxx
  OPTIONS -inlineInputHeader
  LINKDEF ECal_LinkDef.h)

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__STT_io
  STT_Digit.hxx
  OPTIONS -inlineInputHeader
  LINKDEF STT_LinkDef.h)


# Build the library.
add_library(erepsim SHARED ${source} G__ECal_io.cxx G__STT_io.cxx)

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

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libECal_io.rootmap
  DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libECal_io_rdict.pcm
  DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSTT_io.rootmap
  DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSTT_io_rdict.pcm
  DESTINATION lib)
