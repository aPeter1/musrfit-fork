# - libGapIntegrals library ---------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)

root_generate_dictionary(
  TGapIntegralsDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  TGapIntegrals.h
  LINKDEF TGapIntegralsLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(GAP_INTEGRALS_VERSION "1.0.0")
set(GAP_INTEGRALS_LIBRARY_NAME "GapIntegrals")
configure_file("GapIntegrals.pc.in" "GapIntegrals.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(GapIntegrals SHARED
  TGapIntegrals.cpp
  TGapIntegralsDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(GapIntegrals
  PROPERTIES
    VERSION "1.0.0"
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  GapIntegrals BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add compile options for optimization -------------------------------------
target_compile_options(GapIntegrals PUBLIC -O3)

#--- add library dependencies -------------------------------------------------
target_link_libraries(GapIntegrals 
  ${GSL_LIBRARIES} ${FFTW3F_LIBRARY} ${ROOT_LIBRARIES} 
  PUserFcnBase cuba BMWtools
)

#--- install LFRelaxation solib -----------------------------------------------
install(TARGETS GapIntegrals DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTGapIntegralsDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTGapIntegralsDict.rootmap
  DESTINATION lib
)

#--- install GapIntegrals header ----------------------------------------------
install(
  FILES 
    TGapIntegrals.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/GapIntegrals.pc
  DESTINATION lib/pkgconfig
)
#]==]

