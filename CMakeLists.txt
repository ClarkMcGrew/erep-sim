cmake_minimum_required(VERSION 3.0)

# Set the project name and version.  The project name will be used to
# import this into other CMAKE files.
project(ERepSim VERSION 1.0.0)

message("Reconstruction for the 3DST -- ${VERSION}")

# Define the options that can be set in the cache, or on the cmake
# command line.
set(CMAKE_BUILD_TYPE Debug)

# Check to see if this is MACOS
if(APPLE)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif (APPLE)

# Make sure that ROOT is available.  ROOT is absolutely required.
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO)
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# CubeRecon needs the edep-sim I/O package.
find_package(EDepSim)
if(EDepSim_FOUND)
  message("EDepSim found")
endif(EDepSim_FOUND)

# Build the basic simulation
add_subdirectory(src)
add_subdirectory(app)

#############################################################
#
# Prepare the package so that it can be used with the find_package interface.
#
#############################################################

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Build the targets description so that the package can be configured
# using find_package.
install(EXPORT ERepSimTargets
  NAMESPACE ERepSim::
  DESTINATION lib/cmake/ERepSim)

# Write the 'ERepSimConfigVersion.cmake' file which can be used to
# check if a version meets the requested properties.
write_basic_package_version_file(
  ERepSimConfigVersion.cmake
  COMPATIBILITY SameMajorVersion)

# Write the 'ERepSimConfig.cmake' file so that a user package can
# access this with find_package.
configure_package_config_file(
  PackageConfig.cmake.in
  ERepSimConfig.cmake
  PATH_VARS CMAKE_INSTALL_PREFIX
  INSTALL_DESTINATION lib/cmake/ERepSim)

# Install the config files.
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ERepSimConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ERepSimConfigVersion.cmake
  DESTINATION lib/cmake/ERepSim )
