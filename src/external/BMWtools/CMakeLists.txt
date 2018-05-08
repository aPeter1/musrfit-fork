# - BMWtools library ----------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(CUBA_INC ${CMAKE_SOURCE_DIR}/src/external/libCuba/src)

root_generate_dictionary(
  BMWStartupHandlerDict BMWStartupHandler.h LINKDEF BMWStartupHandlerLinkDef.h
)

#--- create pkg-config info ---------------------------------------------------
#[==[ //as35 for now do not create a pkgconfig file
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(BMW_TOOLS_VERSION "1.0.0")
set(BMW_TOOLS_LIBRARY_NAME "BMWtools")
configure_file("BMWtools.pc.in" "BMWtools.pc" @ONLY)
#]==]

#--- lib creation -------------------------------------------------------------
add_library(BMWtools SHARED
  BMWStartupHandler.cpp
  BMWStartupHandlerDict.cxx
  TTrimSPDataHandler.cpp
  BMWIntegrator.cpp
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  BMWtools BEFORE PRIVATE 
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}> 
    $<BUILD_INTERFACE:${CUBA_INC}>
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(BMWtools ${ROOT_LIBRARIES}) # //as35 cuba lib needs to be added here!!

#--- install BMWtools solib ---------------------------------------------------
install(TARGETS BMWtools DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libBMWStartupHandlerDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libBMWStartupHandlerDict.rootmap
  DESTINATION lib
)

#--- install BMWtools header --------------------------------------------------
install(
  FILES 
    BMWStartupHandler.h
    TTrimSPDataHandler.h
    BMWIntegrator.h
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
#[==[ //as35 for now do not install a pkgconfig file
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/BMWtools.pc
  DESTINATION lib/pkgconfig
)
#]==]
