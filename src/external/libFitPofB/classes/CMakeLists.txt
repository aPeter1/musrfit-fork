# - libFitPofB library --------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)
set(POFB_INC ${CMAKE_SOURCE_DIR}/src/external/libFitPofB/include)

root_generate_dictionary(
  TLondon1DDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  -I${POFB_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  TLondon1D.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/TLondon1DLinkDef.h
)
root_generate_dictionary(
  TVortexDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  -I${POFB_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  TVortex.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/TVortexLinkDef.h
)
root_generate_dictionary(
  TSkewedGssDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  -I${POFB_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  TSkewedGss.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/TSkewedGssLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(FIT_P_OF_B_VERSION "1.0.0")
set(FIT_P_OF_B_LIBRARY_NAME "FitPofB")
configure_file("FitPofB.pc.in" "FitPofB.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(FitPofB SHARED
  TBofZCalc.cpp
  TBulkTriVortexFieldCalc.cpp
  TFilmTriVortexFieldCalc.cpp 
  TLondon1D.cpp
  TLondon1DDict.cxx
  TPofBCalc.cpp
  TPofTCalc.cpp
  TVortex.cpp
  TVortexDict.cxx
  TSkewedGss.cpp
  TSkewedGssDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  FitPofB BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${POFB_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(FitPofB ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase BMWtools)

#--- install FitPofB solib ----------------------------------------------------
install(TARGETS FitPofB DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTLondon1DDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTLondon1DDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libTVortexDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTVortexDict.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libTSkewedGssDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTSkewedGssDict.rootmap
  DESTINATION lib
)

#--- install FitPofB header ---------------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TBofZCalc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TBulkTriVortexFieldCalc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TFilmTriVortexFieldCalc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TLondon1D.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TPofBCalc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TPofTCalc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TSkewedGss.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/TVortex.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/FitPofB.pc
  DESTINATION lib/pkgconfig
)
#]==]

