# - libFitPofB library --------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)
set(FIT_P_OF_B_INC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${FIT_P_OF_B_INC})

root_generate_dictionary(
  TLondon1DDict 
  TLondon1D.h 
  OPTIONS
    -I${FFTW3_INCLUDE} 
    -I${MUSRFIT_INC}
    -I${BMW_TOOLS_INC}
    -I${FIT_P_OF_B_INC}
    -inlineInputHeader
  LINKDEF ${FIT_P_OF_B_INC}/TLondon1DLinkDef.h
  MODULE TLondon1D
)
root_generate_dictionary(
  TVortexDict 
  TVortex.h 
  OPTIONS 
    -I${FFTW3_INCLUDE} 
    -I${MUSRFIT_INC}
    -I${BMW_TOOLS_INC}
    -I${FIT_P_OF_B_INC}
    -inlineInputHeader
  LINKDEF ${FIT_P_OF_B_INC}/TVortexLinkDef.h
  MODULE TVortex
)
root_generate_dictionary(
  TSkewedGssDict 
  TSkewedGss.h 
  OPTIONS 
    -I${FFTW3_INCLUDE} 
    -I${MUSRFIT_INC}
    -I${BMW_TOOLS_INC}
    -I${POFB_INC}
    -I${FIT_P_OF_B_INC}
    -inlineInputHeader
  LINKDEF ${FIT_P_OF_B_INC}/TSkewedGssLinkDef.h
  MODULE TSkewedGss
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

#--- set target properties, e.g. version --------------------------------------
set_target_properties(FitPofB
  PROPERTIES
    VERSION "1.0.0"
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  FitPofB BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/external/TLemRunHeader>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${POFB_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

#--- add library dependencies -------------------------------------------------
if (OpenMP_FOUND)
  target_compile_options(FitPofB PUBLIC ${OpenMP_CXX_FLAGS})
endif (OpenMP_FOUND)

set(gomp "")
if (OpenMP_FOUND AND (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux))
  set(gomp gomp)
endif (OpenMP_FOUND AND (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux))
target_link_libraries(FitPofB 
    ${gomp}
    FFTW3::FFTW3 FFTW3::FFTW3F ${ROOT_LIBRARIES} 
    TLemRunHeader PUserFcnBase BMWtools
)

#--- install FitPofB solib ----------------------------------------------------
install(TARGETS FitPofB DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTLondon1D_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTLondon1D.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libTVortex_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTVortex.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libTSkewedGss_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTSkewedGss.rootmap
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

