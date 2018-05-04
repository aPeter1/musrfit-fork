# - PNeXus library ------------------------------------------------------------

include_directories(${NEXUS_INCLUDE_DIR}) # to get the nexus headers

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(PNEXUS_VERSION "0.9.0")
set(PNEXUS_LIBRARY_NAME "PNeXus")
configure_file("PNeXus.pc.in" "PNeXus.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PNeXus SHARED
  PNeXus.cpp
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PNeXus ${NEXUS_LIBRARY})

#--- install PNeXus solib -----------------------------------------------------
install(TARGETS PNeXus DESTINATION lib)

#--- install PNeXus header ----------------------------------------------------
install(FILES PNeXus.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PNeXus.pc
  DESTINATION lib/pkgconfig
)
