# - DepthProfile library ------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(DEPTH_PROFILE_INC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${MUSRFIT_INC})
include_directories(${DEPTH_PROFILE_INC})

root_generate_dictionary(
  PDepthProfileDict 
  PDepthProfile.h 
  OPTIONS 
    -I${FFTW3_INCLUDE_DIR} 
    -I${MUSRFIT_INC}
    -I${DEPTH_PROFILE_INC}
    -inlineInputHeader
  LINKDEF ${DEPTH_PROFILE_INC}/PDepthProfileLinkDef.h
  MODULE PDepthProfile
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(P_DEPTH_PROFILE_VERSION "1.0.0")
set(P_DEPTH_PROFILE_LIBRARY_NAME "PDepthProfile")
configure_file("PDepthProfile.pc.in" "PDepthProfile.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PDepthProfile SHARED
  PDepthProfile.cpp
  PDepthProfileDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PDepthProfile
  PROPERTIES
    VERSION ${P_DEPTH_PROFILE_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PDepthProfile BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../inc>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PDepthProfile ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PRgeHandler PUserFcnBase)

#--- install PDepthProfile solib ----------------------------------------------
install(TARGETS PDepthProfile DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPDepthProfile_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPDepthProfile.rootmap
  DESTINATION lib
)

#--- install DepthProfile header ----------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/PDepthProfile.h 
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PDepthProfile.pc
  DESTINATION lib/pkgconfig
)

