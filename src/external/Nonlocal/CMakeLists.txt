# - Nonlocal library ----------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  PNL_PippardFitterDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  PNL_PippardFitter.h 
  LINKDEF PNL_PippardFitterLinkDef.h
)
root_generate_dictionary(
  PNL_StartupHandlerDict PNL_StartupHandler.h LINKDEF PNL_StartupHandlerLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(PNL_PIPPARDFITTER_VERSION "1.0.0")
set(PNL_PIPPARDFITTER_LIBRARY_NAME "PNL_PippardFitter")
configure_file("PNL_PippardFitter.pc.in" "PNL_PippardFitter.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PNL_PippardFitter SHARED
  PNL_PippardFitter.cpp
  PNL_PippardFitterDict.cxx
  PNL_RgeHandler.cpp
  PNL_StartupHandler.cpp
  PNL_StartupHandlerDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PNL_PippardFitter BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PNL_PippardFitter ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase)

#--- install PNL_PippardFitter solib ------------------------------------------
install(TARGETS PNL_PippardFitter DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPNL_PippardFitterDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPNL_PippardFitterDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPNL_StartupHandlerDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPNL_StartupHandlerDict.rootmap
  DESTINATION lib
)

#--- install PNL_PippardFitter header -----------------------------------------
install(
  FILES 
    PNL_PippardFitter.h 
    PNL_RgeHandler.h
    PNL_StartupHandler.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PNL_PippardFitter.pc
  DESTINATION lib/pkgconfig
)
