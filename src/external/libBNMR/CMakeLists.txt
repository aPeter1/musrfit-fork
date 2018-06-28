# - beta-NMR library ----------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  TBNMRDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  TBNMR.h 
  LINKDEF TBNMRLinkDef.h
  MODULE TBNMR
)

#--- lib creation -------------------------------------------------------------
add_library(BNMR SHARED
  TBNMR.cpp
  TBNMRDict.cxx
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  BNMR BEFORE PRIVATE 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(BNMR
  PROPERTIES
    VERSION "1.0.0"
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(BNMR ${ROOT_LIBRARIES} PUserFcnBase)

#--- install libBNMR solib ----------------------------------------------------
install(TARGETS BNMR DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTBNMR_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libTBNMR.rootmap
  DESTINATION lib
)

#--- install libBNMR header ---------------------------------------------------
install( 
  FILES 
    TBNMR.h
  DESTINATION 
    include
)

