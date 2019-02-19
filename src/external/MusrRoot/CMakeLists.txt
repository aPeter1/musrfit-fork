# - TMusrRunHeader library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRROOT_INC ${CMAKE_SOURCE_DIR}/src/external/MusrRoot)
set(PREFIX_INC ${CMAKE_INSTALL_PREFIX}/include)

root_generate_dictionary(
  TMusrRunHeaderDict 
  -I${MUSRROOT_INC} -I${PREFIX_INC} TMusrRunHeader.h 
  LINKDEF TMusrRunHeaderLinkDef.h
  OPTIONS -inlineInputHeader
  MODULE TMusrRunHeader
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(MUSR_ROOT_VERSION "1.0.0")
set(MUSR_ROOT_LIBRARY_NAME "TMusrRunHeader")
configure_file("TMusrRunHeader.pc.in" "TMusrRunHeader.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(TMusrRunHeader SHARED
  TMusrRunHeader.cpp
  TMusrRunHeaderDict.cxx
)
#--- make sure that the include directory is found ----------------------------
target_include_directories(
  TMusrRunHeader BEFORE PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(TMusrRunHeader
  PROPERTIES
    VERSION ${MUSR_ROOT_VERSION}
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(TMusrRunHeader ${ROOT_LIBRARIES})

#--- install TMusrRunHeader solib ---------------------------------------------
install(TARGETS TMusrRunHeader DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTMusrRunHeader_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libTMusrRunHeader.rootmap
  DESTINATION lib
)

#--- install TMusrRunHeader header --------------------------------------------
install(FILES TMusrRunHeader.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/TMusrRunHeader.pc
  DESTINATION lib/pkgconfig
)
