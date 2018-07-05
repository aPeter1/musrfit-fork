# - Spin Valve library --------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  PSkewedLorentzianDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  PSkewedLorentzian.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/PSkewedLorentzianLinkDef.h
  MODULE PSkewedLorentzian
)
root_generate_dictionary(
  PStartupHandler_SVDict 
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  PStartupHandler_SV.h
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/PStartupHandler_SVLinkDef.h
  MODULE PStartupHandler_SV
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(P_SPIN_VALVE_VERSION "1.0.0")
set(P_SPIN_VALVE_LIBRARY_NAME "PSpinValve")
configure_file("PSpinValve.pc.in" "PSpinValve.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PSpinValve SHARED
  PSkewedLorentzian.cpp
  PSkewedLorentzianDict.cxx
  PStartupHandler_SV.cpp
  PStartupHandler_SVDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PSpinValve
  PROPERTIES
    VERSION ${P_SPIN_VALVE_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PSpinValve BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PSpinValve ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase)

#--- install PSpinValve solib -------------------------------------------------
install(TARGETS PSpinValve DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPSkewedLorentzian_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPSkewedLorentzian.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler_SV_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler_SV.rootmap
  DESTINATION lib
)

#--- install PSpinValve header ------------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/PSkewedLorentzian.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/PStartupHandler_SV.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PSpinValve.pc
  DESTINATION lib/pkgconfig
)

