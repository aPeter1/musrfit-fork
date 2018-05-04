# - mud library ---------------------------------------------------------------

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(MUD_VERSION "1.0.0")
set(MUD_LIBRARY_NAME "mud")
configure_file("mud.pc.in" "mud.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(mud SHARED
  mud_all.c
  mud.c
  mud_encode.c
  mud_friendly.c
  mud_gen.c
  mud_new.c
  mud_tri_ti.c
)

#--- install mud solib --------------------------------------------------------
install(TARGETS mud DESTINATION lib) 

#--- install mud header -------------------------------------------------------
install(FILES mud.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/mud.pc
  DESTINATION lib/pkgconfig
)
