# - BMWtools library ----------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(CUBA_INC ${CMAKE_SOURCE_DIR}/src/external/libCuba/src)

# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

root_generate_dictionary(
  BMWStartupHandlerDict 
  BMWStartupHandler.h 
  OPTIONS 
    -I${MUSRFIT_INC}
    -inlineInputHeader
  LINKDEF BMWStartupHandlerLinkDef.h
  MODULE BMWStartupHandler
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

#--- set target properties, e.g. version --------------------------------------
set_target_properties(BMWtools
  PROPERTIES
    VERSION "1.0.0"
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  BMWtools BEFORE PRIVATE 
    $<BUILD_INTERFACE:${CUBA_INC}>
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(BMWtools ${ROOT_LIBRARIES} cuba)

#--- install BMWtools solib ---------------------------------------------------
install(TARGETS BMWtools DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libBMWStartupHandler_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libBMWStartupHandler.rootmap
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
