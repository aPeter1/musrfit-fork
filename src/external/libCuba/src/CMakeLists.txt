# - cuba library --------------------------------------------------------------

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(CUBA_VERSION "4.2.0")
set(CUBA_LIBRARY_NAME "cuba")
configure_file("cuba.pc.in" "cuba.pc" @ONLY)

add_subdirectory(cuhre)
add_subdirectory(divonne)
add_subdirectory(suave)
add_subdirectory(vegas)

#--- lib creation -------------------------------------------------------------
add_library(cuba SHARED
  $<TARGET_OBJECTS:cuhre>
  $<TARGET_OBJECTS:divonne>
  $<TARGET_OBJECTS:suave>
  $<TARGET_OBJECTS:vegas>
)

#--- install cuba solib -------------------------------------------------------
install(TARGETS cuba DESTINATION lib)

#--- install cuba header ------------------------------------------------------
install(
  FILES 
    cuba.h 
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cuba.pc
  DESTINATION lib/pkgconfig
)
