# - TMusrRunHeader library ----------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
root_generate_dictionary(
  TMusrRunHeaderDict TMusrRunHeader.h LINKDEF TMusrRunHeaderLinkDef.h
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

#--- add library dependencies -------------------------------------------------
target_link_libraries(TMusrRunHeader ${ROOT_LIBRARIES})

#--- install TMusrRunHeader solib ---------------------------------------------
install(TARGETS TMusrRunHeader DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libTMusrRunHeaderDict_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libTMusrRunHeaderDict.rootmap
  DESTINATION lib
)

#--- install TMusrRunHeader header --------------------------------------------
install(FILES TMusrRunHeader.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/TMusrRunHeader.pc
  DESTINATION lib/pkgconfig
)
