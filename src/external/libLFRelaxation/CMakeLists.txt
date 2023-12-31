# - libLFRelaxation library ---------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(BMW_TOOLS_INC ${CMAKE_SOURCE_DIR}/src/external/BMWtools)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are
# setup only afterwards.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

root_generate_dictionary(
  TLFRelaxationDict
  TLFRelaxation.h
  OPTIONS
    -I${FFTW3_INCLUDE}
    -I${MUSRFIT_INC}
    -I${BMW_TOOLS_INC}
    -inlineInputHeader
  LINKDEF TLFRelaxationLinkDef.h
  MODULE TLFRelaxation
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
add_library(LFRelaxation SHARED
  TLFRelaxation.cpp
  TLFRelaxationDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(LFRelaxation
  PROPERTIES
    VERSION "1.0.0"
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  LFRelaxation BEFORE PRIVATE
    $<BUILD_INTERFACE:${FFTW3_INCLUDE}>
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${BMW_TOOLS_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
if (OpenMP_FOUND)
  target_compile_options(LFRelaxation PUBLIC ${OpenMP_CXX_FLAGS})
endif (OpenMP_FOUND)

set(gomp "")
if (OpenMP_FOUND)
  if (OpenMP_CXX_LIBRARIES)
    set(gomp ${OpenMP_CXX_LIBRARIES})
  else (OpenMP_CXX_LIBRARIES)
    set(gomp ${OpenMP_CXX_FLAGS}) # for older cmake OpenMP_CXX_LIBRARIES is not defined
  endif (OpenMP_CXX_LIBRARIES)
endif (OpenMP_FOUND)
target_link_libraries(LFRelaxation 
   ${gomp}
   ${GSL_LIBRARIES} FFTW3::FFTW3F
   ${ROOT_LIBRARIES} PUserFcnBase cuba BMWtools
)

#--- install LFRelaxation solib -----------------------------------------------
install(TARGETS LFRelaxation DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTLFRelaxation_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTLFRelaxation.rootmap
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
