# - Mag. Proximity library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
root_generate_dictionary(
  PMagProximityFitterDict PMagProximityFitter.h LINKDEF PMagProximityFitterLinkDef.h
)
root_generate_dictionary(
  PMPStartupHandlerDict PMPStartupHandler.h LINKDEF PMPStartupHandlerLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(PNL_PIPPARDFITTER_VERSION "1.0.0")
set(PNL_PIPPARDFITTER_LIBRARY_NAME "PMagProximityFitter")
configure_file("PMagProximityFitter.pc.in" "PMagProximityFitter.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PMagProximityFitter SHARED
  PMagProximityFitter.cpp
  PMagProximityFitterDict.cxx
  PMPRgeHandler.cpp
  PMPStartupHandler.cpp
  PMPStartupHandlerDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PMagProximityFitter BEFORE PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PMagProximityFitter PUserFcnBase ${ROOT_LIBRARIES})

#--- install PMagProximityFitter solib ----------------------------------------
install(TARGETS PMagProximityFitter DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPMagProximityFitterDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPMagProximityFitterDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPMPStartupHandlerDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPMPStartupHandlerDict.rootmap
  DESTINATION lib
)

#--- install PMagProximityFitter header ---------------------------------------
install(
  FILES
    PMagProximity.h 
    PMagProximityFitter.h 
    PMPRgeHandler.h
    PMPStartupHandler.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PMagProximityFitter.pc
  DESTINATION lib/pkgconfig
)


