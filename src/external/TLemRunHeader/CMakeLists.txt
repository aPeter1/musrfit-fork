# -TLemRunHeader library ------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

root_generate_dictionary(
  TLemRunHeaderDict 
  TLemRunHeader.h 
  OPTIONS
    -I${CMAKE_CURRENT_SOURCE_DIR} 
    -inlineInputHeader
  LINKDEF TLemRunHeaderLinkDef.h
  MODULE TLemRunHeader
)
root_generate_dictionary(
  TLemStatsDict 
  TLemStats.h 
  OPTIONS 
    -I${CMAKE_CURRENT_SOURCE_DIR} 
    -inlineInputHeader
  LINKDEF TLemStatsLinkDef.h
  MODULE TLemStats 
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(LEM_VERSION "1.5.0")
set(LEM_LIBRARY_NAME "TLemRunHeader")
configure_file("TLemRunHeader.pc.in" "TLemRunHeader.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(TLemRunHeader SHARED
  TLemRunHeader.cxx
  TLemRunHeaderDict.cxx
  TLemStats.cxx
  TLemStatsDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(TLemRunHeader
  PROPERTIES
    VERSION ${LEM_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  TLemRunHeader BEFORE PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(TLemRunHeader ${ROOT_LIBRARIES})

#--- install TLemRunHeader solib ----------------------------------------------
install(TARGETS TLemRunHeader DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTLemRunHeader_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libTLemRunHeader.rootmap
        ${CMAKE_CURRENT_BINARY_DIR}/libTLemStats_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libTLemStats.rootmap
  DESTINATION lib
)

#--- install headers ----------------------------------------------------------
install(FILES TLemRunHeader.h TLemStats.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/TLemRunHeader.pc
  DESTINATION lib/pkgconfig
)
