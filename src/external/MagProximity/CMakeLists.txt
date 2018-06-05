# - Mag. Proximity library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
root_generate_dictionary(
  PMagProximityFitterDict 
  -I${FFTW3_INCLUDE}
  -I${MUSRFIT_INC}
  PMagProximityFitter.h 
  LINKDEF PMagProximityFitterLinkDef.h
)
root_generate_dictionary(
  PMPStartupHandlerDict PMPStartupHandler.h LINKDEF PMPStartupHandlerLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(P_MAG_PROXIMITY_FITTER_VERSION "1.0.0")
set(P_MAG_PROXIMITY_FITTER_LIBRARY_NAME "PMagProximityFitter")
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
  PMagProximityFitter BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PMagProximityFitter
  PROPERTIES
    VERSION ${P_MAG_PROXIMITY_FITTER_VERSION}
)

#--- add compile options for optimization -------------------------------------
target_compile_options(PMagProximityFitter PUBLIC -O3)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PMagProximityFitter ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase)

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


