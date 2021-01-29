# Build all of the applications here.

# Add a program to test writing the objects.
add_executable(erepReadWriteTest erepReadWriteTest.cxx)
target_link_libraries(erepReadWriteTest
  LINK_PUBLIC erepsim EDepSim::edepsim_io)
install(TARGETS erepReadWriteTest RUNTIME DESTINATION bin)

# Add a program to test reading the objects include edepsim
add_executable(erepReadTest erepReadTest.cxx)
target_link_libraries(erepReadTest LINK_PUBLIC
  EDepSim::edepsim_io ${ROOT_LIBRARIES})
install(TARGETS erepReadTest RUNTIME DESTINATION bin)

# Add a program to test reading the objects without edepsim
add_executable(erepExportTest erepExportTest.cxx)
target_link_libraries(erepExportTest LINK_PUBLIC ${ROOT_LIBRARIES})
install(TARGETS erepExportTest RUNTIME DESTINATION bin)

# Add a program to test reading the objects without edepsim
add_executable(erep-merge erepMerge.cxx)
target_link_libraries(erep-merge
  LINK_PUBLIC erepsim EDepSim::edepsim_io)
install(TARGETS erep-merge RUNTIME DESTINATION bin)

