# - Photo Meissner library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(PHOTO_MEISSNER_INC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${PHOTO_MEISSNER_INC})

root_generate_dictionary(
  PPhotoMeissnerDict
  PPhotoMeissner.h 
  OPTIONS 
    -I${FFTW3_INCLUDE} 
    -I${GSL_INCLUDE_DIRS} 
    -I${ROOT_INCLUDE_DIRS}
    -I${MUSRFIT_INC}
    -I${PHOTO_MEISSNER_INC}
    -inlineInputHeader
  LINKDEF ${PHOTO_MEISSNER_INC}/PPhotoMeissnerLinkDef.h
  MODULE PPhotoMeissner
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(P_PHOTO_MEISSNER_VERSION "1.0.0")
set(P_PHOTO_MEISSNER_LIBRARY_NAME "PPhotoMeissner")
configure_file("PPhotoMeissner.pc.in" "PPhotoMeissner.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PPhotoMeissner SHARED
  PPhotoMeissner.cpp
  PPhotoMeissnerDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PPhotoMeissner
  PROPERTIES
    VERSION ${P_PHOTO_MEISSNER_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PPhotoMeissner BEFORE PRIVATE
    $<BUILD_INTERFACE:${FFTW3_INCLUDE}> 
    $<BUILD_INTERFACE:${GSL_INCLUDE_DIRS}> 
    $<BUILD_INTERFACE:${ROOT_INCLUDE_DIRS}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PPhotoMeissner 
   FFTW3::FFTW3 ${GSL_LIBRARY} ${ROOT_LIBRARIES} PRgeHandler PUserFcnBase
)

#--- install PPhotoMeissner solib ---------------------------------------------
install(TARGETS PPhotoMeissner DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPPhotoMeissner_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPPhotoMeissner.rootmap
  DESTINATION lib
)

#--- install PPhotoMeissner header --------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/PPhotoMeissner.h 
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PPhotoMeissner.pc
  DESTINATION lib/pkgconfig
)
