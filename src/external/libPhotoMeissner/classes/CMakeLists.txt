# - Photo Meissner library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  PPhotoMeissnerDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  PPhotoMeissner.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/PPhotoMeissnerLinkDef.h
  MODULE PPhotoMeissner
)
root_generate_dictionary(
  PStartupHandler_PMDict 
  -I${MUSRFIT_INC}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../include 
  PStartupHandler_PM.h 
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../include/PStartupHandler_PMLinkDef.h
  MODULE PStartupHandler_PM
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
  PStartupHandler_PM.cpp
  PStartupHandler_PMDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PPhotoMeissner
  PROPERTIES
    VERSION ${P_PHOTO_MEISSNER_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PPhotoMeissner BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PPhotoMeissner 
   ${FFTW3_LIBRARY} ${GSL_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase
)

#--- install PPhotoMeissner solib ---------------------------------------------
install(TARGETS PPhotoMeissner DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPPhotoMeissner_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPPhotoMeissner.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler_PM_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPStartupHandler_PM.rootmap
  DESTINATION lib
)

#--- install PPhotoMeissner header --------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/PPhotoMeissner.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/PStartupHandler_PM.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PPhotoMeissner.pc
  DESTINATION lib/pkgconfig
)
