# - beta-NMR LineProfile library ----------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  libLineProfileDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  libLineProfile.h 
  LINKDEF libLineProfileLinkDef.h
  MODULE libLineProfile
)

#--- lib creation -------------------------------------------------------------
add_library(LineProfile SHARED
  libLineProfile.cpp
  libLineProfileDict.cxx
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  LineProfile BEFORE PRIVATE 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(LineProfile
  PROPERTIES
    VERSION "1.0.0"
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(LineProfile ${ROOT_LIBRARIES} PUserFcnBase)

#--- install libLineProfile solib ----------------------------------------------------
install(TARGETS LineProfile DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libLineProfile_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libLineProfile.rootmap
  DESTINATION lib
)

#--- install libLineProfile header ---------------------------------------------------
install( 
  FILES 
    libLineProfile.h
  DESTINATION 
    include
)

