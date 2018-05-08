# - libLFRelaxation library ---------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)

root_generate_dictionary(
  TLFRelaxationDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  TLFRelaxation.h
  LINKDEF TLFRelaxationLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(T_LF_RELAXATION_VERSION "1.0.0")
set(T_LF_RELAXATION_LIBRARY_NAME "TLFRelaxation")
configure_file("TLFRelaxation.pc.in" "TLFRelaxation.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(TLFRelaxation SHARED
  TLFRelaxation.cpp
  TLFRelaxationDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  TLFRelaxation BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(TLFRelaxation ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase BMWtools) # //as35 cuba lib still missing

#--- install TLFRelaxation solib ----------------------------------------------
install(TARGETS TLFRelaxation DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTLFRelaxationDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTLFRelaxationDict.rootmap
  DESTINATION lib
)

#--- install TLFRelaxation header ---------------------------------------------
install(
  FILES 
    TLFRelaxation.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/TLFRelaxation.pc
  DESTINATION lib/pkgconfig
)
#]==]

