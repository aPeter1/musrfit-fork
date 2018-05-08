# - libZFRelaxation library ---------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)

root_generate_dictionary(
  ZFRelaxationDict 
  -I${FFTW3_INCLUDE_DIR} 
  -I${MUSRFIT_INC}
  ZFRelaxation.h
  LINKDEF ZFRelaxationLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(ZF_RELAXATION_VERSION "1.0.0")
set(ZF_RELAXATION_LIBRARY_NAME "ZFRelaxation")
configure_file("ZFRelaxation.pc.in" "ZFRelaxation.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(ZFRelaxation SHARED
  ZFRelaxation.cpp
  ZFRelaxationDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  ZFRelaxation BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(ZFRelaxation ${FFTW3_LIBRARY} ${ROOT_LIBRARIES} PUserFcnBase BMWtools) # //as35 cuba lib still missing

#--- install ZFRelaxation solib -----------------------------------------------
install(TARGETS ZFRelaxation DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libZFRelaxationDict_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libZFRelaxationDict.rootmap
  DESTINATION lib
)

#--- install ZFRelaxation header ----------------------------------------------
install(
  FILES 
    ZFRelaxation.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/ZFRelaxation.pc
  DESTINATION lib/pkgconfig
)
#]==]

