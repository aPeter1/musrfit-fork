# - libCalcMeanFieldsLEM library ----------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)
set(POFB_INC ${CMAKE_SOURCE_DIR}/src/external/libFitPofB/include)

root_generate_dictionary(
  TCalcMeanFieldsLEMDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${BMW_TOOLS_INC}
  -I${POFB_INC}
  TCalcMeanFieldsLEM.h
  LINKDEF TCalcMeanFieldsLEMLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(T_CALC_MEAN_FIELD_LEM_VERSION "1.0.0")
set(T_CALC_MEAN_FIELD_LEM_LIBRARY_NAME "TCalcMeanFieldsLEM")
configure_file("TCalcMeanFieldsLEM.pc.in" "TCalcMeanFieldsLEM.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(CalcMeanFieldsLEM SHARED
  TCalcMeanFieldsLEM.cpp
  TCalcMeanFieldsLEMDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(CalcMeanFieldsLEM
  PROPERTIES
    VERSION "1.0.0"
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  CalcMeanFieldsLEM BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${POFB_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add compile options for optimization -------------------------------------
target_compile_options(CalcMeanFieldsLEM PUBLIC -O3)

#--- add library dependencies -------------------------------------------------
target_link_libraries(CalcMeanFieldsLEM 
  ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} BMWtools FitPofB PUserFcnBase
)

#--- install CalcMeanFieldsLEM solib ------------------------------------------
install(TARGETS CalcMeanFieldsLEM DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTCalcMeanFieldsLEMDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTCalcMeanFieldsLEMDict.rootmap
  DESTINATION lib
)

#--- install CalcMeanFieldsLEM header -----------------------------------------
install(
  FILES 
    TCalcMeanFieldsLEM.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/TCalcMeanFieldsLEM.pc
  DESTINATION lib/pkgconfig
)
#]==]

